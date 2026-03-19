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

    /* DEBUG SEMENTARA - hapus setelah sistem jalan */
    fprintf(stderr, "DEBUG: PAM_USER='%s' PAM_TYPE='%s' PAM_RHOST='%s'\n",
        user ? user : "(null)",
        type ? type : "(null)",
        client ? client : "(null)");

    // validasi awal: pastikan data tidak kosong
    if (!user || !type || !client) {
        fprintf(stderr, "DEBUG: EARLY EXIT - ada variable yang null!\n");
        return 0;
    }

    // parse ip address
    char ip[64];
    if (extract_and_validation_ip(client, ip, sizeof(ip)) != 0) {
        log_event("ERROR", user, "-", "Invalid IP");
        return 0;
    }

    // handle open_session
    if (strcmp(type, "open_session") == 0) {
        if (ip_count_sessions(ip) == 0) {
            firewall_allow(ip);
            log_event("OPEN", user, ip, "RULE ADDED");
        }
        ip_add_entry(ip, user);
    }

    // handle close_session
    if (strcmp(type, "close_session") == 0) {
        if (ip_remove_entry(ip, user)) {
            if (ip_count_sessions(ip) == 0) {
                firewall_deny(ip);
                log_event("CLOSE", user, ip, "RULE REMOVED");
            } else {
                log_event("CLOSE", user, ip, "RULE KEHITUNG");
            }
        }
    }

    return 0;
}