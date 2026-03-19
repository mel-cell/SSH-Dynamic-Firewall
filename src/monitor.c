#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define LOG_FILE "/etc/ssh-firewall/firewall.log"
#define STORE_FILE "/etc/ssh-firewall/active_ips.txt"

void print_banner() {
    time_t now = time(NULL);
    char *time_str = ctime(&now);
    time_str[strlen(time_str)-1] = '\0'; // hapus newline

    printf("\033[H\033[J"); // Clear screen dan gerakkan kursor ke atas
    printf("\033[1;36m============================================================\033[0m\n");
    printf("\033[1;36m      SSH FIREWALL LIVE MONITOR - %s       \033[0m\n", time_str);
    printf("\033[1;36m============================================================\033[0m\n\n");
}

void show_active_sessions() {
    FILE *f = fopen(STORE_FILE, "r");
    if (!f) {
        printf("[\033[1;31m!\033[0m] Tidak bisa membuka file active_ips.txt\n");
        return;
    }

    printf("\033[1;33m--- USER AKTIF SAAT INI ---\033[0m\n");
    printf("%-20s | %-15s | %s\n", "IP ADDRESS", "USERNAME", "LOGIN TIME");
    printf("------------------------------------------------------------\n");

    char line[256];
    int count = 0;
    while (fgets(line, sizeof(line), f)) {
        char ip[64], user[64], timestamp[128];
        if (sscanf(line, "%[^|]| %[^|]| %[^\n]", ip, user, timestamp) == 3) {
            printf("\033[1;32m%-20s\033[0m | %-15s | %s\n", ip, user, timestamp);
            count++;
        }
    }
    
    if (count == 0) {
        printf("(\033[0;37mServer Sepi - Tidak ada user aktif\033[0m)\n");
    }
    printf("\n");
    fclose(f);
}

void show_last_logs() {
    printf("\033[1;33m--- LOG TERAKHIR (5 Baris) ---\033[0m\n");
    char cmd[256];
    /* Kita pastikan file log ada, kalau tidak tail bakal error */
    if (access(LOG_FILE, F_OK) == 0) {
        snprintf(cmd, sizeof(cmd), "tail -n 5 %s", LOG_FILE);
        system(cmd);
    } else {
        printf("(Belum ada log)\n");
    }
    printf("\n");
}

void show_ufw_status() {
    printf("\033[1;33m--- STATUS FIREWALL (UFW) UNTUK IP AKTIF ---\033[0m\n");
    /* Cukup tunjukkan rule yang spesifik agar tidak terlalu panjang di layar */
    system("sudo ufw status | grep -E 'Action|--|port 1:1000|ALLOW FWD'");
}

int main(int argc, char *argv[]) {
    int live_mode = 0;
    
    /* Jika dijalankan dengan './ssh-monitor --live' */
    if (argc > 1 && strcmp(argv[1], "--live") == 0) {
        live_mode = 1;
    }

    do {
        print_banner();
        show_active_sessions();
        show_last_logs();
        show_ufw_status();

        if (live_mode) {
            printf("\n\033[0;37m(Mode Live: Update setiap 2 detik. Tekan Ctrl+C untuk keluar)\033[0m\n");
            fflush(stdout);
            sleep(2);
        }
    } while (live_mode);

    return 0;
}
