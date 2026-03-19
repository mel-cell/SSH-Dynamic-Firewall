#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define LOG_FILE "/etc/ssh-firewall/firewall.log"
#define STORE_FILE "/etc/ssh-firewall/active_ips.txt"

void print_header(const char *title) {
    time_t now = time(NULL);
    char *time_str = ctime(&now);
    time_str[strlen(time_str)-1] = '\0'; 

    printf("\033[H\033[J"); 
    printf("\033[1;36m============================================================\033[0m\n");
    printf("\033[1;36m      %s - %s       \033[0m\n", title, time_str);
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
    if (count == 0) printf("(\033[0;37mTidak ada user aktif\033[0m)\n");
    printf("\n");
    fclose(f);
}

void show_ufw_status() {
    printf("\033[1;33m--- STATUS FIREWALL (UFW) ---\033[0m\n");
    if (system("sudo ufw status | grep -E 'Action|--|port 1:1000|ALLOW FWD'") != 0) {
        printf("(\033[0;31mGagal memanggil UFW. Pastikan kamu punya akses sudo\033[0m)\n");
    }
    printf("\n");
}

void show_help() {
    printf("SSH Dynamic Firewall CLI Monitor\n\n");
    printf("Penggunaan:\n");
    printf("  whitelist          : Tampilkan monitor ringkas sekali jalan\n");
    printf("  whitelist --live   : Pantau status user secara real-time\n");
    printf("  whitelist -port    : Tampilkan status port firewall sekali jalan\n");
    printf("  whitelist -port --live : Pantau status user & port secara real-time\n\n");
}

int main(int argc, char *argv[]) {
    int live_mode = 0;
    int show_port = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--live") == 0) live_mode = 1;
        else if (strcmp(argv[i], "-port") == 0) show_port = 1;
    }

    if (argc == 1) {
        show_help();
        return 0;
    }

    do {
        char title[64];
        if (live_mode && show_port) strcpy(title, "FULL FIREWALL MONITOR (LIVE)");
        else if (live_mode) strcpy(title, "USER SESSION MONITOR (LIVE)");
        else if (show_port) strcpy(title, "FIREWALL PORT STATUS");
        else strcpy(title, "SSH MONITOR");

        print_header(title);
        show_active_sessions();
        if (show_port) show_ufw_status();

        if (live_mode) {
             printf("\033[0;37m(Mode Live: Update setiap 2 detik. Tekan Ctrl+C untuk keluar)\033[0m\n");
             fflush(stdout);
             sleep(2);
        }
    } while (live_mode);

    return 0;
}
