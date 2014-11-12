/** IO Platform
 * \file lib/posixio/posixio.h
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#ifndef _POSIXIO_H
#define _POSIXIO_H

#include <sys/types.h>
#include <stdarg.h>

#ifndef POSIXIO_MAX_OPEN_FILES
/// Maximum number of concurrently open files.
#define POSIXIO_MAX_OPEN_FILES 32
#endif
#ifndef POSIXIO_MAX_DEVICES
/// Maxium number of known devices.
#define POSIXIO_MAX_DEVICES 32
#endif

struct stat;

/**
 * The definition of an IO device, specifically the handlers it provides
 * to handle I/O tasks.
 */
struct iodev {
    char    *name; /// The name of the device.

    // fdio handlers
    int     (*close)(void *fh);                                 ///< Handler for close() of a file on this device.
    off_t   (*lseek)(void *fh, off_t ptr, int dir);             ///< Handler for lseek() of a file on this device.
    ssize_t (*read)(void *fh, void *ptr, size_t len);           ///< Handler for read() from a file on this device.
    ssize_t (*write)(void *fh, const void *ptr, size_t len);    ///< Handler for write() to a file on this device.
    int     (*fstat)(void *fh, struct stat *st);                ///< Handler for fstat() of a file on this device.

    // fileio handlers
    int     (*link)(const char *old, const char *new);      ///< Handler for link() targetting filenames on this device.
    void    * (*open)(const char *name, int flags, ...);    ///< Handler for open() of a filename on this device.
    int     (*stat)(const char *file, struct stat *st);     ///< Handler for stat() of a filename on this device.
    int     (*unlink)(const char *name);                    ///< Handler for unlink() of a filename on this device.

    int     flags;                                          ///< Device flags. See POSIXDEV_*.
};

/// Device is a TTY-type device.
#define POSIXDEV_ISATTY     0x01

/**
 * The state of an open file.
 */
struct iofile {
    char            *name;  ///< The name of the file.
    struct iodev    *dev;   ///< The device the file resides on.
    void            *fh;    ///< An opaque handle given to us by the dev.
    int             flags;  ///< Any flags given to open()
};

/**
 * Known device types.
 */
enum POSIXIO_DEVICES {
    DEV_NONE,   ///< No (or unknown) device.
    DEV_USART1, ///< Serial port 1
    DEV_USART2, ///< Serial port 2
    DEV_USART3, ///< Serial port 3
    DEV_UART4,  ///< Serial port 4
    DEV_UART5,  ///< Serial port 5
};

int posixio_start(void);

int posixio_split_path(const char *path, char *device, size_t device_len, char *file, size_t file_len);
int posixio_split_path_malloc(const char *path, char **device, char **file);

#ifdef POSIXIO_PRIVATE
int posixio_register_dev(struct iodev *dev);
int posixio_newfd(void);
struct iofile *posixio_file_fromfd(int fd);
int posixio_setfd(int fd, struct iofile *file);
struct iodev *posixio_getdev(char *name);
void posixio_fdlock(void);
void posixio_fdunlock(void);
int _open(const char *name, int flags, ...);
int _close(int fd);

#endif

#endif /* POSIXIO_H */

// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
