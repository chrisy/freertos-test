/** POSIX-like IO Platform
 * \file lib/posixio/posixio.c
 *
 * \author Chris Luke <chrisy@flirble.org>
 * \copyright Copyright (c) Chris Luke <chrisy@flirble.org>
 *
 * \copyright This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

/// Request private declarations from posixio.h
#define POSIXIO_PRIVATE

#include <config.h>

#include <FreeRTOS.h>
#include <semphr.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <real_errno.h>

#include <posixio/posixio.h>
#include <posixio/dev/serial.h>

/// List of the registered devices.
static struct iodev *devs[POSIXIO_MAX_DEVICES];
/// The number of registered devices,
static volatile int dev_count = 0;

/// A semaphore to protect critical sections.
static SemaphoreHandle_t fd_sem;

/// List of currently open files.
static struct iofile *files[POSIXIO_MAX_OPEN_FILES];


/**
 * Initialize the POSIX I/O layer. At minimum, this will reset the list
 * of open files and register a serial port device.
 */
int posixio_start(void)
{
    int i;

    ASSERT((fd_sem = xSemaphoreCreateMutex()));

    for (i = 0; i < POSIXIO_MAX_OPEN_FILES; i++)
        files[i] = NULL;

    if (!posixio_register_serial()) return 0;

    // A hack to fool the linker
    _open("", 0);
    _close(-1);

    return 1;
}


/**
 * Register a device.
 *
 * @param dev The device to register.
 * @returns 0 on success or, on error, -1 with an error code in errno.
 */
int posixio_register_dev(struct iodev *dev)
{
    if (dev_count == POSIXIO_MAX_DEVICES) {
        errno = ENOENT;
        return -1;
    }

    posixio_fdlock();
    devs[dev_count] = dev;
    dev_count++;
    posixio_fdunlock();

    return 0;
}


/**
 * Find and return an unsused file descriptor.
 * Note that this does not reserve the returned descriptor; A caller
 * may do that by calling posixio_setfd().
 * This function assumes that the current task holds posixio_fdlock().
 * This is an internal function.
 *
 * @returns A new file descriptor on success, or -1 with an error code in errno.
 */
int posixio_newfd(void)
{
    // assumes we have fdlock()
    int i;

    // this starts at 3 to leave from for stdin, out, err
    for (i = 3; i < POSIXIO_MAX_OPEN_FILES; i++)
        if (files[i] == NULL)
            return i;
    errno = EMFILE;
    return -1;
}


/**
 * Find the iofile structure for an open file identified by a file
 * descriptor.
 * This function assumes that the current task holds posixio_fdlock().
 * This is an internal function.
 *
 * @param fd The file descriptor to fetch the iofile for.
 * @returns A struct iofile on success or NULL, with an error code in errno,
 *      on error.
 */
struct iofile *posixio_file_fromfd(int fd)
{
    // assumes we have fdlock
    if (fd < 0 || fd >= POSIXIO_MAX_OPEN_FILES) {
        errno = EINVAL;
        return NULL;
    }

    return files[fd];
}


/**
 * Record the iofile structure of an open file against a given file
 * descriptor.
 * This function assumes that the current task holds posixio_fdlock().
 * This is an internal function.
 *
 * @param fd The file descriptor to update. If the descriptor currently
 *      references an open file that file will first be closed.
 * @param file The struct iofile to update the descriptor with. The contents
 *      are copied.
 * @returns 0 on success or, if there is an error, -1 with an error code
 *      in errno.
 */
int posixio_setfd(int fd, struct iofile *file)
{
    // assumes we have fdlock
    if (fd < 0 || fd >= POSIXIO_MAX_OPEN_FILES) {
        errno = EBADF;
        return -1;
    }

    if (file == NULL) {
        // Assume it was closed already
        files[fd] = NULL;
        return 0;
    }

    if (files[fd] != NULL)
        // We need to close the file we're replacing
        close(fd);

    files[fd] = file;

    return 0;
}


/**
 * Attempts to split a string that looks like /DEVICE/FILE
 * into two strings; one for DEVICE and one for FILE. This
 * helper copies the results into two string buffers provided
 * by the caller.
 *
 * @param path The string to split.
 * @param device The string to copy the device name into.
 * @param device_len The maximum length that may be copied into device.
 * @param file The string to copy the file name into.
 * @param file_len The maximum length that may be copied into file.
 * @returns 0 on success, -1 otherwise with errno set to an error code.
 */
int posixio_split_path(const char *path,
                       char *device, size_t device_len,
                       char *file, size_t file_len)
{
    if (path == NULL || path[0] != '/') {
        errno = EINVAL;
        return -1;
    }

    char *p;
    p = strchr(path + 1, '/');
    if (p == NULL) {
        errno = EINVAL;
        return -1;
    }

    size_t len;
    len = p - (path + 1);
    if (len > (device_len - 1))
        len = device_len - 1;
    strncpy((char *)path + 1, device, len);
    device[len - 1] = '\0';

    strncpy(p + 1, file, file_len);
    file[file_len - 1] = '\0';

    return 0;
}


/**
 * Attempts to split a string that looks like /DEVICE/FILE
 * into two strings; one for DEVICE and one for FILE. This
 * helper copies the results into two string buffers allocated
 * using malloc(). It is the callers responsibilty to free() these
 * strings when no longer needed.
 *
 * @param path The string to split.
 * @param device Pointer to the string to copy the device name into.
 * @param file Pointer to the string to copy the file name into.
 * @returns 0 on success, -1 otherwise with errno set to an error code.
 */
int posixio_split_path_malloc(const char *path,
                              char **device, char **file)
{
    if (path == NULL || path[0] != '/') {
        errno = EINVAL;
        return -1;
    }

    char *p;
    p = strchr(path + 1, '/');
    if (p == NULL) {
        errno = EINVAL;
        return -1;
    }

    size_t len;
    len = p - (path + 1);
    *device = malloc(len + 1);
    if (*device == NULL) {
        errno = ENOMEM;
        return -1;
    }
    strncpy(*device, (char *)path + 1, len);
    (*device)[len] = '\0';

    *file = strdup(p + 1);
    if (*file == NULL) {
        free(*device);
        *device = NULL;
        errno = ENOMEM;
        return -2;
    }

    return 0;
}


/**
 * Attempt to find a named device amongst the list of registered devices.
 * This function assumes that the current task holds posixio_fdlock().
 *
 * @param name The name of the device to search for.
 * @returns A struct iodev on success or NULL otherwise.
 */
struct iodev *posixio_getdev(char *name)
{
    // assumes we have fdlock
    int i;

    for (i = 0; i < dev_count; i++)
        if (!strcmp(name, devs[i]->name))
            return devs[i];
    return NULL;
}


/// Acquire the posixio semaphore.
void posixio_fdlock(void)
{
    xSemaphoreTake(fd_sem, portMAX_DELAY);
}

/// Release the posixio semaphore.
void posixio_fdunlock(void)
{
    xSemaphoreGive(fd_sem);
}

// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
