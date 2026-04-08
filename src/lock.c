#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>
#include "lock.h"

int lock_acquire(int fd) {
    if (flock(fd, LOCK_EX) == 0) {
        return 0;
    }
    return -1;
}

int lock_release(int fd) {
    if (flock(fd, LOCK_UN) == 0) {
        return 0;
    }
    return -1;
}

static int global_lock_fd = -1;

int global_lock_acquire(void) {
    if (global_lock_fd != -1) return 0; // Already locked
    global_lock_fd = open("/etc/ssh-firewall/firewall.lock", O_CREAT | O_RDWR, 0600);
    if (global_lock_fd < 0) return -1;
    
    if (flock(global_lock_fd, LOCK_EX) == 0) {
        return 0;
    }
    close(global_lock_fd);
    global_lock_fd = -1;
    return -1;
}

int global_lock_release(void) {
    if (global_lock_fd >= 0) {
        flock(global_lock_fd, LOCK_UN);
        close(global_lock_fd);
        global_lock_fd = -1;
        return 0;
    }
    return -1;
}