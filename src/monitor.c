#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LOG_FILE "/etc/ssh-firewall/firewall.log"
#define STORE_FILE "/etc/ssh-firewall/active_ips.txt"

void print_banner() {
    printf("\033[1;36m========================================\033[0m\n");
    printf("\033[1;36m      SSH FIREWALL MONITOR STATUS       \033[0m\n");
    printf("\033[1;36m========================================\033[0m\n\n");
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
        /* Format di active_ips.txt: ip | user | timestamp */
        if (sscanf(line, "%[^|]| %[^|]| %[^\n]", ip, user, timestamp) == 3) {
            printf("\033[1;32m%-20s\033[0m | %-15s | %s\n", ip, user, timestamp);
            count++;
        }
    }
    
    if (count == 0) {
        printf("(\033[0;37mTidak ada user yang aktif\033[0m)\n");
    }
    printf("\n");
    fclose(f);
}

void show_last_logs() {
    printf("\033[1;33m--- LOG TERAKHIR (5 Baris) ---\033[0m\n");
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "tail -n 5 %s", LOG_FILE);
    system(cmd);
    printf("\n");
}

void show_ufw_status() {
    printf("\033[1;33m--- STATUS FIREWALL (UFW) ---\033[0m\n");
    system("sudo ufw status numbered | grep -E 'ALLOW|Action|--'");
}

int main() {
    print_banner();
    show_active_sessions();
    show_last_logs();
    show_ufw_status();
    return 0;
}
