#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "netutil.h"
#include "ipstore.h"
#include "firewall.h"
#include "logger.h"

int main() {
    const char *user = getenv("PAM_USER");
    const char *type = getenv("PAM_TYPE");
    /*
     * PENTING: Gunakan PAM_RHOST bukan SSH_CLIENT!
     * SSH_CLIENT = shell variable, hanya ada SETELAH session dibuka.
     * PAM_RHOST  = PAM variable, berisi IP remote, dikirim oleh pam_exec.so.
     */
    const char *client = getenv("PAM_RHOST");

    // validasi awal: pastikan data tidak kosong
    if (!user || !type || !client) {
        return 0;
    }

    // parse ip address
    char ip[64];
    if (extract_and_validation_ip(client, ip, sizeof(ip)) != 0) {
        return 0;
    }

    // handle open_session
    if (strcmp(type, "open_session") == 0) {
        if (ip_count_sessions(ip) == 0) {
            /* IP baru: buka firewall, lalu catat ke active list */
            firewall_allow(ip);
            ip_add_entry(ip, user);
            log_event("OPEN", user, ip, "rule ADDED");
        } else {
            /* IP sudah ada dari user lain, cukup catat sesinya */
            ip_add_entry(ip, user);
            log_event("OPEN", user, ip, "IP already active, skipped");
        }
    }

    // handle close_session
    else if (strcmp(type, "close_session") == 0) {
        if (ip_remove_entry(ip, user)) {
            if (ip_count_sessions(ip) == 0) {
                /* Tidak ada sesi lain dari IP ini: tutup firewall */
                firewall_deny(ip);
                log_event("CLOSE", user, ip, "rule REMOVED");
            } else {
                /* Masih ada user lain dari IP ini */
                log_event("CLOSE", user, ip, "other users still active");
            }
        }
    }

    return 0;
}