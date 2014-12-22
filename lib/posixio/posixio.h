/** IO Platform
 * \file lib/posixio/posixio.h
 *
 * \author Chris Luke <chrisy@flirble.org>
 * \copyright Copyright (c) Chris Luke <chrisy@flirble.org>
 *
 * \copyright This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#ifndef _POSIXIO_H
#define _POSIXIO_H

#include <sys/types.h>
#include <stdarg.h>
#include <fcntl.h>

#ifndef POSIXIO_MAX_OPEN_FILES
/// Maximum number of concurrently open files.
#define POSIXIO_MAX_OPEN_FILES 32
#endif
#ifndef POSIXIO_MAX_DEVICES
/// Maxium number of registered devices.
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
    int     (*close)(void *fh);                                 ///< Handler for \c close() of a file on this device.
    off_t   (*lseek)(void *fh, off_t ptr, int dir);             ///< Handler for \c lseek() of a file on this device.
    ssize_t (*read)(void *fh, void *ptr, size_t len);           ///< Handler for \c read() from a file on this device.
    ssize_t (*write)(void *fh, const void *ptr, size_t len);    ///< Handler for \c write() to a file on this device.
    int     (*fstat)(void *fh, struct stat *st);                ///< Handler for \c fstat() of a file on this device.
    int     (*fcntl)(void *fh, int cmd, int arg);               ///< Handler for \c fcntl() of a file on this device.
    int     (*ioctl)(void *fh, unsigned long request, ...);     ///< Handler for \c ioctl() of a file on this device.

    // fileio handlers
    int     (*link)(const char *old, const char *new);      ///< Handler for \c link() targeting filenames on this device.
    void    * (*open)(const char *name, int flags, ...);    ///< Handler for \c open() of a filename on this device.
    int     (*stat)(const char *file, struct stat *st);     ///< Handler for \c stat() of a filename on this device.
    int     (*unlink)(const char *name);                    ///< Handler for \c unlink() of a filename on this device.

    int     flags;                                          ///< Device flags. See \ref POSIXIO_DEVICE_FLAGS.
};

enum POSIXIO_DEVICE_FLAGS {
    POSIXDEV_CHARACTER_STREAM = 0x0001, ///< Device is a character stream.
    POSIXDEV_BLOCK_FILE = 0x0002,       ///< Device is a file-like block device.
    POSIXDEV_ISATTY = 0x0004,           ///< Device is a TTY-type device.
    POSIXDEV_SOCKET = 0x0008,           ///< Device implements a sockets layer.
};


/**
 * The state of an open file.
 */
struct iofile {
    char            *name;  ///< The name of the file.
    struct iodev    *dev;   ///< The device the file resides on.
    void            *fh;    ///< An opaque handle given to us by the dev.
    int             flags;  ///< Any flags given to \c open()
};

/**
 * Known devices. Primarily used to fill st_dev in responses from _stat()
 * and _fstat().
 */
enum POSIXIO_DEVICES {
    DEV_NONE = 0,   ///< No (or unknown) device.
    DEV_USART1,     ///< Serial port 1
    DEV_USART2,     ///< Serial port 2
    DEV_USART3,     ///< Serial port 3
    DEV_UART4,      ///< Serial port 4
    DEV_UART5,      ///< Serial port 5
    DEV_SPI1,       ///< SPI port 1
    DEV_SPI2,       ///< SPI port 2
    DEV_I2C1,       ///< I2C port 1
    DEV_I2C2,       ///< I2C port 2
    DEV_MMC1,       ///< MMC/SDIO port 1
};

/**
 * Our IOCTL's. We lack real ones, but these will do for us, for now.
 */
enum POSIXIO_IOCTLS {
    IOCTL_SETBAUD = 1,
};


int posixio_start(void);

int posixio_split_path(const char *path, char *device, size_t device_len, char *file, size_t file_len);
int posixio_split_path_malloc(const char *path, char **device, char **file);

int _link(const char *old, const char *new);
int _open(const char *name, int flags, ...);
int _stat(const char *file, struct stat *st);
int _unlink(const char *name);

int _close(int fd);
int _isatty(int fd);
off_t _lseek(int fd, off_t ptr, int dir);
ssize_t _read(int fd, void *ptr, size_t len);
ssize_t _write(int fd, const void *ptr, size_t len);
int _fstat(int fd, struct stat *st);
int _fcntl(int fd, int cmd, int arg);
int ioctl(int fd, unsigned long request, ...);
int dup(int fd);
int dup2(int fd, int fd2);


#ifdef POSIXIO_PRIVATE
int posixio_register_dev(struct iodev *dev);
int posixio_newfd(void);
struct iofile *posixio_file_fromfd(int fd);
int posixio_setfd(int fd, struct iofile *file);
struct iodev *posixio_getdev(char *name);
void posixio_fdlock(void);
void posixio_fdunlock(void);
#endif  /* POSIXIO_PRIVATE */

#endif  /* POSIXIO_H */

// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
