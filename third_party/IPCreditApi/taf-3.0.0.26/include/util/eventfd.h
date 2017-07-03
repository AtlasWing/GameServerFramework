#ifndef ERSATZ_EVENT_FD_H
#define ERSATZ_EVENT_FD_H

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <syscall.h>

#ifdef SYS_eventfd
// since glibc 2.9
#include <sys/eventfd.h>
#else

#if defined __x86_64
#define SYS_eventfd 284
#elif defined __i386
#define SYS_eventfd 323
#else
#error unknown architecture
#endif

#ifndef O_CLOEXEC
#define O_CLOEXEC       02000000        /* set close_on_exec */
#endif

#define EFD_SEMAPHORE (1 << 0)
#define EFD_CLOEXEC   O_CLOEXEC
#define EFD_NONBLOCK  O_NONBLOCK

inline int eventfd(unsigned int initval, int flags)
{
    return syscall(SYS_eventfd, initval, flags);
}

typedef uint64_t eventfd_t;

inline int eventfd_read(int fd, eventfd_t *value)
{
    return read(fd, value, sizeof(*value)) == sizeof(*value) ?  0 : -1;
}

inline int eventfd_write(int fd, eventfd_t value)
{
    return write(fd, &value, sizeof(value)) == sizeof(value) ?  0 : -1;
}
#endif

#endif // ERSATZ_EVENT_FD_H

