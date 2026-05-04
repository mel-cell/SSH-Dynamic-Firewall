#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "firewall.h"

/* Fungsi internal untuk mengeksekusi perintah */
static int execute_cmd(char *const args[]) {
    pid_t pid = fork();
    if (pid == 0) {
        /* PROSES ANAK: Jalankan perintah */
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
    /* Rule 1: Allow all access from this IP (Host level) */
    char *args1[] = {"sudo", "ufw", "insert", "1", "allow", "from", (char *)ip, NULL};
    execute_cmd(args1);

    /* Rule 2: Allow access to Docker forwarding (Host to Container) */
    char *args2[] = {"sudo", "ufw", "route", "insert", "1", "allow", "from", (char *)ip, NULL};
    execute_cmd(args2);

    /* Rule 3: Direct iptables for DOCKER-USER (Priority) */
    /* Cek dulu apakah rule sudah ada */
    char *check3[] = {"sudo", "iptables", "-C", "DOCKER-USER", "-s", (char *)ip, "-j", "ACCEPT", NULL};
    if (execute_cmd(check3) != 0) {
        /* Masukkan di posisi 1 (setelah rule ESTABLISHED yang kita buat di init) */
        /* Karena kita memasukkan ESTABLISHED di posisi 1 saat init, kita masukkan IP baru di posisi 2 */
        char *args3[] = {"sudo", "iptables", "-I", "DOCKER-USER", "2", "-s", (char *)ip, "-j", "ACCEPT", NULL};
        execute_cmd(args3);
    }

    return 0;
}

int firewall_deny(const char *ip) {
    /* Hapus semua aturan dari IP ini (Host level) */
    char *args1[] = {"sudo", "ufw", "delete", "allow", "from", (char *)ip, NULL};
    execute_cmd(args1);

    /* Hapus semua aturan forwarding dari IP ini */
    char *args2[] = {"sudo", "ufw", "delete", "route", "allow", "from", (char *)ip, NULL};
    execute_cmd(args2);

    /* Hapus dari DOCKER-USER (Hanya jika ada) */
    char *check3[] = {"sudo", "iptables", "-C", "DOCKER-USER", "-s", (char *)ip, "-j", "ACCEPT", NULL};
    if (execute_cmd(check3) == 0) {
        char *args3[] = {"sudo", "iptables", "-D", "DOCKER-USER", "-s", (char *)ip, "-j", "ACCEPT", NULL};
        execute_cmd(args3);
    }

    return 0;
}

int firewall_init(void) {
    /* 
     * Inisialisasi rantai DOCKER-USER.
     * Kita ingin memastikan trafik ke Docker melewati pengecekan kita.
     */
    
    printf("[Firewall] Initializing Docker security patch...\n");

    /* 1. Izinkan koneksi yang sudah terjalin (Hanya jika belum ada) */
    char *check1[] = {"sudo", "iptables", "-C", "DOCKER-USER", "-m", "conntrack", "--ctstate", "RELATED,ESTABLISHED", "-j", "ACCEPT", NULL};
    if (execute_cmd(check1) != 0) {
        char *cmd1[] = {"sudo", "iptables", "-I", "DOCKER-USER", "1", "-m", "conntrack", "--ctstate", "RELATED,ESTABLISHED", "-j", "ACCEPT", NULL};
        execute_cmd(cmd1);
    }

    /* 2. Pastikan trafik internal tetap bisa berjalan (Return ke Docker rules) */
    char *ranges[] = {"172.16.0.0/12", "192.168.0.0/16", "10.0.0.0/8"};
    for (int i = 0; i < 3; i++) {
        char *check[] = {"sudo", "iptables", "-C", "DOCKER-USER", "-s", ranges[i], "-j", "RETURN", NULL};
        if (execute_cmd(check) != 0) {
            char *cmd[] = {"sudo", "iptables", "-A", "DOCKER-USER", "-s", ranges[i], "-j", "RETURN", NULL};
            execute_cmd(cmd);
        }
    }

    /* 3. Drop trafik sisanya (dari internet) secara default */
    char *check5[] = {"sudo", "iptables", "-C", "DOCKER-USER", "-j", "DROP", NULL};
    if (execute_cmd(check5) != 0) {
        char *cmd5[] = {"sudo", "iptables", "-A", "DOCKER-USER", "-j", "DROP", NULL};
        execute_cmd(cmd5);
    }

    printf("[Firewall] Docker security patch active.\n");
    return 0;
}