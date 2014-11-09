/** IO Platform
 * \file lib/posixio/fdio.c
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#define POSIXIO_PRIVATE

#include <config.h>
#include <posixio/posixio.h>

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
    posixio_fdlock();

    struct iofile *file = posixio_file_fromfd(fd);

    if (file == NULL) {
        posixio_fdunlock();
        errno = EBADF;
        return -1;
    }

    // Look for a handler
    if (file->dev->close != NULL)
        file->dev->close(file->fh);

    int ret = posixio_setfd(fd, NULL);

    posixio_fdunlock();

    return ret;
}

int _isatty(int fd)
{
    posixio_fdlock();

    struct iofile *file = posixio_file_fromfd(fd);

    if (file == NULL) {
        posixio_fdunlock();
        errno = EBADF;
        return -1;
    }

    int ret = (file->dev->flags & POSIXDEV_ISATTY) == POSIXDEV_ISATTY;

    posixio_fdunlock();
    return ret;
}

off_t _lseek(int fd, off_t ptr, int dir)
{
    posixio_fdlock();

    struct iofile *file = posixio_file_fromfd(fd);

    if (file == NULL) {
        posixio_fdunlock();
        errno = EBADF;
        return -1;
    }

    // Look for a handler
    if (file->dev->lseek != NULL) {
        int ret = file->dev->lseek(file->fh, ptr, dir);
        posixio_fdunlock();
        return ret;
    }

    posixio_fdunlock();
    errno = EINVAL;
    return -1;
}

ssize_t _read(int fd, void *ptr, size_t len)
{
    posixio_fdlock();

    struct iofile *file = posixio_file_fromfd(fd);

    if (file == NULL) {
        posixio_fdunlock();
        errno = EBADF;
        return -1;
    }

    // Look for a handler
    if (file->dev->read != NULL) {
        int ret = file->dev->read(file->fh, ptr, len);
        posixio_fdunlock();
        return ret;
    }

    posixio_fdunlock();
    errno = EINVAL;
    return -1;
}

ssize_t _write(int fd, const void *ptr, size_t len)
{
    posixio_fdlock();

    struct iofile *file = posixio_file_fromfd(fd);

    if (file == NULL) {
        posixio_fdunlock();
        errno = EBADF;
        return -1;
    }

    // Look for a handler
    if (file->dev->write != NULL) {
        int ret = file->dev->write(file->fh, ptr, len);
        posixio_fdunlock();
        return ret;
    }

    errno = EINVAL;
    return -1;
}

int _fstat(int fd, struct stat *st)
{
    posixio_fdlock();

    struct iofile *file = posixio_file_fromfd(fd);

    if (file == NULL) {
        posixio_fdunlock();
        errno = EBADF;
        return -1;
    }

    if (file->dev->stat != NULL) {
        int ret = file->dev->fstat(file->fh, st);
        posixio_fdunlock();
        return ret;
    }

    posixio_fdunlock();
    errno = ENOENT;
    return -1;
}

int dup(int fd)
{
    posixio_fdlock();

    struct iofile *file = posixio_file_fromfd(fd);

    if (file == NULL) {
        posixio_fdunlock();
        errno = EBADF;
        return -1;
    }

    // Get a new descriptor
    int fd2 = posixio_newfd();
    if (fd2 == -1) {
        posixio_fdunlock();
        return -1;
    }

    // allocate a file structure - store it with fd.
    struct iofile *file2 = malloc(sizeof(struct iofile));
    if (file2 == NULL) {
        posixio_fdunlock();
        errno = ENOMEM;
        return -1;
    }

    // copy it
    memcpy(file2, file, sizeof(struct iofile));

    // if the device has an open handler, use it
    if (file2->dev->open != NULL) {
        file2->fh = file2->dev->open(file2->name, file2->flags);
        if (file2->fh == NULL) {
            posixio_fdunlock();
            free(file2);
            return -1;
        }
    }

    // store the file data
    if (posixio_setfd(fd2, file2) == -1) {
        posixio_fdunlock();
        free(file2);
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

    posixio_fdlock();

    struct iofile *file = posixio_file_fromfd(fd);

    if (file == NULL) {
        posixio_fdunlock();
        errno = EBADF;
        return -1;
    }

    // allocate a file structure - store it with fd.
    struct iofile *file2 = malloc(sizeof(struct iofile));
    if (file2 == NULL) {
        posixio_fdunlock();
        errno = ENOMEM;
        return -1;
    }

    // copy it
    memcpy(file2, file, sizeof(struct iofile));

    // if the device has an open handler, use it
    if (file2->dev->open != NULL) {
        file2->fh = file2->dev->open(file2->name, file2->flags);
        if (file2->fh == NULL) {
            posixio_fdunlock();
            free(file2);
            return -1;
        }
    }

    // store the file data
    // this will close any pre-existing fd2
    if (posixio_setfd(fd2, file2) == -1) {
        posixio_fdunlock();
        free(file2);
        return -1;
    }

    posixio_fdunlock();

    return fd2;
}

// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
