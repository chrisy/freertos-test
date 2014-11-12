/** POSIX-like IO Platform
 * \file lib/posixio/fileio.c
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

/// Request private declarations from posixio.h
#define POSIXIO_PRIVATE

#include <config.h>
#include <posixio/posixio.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#undef errno
extern int errno;

/* The POSIX-ish I/O primitives!
 * Here we open a file by looking up the dev for
 * a given path.
 */

/**
 * Implements link() by passing the call through to the link handler
 * of the underlying device of the second filename.
 *
 * @param old The existing file to make a link towards.
 * @param new The new filename that will contain the link.
 * @returns 0 on success, -1 otherwise with errno set to an error value.
 */
int _link(const char *old, const char *new)
{
    // strip out device name from path
    char *device = NULL, *path = NULL;

    if (posixio_split_path_malloc(new, &device, &path) == -1)
        return -1;

    // validate device name
    struct iodev *dev = posixio_getdev(device);
    if (dev == NULL) {
        free(device);
        free(path);
        errno = ENODEV;
        return -1;
    }
    free(device);

    if (dev->link != NULL) {
        int ret = dev->link(path, old);
        free(path);
        return ret;
    }

    free(path);
    errno = EMLINK;
    return -1;
}


/**
 * Implements open() to open a file with a given mode.
 * Several things happen in this function, including validating the
 * filename (this platform requires all filenames be fully qualified
 * and that they begin with /DEVICE/), a file descriptor is assigned
 * and the underlying device asked to "open" the file (the precise
 * nature of which will vary between device implmentations.)
 *
 * @param name The file name to open. This is expected to be in the form
 *      "/DEVICE/FILE".
 * @param flags Flags to pass to the underlying device. Common values that
 *      can be bitwise-ORed into this include O_RDONLY, O_WRONLY, O_RDWR,
 *      O_CREAT, O_APPEND, though device implementations will vary. 0 is an
 *      acceptable value if no flags are required.
 * @param ... Some devices will allow the caller to indicate the initial
 *      mode of a file if open() is creating it. If used, this may be
 *      indicated in this parameter.
 * @returns A new file descriptor on success or -1 on error with an error
 *      value in errno.
 */
int _open(const char *name, int flags, ...)
{
    // strip out device name from path
    char *device = NULL, *path = NULL;

    if (posixio_split_path_malloc(name, &device, &path) == -1)
        return -1;

    posixio_fdlock();

    // validate device name
    struct iodev *dev = posixio_getdev(device);
    if (dev == NULL) {
        posixio_fdunlock();
        free(device);
        free(path);
        errno = ENODEV;
        return -1;
    }

    // get an fd
    int fd = posixio_newfd();
    if (fd == -1) {
        posixio_fdunlock();
        free(device);
        free(path);
        return -1;
    }

    // allocate a file structure - store it with fd.
    struct iofile *file = malloc(sizeof(struct iofile));
    if (file == NULL) {
        posixio_fdunlock();
        free(device);
        free(path);
        errno = ENOMEM;
        return -1;
    }

    // store the details
    file->name = path;
    file->dev = dev;
    file->flags = flags;
    file->fh = NULL;

    // if the device has an open handler, use it
    if (dev->open != NULL) {
        va_list ap;
        va_start(ap, flags);
        file->fh = dev->open(path, flags, ap);
        va_end(ap);
        if (file->fh == NULL) {
            posixio_fdunlock();
            free(device);
            free(path);
            free(file);
            return -1;
        }
    }

    // store the file data
    if (posixio_setfd(fd, file) == -1) {
        posixio_fdunlock();
        free(device);
        free(path);
        free(file);
        return -1;
    }

    posixio_fdunlock();
    free(device);
    // path and file are retained so are not free()ed here

    return fd;
}


/**
 * Implements stat() by passing the call through to the stat handler
 * of the underlying device of the given filename.
 *
 * @param file The file name that should be stat()ed.
 * @param st A struct stat into which the results of the operation are placed.
 * @returns 0 on success, -1 otherwise with errno set to an error value.
 */
int _stat(const char *file, struct stat *st)
{
    // strip out device name from path
    char *device = NULL, *path = NULL;

    if (posixio_split_path_malloc(file, &device, &path) == -1)
        return -1;

    // validate device name
    posixio_fdlock();
    struct iodev *dev = posixio_getdev(device);
    if (dev == NULL) {
        posixio_fdunlock();
        free(device);
        free(path);
        errno = ENODEV;
        return -1;
    }

    if (dev->stat != NULL) {
        int ret = dev->stat(path, st);
        posixio_fdunlock();
        free(device);
        free(path);
        return ret;
    }

    posixio_fdunlock();
    errno = ENOENT;
    return -1;
}


/**
 * Implements unlink(), which usually deletes a file from a filesystem,
 * by calling the unlink handler of the underlying device.
 *
 * @param name The file name of a file to be unlinked.
 * @returns 0 on success or -1 otherwise with an error value in errno.
 */
int _unlink(const char *name)
{
    // strip out device name from path
    char *device = NULL, *path = NULL;

    if (posixio_split_path_malloc(name, &device, &path) == -1)
        return -1;

    // validate device name
    posixio_fdlock();
    struct iodev *dev = posixio_getdev(device);
    if (dev == NULL) {
        posixio_fdunlock();
        free(device);
        free(path);
        errno = ENODEV;
        return -1;
    }

    if (dev->unlink != NULL) {
        int ret = dev->unlink(path);
        posixio_fdunlock();
        free(device);
        free(path);
        return ret;
    }

    posixio_fdunlock();
    errno = EINVAL;
    return -1;
}

// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
