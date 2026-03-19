#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "firewall.h"

int firewall_allow(const char *ip) {
    pid_t pid = fork();
    if (pid == 0) {
        /* PROSES ANAK: Jalankan UFW */
        /* Kita pake sudo karena UFW butuh akses root */
        char *args[] = {
            "sudo", "ufw", "allow", "from", (char *)ip, NULL
        };
        execvp("sudo", args);

        /* Kalau sampai baris ini, berarti execvp GAGAL */
        exit(1);
    } else if (pid > 0) {
        /* PROSES INDUK: Tunggu anak selesai */
        int status;
        waitpid(pid, &status, 0);
        return (WIFEXITED(status) && WEXITSTATUS(status) == 0) ? 0 : -1;
    } else {
        /* Fork gagal */
        return -1;
    }
}

int firewall_deny(const char *ip) {
    pid_t pid = fork();

    if (pid == 0) {
        /* PROSES ANAK: Jalankan UFW */
        /* Kita pake sudo karena UFW butuh akses root */
        char *args[] = {
            "sudo", "ufw", "delete", "allow", "from", (char *)ip, NULL
        };
        execvp("sudo", args);

        /* Kalau sampai baris ini, berarti execvp GAGAL */
        exit(1);
    } else if (pid > 0) {
        /* PROSES INDUK: Tunggu anak selesai */
        int status;
        waitpid(pid, &status, 0);
        return (WIFEXITED(status) && WEXITSTATUS(status) == 0) ? 0 : -1;
    } else {
        /* Fork gagal */
        return -1;
    }
}