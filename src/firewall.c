#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "firewall.h"

/* Fungsi internal untuk mengeksekusi perintah UFW */
static int execute_ufw(char *const args[]) {
    pid_t pid = fork();
    if (pid == 0) {
        /* PROSES ANAK: Jalankan UFW */
        /* Kita tidak perlu mencetak output ke terminal */
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

int firewall_allow(const char *ip) {
    /* Rule 1: Allow access to system ports 1-1000 */
    /* Kita gunakan 'insert 1' agar aturannya ada di urutan teratas (pioritas tinggi) */
    char *args1[] = {"sudo", "ufw", "insert", "1", "allow", "from", (char *)ip, "to", "any", "port", "1:1000", NULL};
    execute_ufw(args1);

    /* Rule 2: Allow access to Docker forwarding (Host to Container) */
    char *args2[] = {"sudo", "ufw", "route", "allow", "from", (char *)ip, NULL};
    execute_ufw(args2);

    return 0;
}

int firewall_deny(const char *ip) {
    /* Hapus aturan port 1-1000 */
    char *args1[] = {"sudo", "ufw", "delete", "allow", "from", (char *)ip, "to", "any", "port", "1:1000", NULL};
    execute_ufw(args1);

    /* Hapus aturan Docker forwarding */
    char *args2[] = {"sudo", "ufw", "delete", "route", "allow", "from", (char *)ip, NULL};
    execute_ufw(args2);

    return 0;
}