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
#include <string.h>

#undef errno
extern int errno;

/* The POSIX-ish I/O primitives!
 * Here we just indirect through the dev structure associated
 * with open files.
 */
int _close(int fd)
{
    struct iofile *file = posixio_file_fromfd(fd);

    if (file == NULL) {
        errno = EBADF;
        return -1;
    }

    posixio_fdlock();

    // Look for a handler
    if (file->dev->close != NULL)
        file->dev->close(file->fh);

    posixio_fdunlock();

    int ret = posixio_setfd(fd, NULL);

    return ret;
}

int _isatty(int fd)
{
    struct iofile *file = posixio_file_fromfd(fd);

    if (file == NULL) {
        errno = EBADF;
        return -1;
    }
    return (file->dev->flags & POSIXDEV_ISATTY) == POSIXDEV_ISATTY;
}

off_t _lseek(int fd, off_t ptr, int dir)
{
    struct iofile *file = posixio_file_fromfd(fd);

    if (file == NULL) {
        errno = EBADF;
        return -1;
    }

    // Look for a handler
    if (file->dev->lseek != NULL)
        return file->dev->lseek(file->fh, ptr, dir);

    errno = EINVAL;
    return -1;
}

ssize_t _read(int fd, void *ptr, size_t len)
{
    struct iofile *file = posixio_file_fromfd(fd);

    if (file == NULL) {
        errno = EBADF;
        return -1;
    }

    // Look for a handler
    if (file->dev->read != NULL)
        return file->dev->read(file->fh, ptr, len);

    errno = EINVAL;
    return -1;
}

ssize_t _write(int fd, const void *ptr, size_t len)
{
    struct iofile *file = posixio_file_fromfd(fd);

    if (file == NULL) {
        errno = EBADF;
        return -1;
    }

    // Look for a handler
    if (file->dev->write != NULL)
        return file->dev->write(file->fh, ptr, len);

    errno = EINVAL;
    return -1;
}

int dup(int fd)
{
    struct iofile *file = posixio_file_fromfd(fd);

    if (file == NULL) {
        errno = EBADF;
        return -1;
    }

    posixio_fdlock();

    // Get a new descriptor
    int fd2 = posixio_newfd();
    if (fd2 == -1) {
        posixio_fdunlock();
        return -1;
    }

    // allocate a file structure - store it with fd.
    struct iofile *file2 = malloc(sizeof(struct iofile));
    if (file2 == NULL) {
        errno = ENOMEM;
        posixio_fdunlock();
        return -1;
    }

    // copy it
    memcpy(file2, file, sizeof(struct iofile));

    // if the device has an open handler, use it
    if (file2->dev->open != NULL) {
        file2->fh = file2->dev->open(file2->name, file2->flags);
        if (file2->fh == NULL) {
            free(file2);
            posixio_fdunlock();
            return -1;
        }
    }

    // store the file data
    if (posixio_setfd(fd2, file2) == -1) {
        free(file2);
        posixio_fdunlock();
        return -1;
    }

    posixio_fdunlock();

    return fd2;
}

int dup2(int fd, int fd2)
{
    if (fd < 0 || fd >= POSIXIO_MAX_OPEN_FILES) {
        errno = EBADF;
        return -1;
    }

    // Simple optimization
    if (fd == fd2) return fd;

    struct iofile *file = posixio_file_fromfd(fd);

    if (file == NULL) {
        errno = EBADF;
        return -1;
    }

    posixio_fdlock();

    // allocate a file structure - store it with fd.
    struct iofile *file2 = malloc(sizeof(struct iofile));
    if (file2 == NULL) {
        errno = ENOMEM;
        posixio_fdunlock();
        return -1;
    }

    // copy it
    memcpy(file2, file, sizeof(struct iofile));

    // if the device has an open handler, use it
    if (file2->dev->open != NULL) {
        file2->fh = file2->dev->open(file2->name, file2->flags);
        if (file2->fh == NULL) {
            free(file2);
            posixio_fdunlock();
            return -1;
        }
    }

    // store the file data
    // this will close any pre-existing fd2
    if (posixio_setfd(fd2, file2) == -1) {
        free(file2);
        posixio_fdunlock();
        return -1;
    }

    posixio_fdunlock();

    return fd2;
}
