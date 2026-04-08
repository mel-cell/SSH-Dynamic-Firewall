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
    char target_prefix[128];
    snprintf(target_prefix, sizeof(target_prefix), "%s | ", ip);

    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, target_prefix, strlen(target_prefix)) == 0) {
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

    // Remove internal lock_acquire since we rely on global_lock in main.c
    fprintf(f, "%s | %s | %s\n", ip, user, ts);

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
    char target[150];
    snprintf(target, sizeof(target), "%s | %s |", ip, user);

    bool removed_one = false;
    while (fgets(line, sizeof(line), f)) {
        /* Jika baris mengandung target DAN belum ada yang dihapus, hapus */
        if (!removed_one && strncmp(line, target, strlen(target)) == 0) {
            removed_one = true;
            continue; // Lewati baris ini (hapus)
        }
        /* Sisanya tulis ke file temp */
        fputs(line, temp);
    }

    fclose(f);
    fclose(temp);

    /* Ganti file asli dengan file temp yang sudah "dibersihkan" */
    if (removed_one) {
        rename(IP_STORE_FILE ".tmp", IP_STORE_FILE);
    } else {
        remove(IP_STORE_FILE ".tmp");
    }

    return removed_one;
}