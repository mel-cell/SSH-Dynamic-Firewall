#include <sys/file.h>
#include <unistd.h>
#include "lock.h"

int lock_acquire(int fd) {
    /* 
     * LOCK_EX: Exclusive Lock — cuma 1 orang yang boleh masuk.
     * flock akan "nge-block" (nunggu) proses kamu sampai kunci tersedia. 
     */
    if (flock(fd, LOCK_EX) == 0) {
        return 0;
    }
    return -1;
}

int lock_release(int fd) {
    /* 
     * LOCK_UN: Unlock — melepaskan kunci.
     */
    if (flock(fd, LOCK_UN) == 0) {
        return 0;
    }
    return -1;
}