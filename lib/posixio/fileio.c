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
int _link(const char *old, const char *new)
{
    // strip out device name from path
    char *device = NULL, *path = NULL;

    if (posixio_split_path_malloc(old, &device, &path) == -1)
        return -1;

    // validate device name
    struct iodev *dev = posixio_getdev(device);
    if (dev == NULL) {
        free(device);
        free(path);
        errno = ENODEV;
        return -1;
    }

    if (dev->link != NULL) {
        int ret = dev->link(path, new);
        free(device);
        free(path);
        return ret;
    }

    errno = EMLINK;
    return -1;
}

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

    return fd;
}

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
