#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define LOG_FILE "/etc/ssh-firewall/firewall.log"
#define STORE_FILE "/etc/ssh-firewall/active_ips.txt"

void clear_screen() {
    /* Gunakan perintah system 'clear' yang lebih universal agar tidak duplikat */
    system("clear");
}

void show_active_sessions() {
    FILE *f = fopen(STORE_FILE, "r");
    if (!f) return;

    printf("\033[1;33m>>> PENGGUNA TERHUBUNG\033[0m\n");
    char line[256];
    int count = 0;
    while (fgets(line, sizeof(line), f)) {
        char ip[64], user[64], timestamp[128];
        if (sscanf(line, "%[^|]| %[^|]| %[^\n]", ip, user, timestamp) == 3) {
            printf(" \033[1;32m●\033[0m %-15s | %-12s | %s\n", user, ip, timestamp);
            count++;
        }
    }
    if (count == 0) printf(" (Server kosong)\n");
    printf("\n");
    fclose(f);
}

void show_last_logs() {
    if (access(LOG_FILE, F_OK) == 0) {
        printf("\033[1;33m>>> LOG AKTIVITAS (5 TERAKHIR)\033[0m\n");
        system("tail -n 5 /etc/ssh-firewall/firewall.log | sed 's/^/  /'");
        printf("\n");
    }
}

void show_ufw_status() {
    printf("\033[1;33m>>> STATUS FIREWALL (UFW)\033[0m\n");
    system("sudo ufw status | grep -E 'ALLOW|Action|--|port 1:1000|ALLOW FWD' | sed 's/^/  /'");
    printf("\n");
}

int main(int argc, char *argv[]) {
    int live_mode = 0, show_port = 0, show_logs = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--live") == 0) live_mode = 1;
        else if (strcmp(argv[i], "-port") == 0) show_port = 1;
        else if (strcmp(argv[i], "-logs") == 0) show_logs = 1;
    }

    if (argc == 1) {
        printf("Gunakan: whitelist [--live] [-port] [-logs]\n");
        return 0;
    }

    do {
        if (live_mode) clear_screen();
        
        show_active_sessions();
        if (show_logs) show_last_logs();
        if (show_port) show_ufw_status();

        if (live_mode) {
             printf("\033[0;37m(Memonitor... Tekan CTRL+C untuk berhenti)\033[0m\n");
             fflush(stdout);
             sleep(2);
        }
    } while (live_mode);

    return 0;
}
