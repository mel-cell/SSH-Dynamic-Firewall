#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "ipstore.h"
#include "lock.h"

int ip_count_sessions(const char *ip) {
    FILE *f = fopen(IP_STORE_FILE, "r");
    if (!f) return 0;

    int count = 0;
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, ip)) {
            count++;
        }
    }
    fclose(f);
    return count;
}

bool ip_add_entry(const char *ip, const char *user) {
    time_t now;
    time(&now);
    struct tm *t = localtime(&now);
    char ts[64];
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", t);

    FILE *f = fopen(IP_STORE_FILE, "a");
    if (!f) return false;

    int fd = fileno(f);
    if (lock_acquire(fd) != 0) {
        fclose(f);
        return false;
    }

    fprintf(f, "%s | %s | %s\n", ip, user, ts);

    lock_release(fd);
    fclose(f);
    return true;
}

bool ip_remove_entry(const char *ip, const char *user) {
    FILE *f = fopen(IP_STORE_FILE, "r");
    if (!f) return false;

    /* Buat file sementara: active_ips.txt.tmp */
    FILE *temp = fopen(IP_STORE_FILE ".tmp", "w");
    if (!temp) {
        fclose(f);
        return false;
    }

    char line[256];
    char target[128];
    sprintf(target, "%s | %s", ip, user);

    bool found = false;
    while (fgets(line, sizeof(line), f)) {
        /* Jika baris TIDAK mengandung target, tulis ke file temp */
        if (strstr(line, target) == NULL) {
            fputs(line, temp);
        } else {
            found = true;
        }
    }

    fclose(f);
    fclose(temp);

    /* Ganti file asli dengan file temp yang sudah "dibersihkan" */
    if (found) {
        rename(IP_STORE_FILE ".tmp", IP_STORE_FILE);
    } else {
        remove(IP_STORE_FILE ".tmp");
    }

    return found;
}