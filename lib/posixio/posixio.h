/** IO Platform
 * vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
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
#define POSIXIO_MAX_OPEN_FILES 32
#endif
#ifndef POSIXIO_MAX_DEVICES
#define POSIXIO_MAX_DEVICES 32
#endif

struct stat;

// An IO device
struct iodev {
    char    *name;

    // fdio handlers
    int     (*close)(void *fh);
    off_t   (*lseek)(void *fh, off_t ptr, int dir);
    ssize_t (*read)(void *fh, void *ptr, size_t len);
    ssize_t (*write)(void *fh, const void *ptr, size_t len);

    // fileio handlers
    int     (*link)(const char *old, const char *new);
    void    * (*open)(const char *name, int flags, ...);
    int     (*stat)(const char *file, struct stat *st);
    int     (*unlink)(const char *name);

    int     flags;    // device flags
};

#define POSIXDEV_ISATTY     0x01

// An open file
struct iofile {
    char            *name;  // name of the file
    struct iodev    *dev;   // device the file resides on
    void            *fh;    // opaque handle given to us by the dev
    int             flags;  // as per open()
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
#endif

#endif /* POSIXIO_H */
