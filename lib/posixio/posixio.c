/** IO Platform
 * vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
 * \file lib/posixio/io.c
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */
#define POSIXIO_PRIVATE

#include <config.h>
#include <posixio.h>

#include <FreeRTOS.h>
#include <semphr.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include <dev/serial.h>

static struct iodev *devs[POSIXIO_MAX_DEVICES];
static volatile int dev_count = 0;

static SemaphoreHandle_t fd_sem;
static struct iofile *files[POSIXIO_MAX_OPEN_FILES];

extern int _open(const char *name, int flags, ...);
extern int _close(int fd);

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

int posixio_register_dev(struct iodev *dev)
{
    if (dev_count == POSIXIO_MAX_DEVICES) {
        errno = ENOENT;
        return -1;
    }

    devs[dev_count] = dev;
    dev_count++;

    return 0;
}

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

struct iofile *posixio_file_fromfd(int fd)
{
    if (fd < 0 || fd >= POSIXIO_MAX_OPEN_FILES) {
        errno = EINVAL;
        return NULL;
    }

    return files[fd];
}

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
    strncpy((char *)path + 1, *device, len);
    *device[len] = '\0';

    *file = strdup(p + 1);
    if (*file == NULL) {
        free(*device);
        *device = NULL;
        errno = ENOMEM;
        return -2;
    }

    return 0;
}

struct iodev *posixio_getdev(char *name)
{
    int i;

    for (i = 0; i < dev_count; i++)
        if (!strcmp(name, devs[i]->name))
            return devs[i];
    return NULL;
}

void posixio_fdlock(void)
{
    xSemaphoreTake(fd_sem, 1000);
}

void posixio_fdunlock(void)
{
    xSemaphoreGive(fd_sem);
}
