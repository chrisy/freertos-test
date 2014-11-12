/** POSIX-like IO Platform
 * \file lib/posixio/fdio.c
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
#include <string.h>

#undef errno
extern int errno;

/* The POSIX-ish I/O primitives!
 * Here we just indirect through the dev structure associated
 * with open files.
 */

/**
 * Implements close() on an open file by calling the close handler
 * of any underlying device and then removing any association for the
 * file descriptor of the open file.
 *
 * @param fd File descriptor of a file that needs to be closed.
 * @returns 0 on success, -1 otherwise with errno set to an error code.
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


/**
 * Implements isatty() by seeing if POSIXDEV_ISATTY is set in the
 * underlying device flags.
 *
 * @param fd An open file whose device is to be queried.
 * @return 1 if the device is a tty, 0 if not and -1 on error with errno
 *      set to an error value.
 */
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


/**
 * Implements lseek() on an open file by passing the call through to the
 * lseek handler of the underlying device.
 *
 * @param fd File descriptor of an open file to operate on.
 * @param ptr The offset, in bytes, to seek to in the open file.
 * @param dir The direction to seek in the open file, usually SEEK_SET,
 *      SEEK_CUR or SEEK_END.
 * @returns -1 on error, with errno set to an error value. Other values
 *      may be device implementation specific, but POSIX expects that
 *      lseek() returns the resulting offset from the beginning of the file.
 */
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


/**
 * Implements read() on an open file by passing the call through to the
 * read handler of the underlying device.
 *
 * @param fd An open file to operate on.
 * @param ptr A pointer to a region of memory into which bytes read from the
 *      file can be placed.
 * @param len An upper limit on the number of bytes to read from the open
 *      file. This will often be the size of the buffer referred to by ptr.
 * @returns The number of bytes read into ptr or -1 on error with errno set
 *      to an error value. For most types of device a return value of 0
 *      indicates that the file has met an end-of-file condition; in
 *      the case of network devices, this means a stream connection was
 *      terminated remotely.
 */
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


/**
 * Implements write() on an open file by passing the call through to the
 * write handler of the underlying device.
 *
 * @param fd An open file to operate on.
 * @param ptr A pointer to a region of memory from which bytes are written
 *      to the open file.
 * @param len An upper limit on the number of bytes to write to the open
 *      file.
 * @returns The number of bytes written to the open file or -1 on error
 *      with errno set to an error value.
 */
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


/**
 * Implements fstat() on an open file by passing the call through to the
 * fstat handler of the underlying device.
 *
 * @param fd An open file to operate on.
 * @param st A struct stat into which the results of the operation are placed.
 * @returns 0 on success, -1 otherwise with errno set to an error value.
 */
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


/**
 * Duplicates the file descriptor for an open file. This allocates a new
 * file descriptor and clones the references of the open file into it.
 *
 * @param fd An open file to operate on.
 * @returns A new file descriptor or -1 on error with errno set to an error
 *      value.
 */
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


/**
 * Duplicates a file descriptor for an open file into a specified other
 * file descriptor. It closes the second descriptor if it has an open
 * file attached to it and then clones the references of the first descriptor
 * into the second.
 *
 * @param fd An open file to clone.
 * @param fd2 An arbitrary file descriptor value into which fd should be
 *      cloned.
 * @returns fd2 or -1 on error with errno set to an error value.
 */
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
