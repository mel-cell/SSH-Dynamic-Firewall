#include <stdio.h>
#include <time.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/file.h>
#include "logger.h"
#include "lock.h"

#define LOG_FILE "/etc/ssh-firewall/firewall.log"

void log_event(const char *action, const char *user, const char *ip, const char *status) {
    // ambil waktu sekarang
    time_t now;
    time(&now);
    struct tm *t = localtime(&now);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);

    // catat ke sistem jurnal (syslog)
    openlog("ssh-firewall", LOG_PID | LOG_CONS, LOG_AUTH);
    syslog(LOG_INFO, "[%s] %s | %s | %s | %s\n", time_str, action, user, ip, status);
    closelog();

    // catat ke file /etc/ssh-....
    FILE *f = fopen(LOG_FILE, "a"); //a = append
    if (f != NULL) {
        int fd = fileno(f); // ambil file desc buat di kunci

        lock_acquire(fd);

        fprintf(f, "[%s] %s | %s | %s\n", action, time_str, user, ip, status); // check supaya beneran ketulis

        lock_release(fd);
        fclose(f);
    }
}