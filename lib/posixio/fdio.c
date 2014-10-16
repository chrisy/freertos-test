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
 * Here we just indirect through the dev structure associated
 * with open files.
 */
int close(int fd)
{
    struct iofile *file = posixio_file_fromfd(fd);
    if(file == NULL) {
        errno = ENOENT;
        return -1;
    }

    // Look for a handler
    if(file->dev->close != NULL)
        file->dev->close(file->fh);

    return posixio_setfd(fd, NULL);
}

int isatty(int fd)
{
    struct iofile *file = posixio_file_fromfd(fd);
    if(file == NULL) {
        errno = ENOENT;
        return -1;
    }
    return (file->dev->flags & POSIXDEV_ISATTY) == POSIXDEV_ISATTY;
}

off_t lseek(int fd, off_t ptr, int dir)
{
    struct iofile *file = posixio_file_fromfd(fd);
    if(file == NULL) {
        errno = ENOENT;
        return -1;
    }

    // Look for a handler
    if(file->dev->lseek != NULL)
        return file->dev->lseek(file->fh, ptr, dir);

    errno = EINVAL;
    return -1;
}

ssize_t read(int fd, void *ptr, size_t len)
{
    struct iofile *file = posixio_file_fromfd(fd);
    if(file == NULL) {
        errno = ENOENT;
        return -1;
    }

    // Look for a handler
    if(file->dev->read != NULL)
        return file->dev->read(file->fh, ptr, len);

    errno = EINVAL;
    return -1;
}

ssize_t write(int fd, const void *ptr, size_t len)
{
    struct iofile *file = posixio_file_fromfd(fd);
    if(file == NULL) {
        errno = ENOENT;
        return -1;
    }

    // Look for a handler
    if(file->dev->write != NULL)
        return file->dev->write(file->fh, ptr, len);

    errno = EINVAL;
    return -1;
}
