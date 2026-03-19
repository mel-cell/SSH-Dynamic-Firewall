#ifndef IPSTORE_H
#define IPSTORE_H

#include <stdbool.h>
#define IP_STORE_FILE "/etc/ssh-firewall/active_ips.txt"


/*
 * ip_add_entry:
 * Menambah baris baru ke active_ips.txt (misal: "1.2.3.4 | senvada | 2024-...")
 */

bool ip_add_entry(const char *ip, const char *user);

/*
 * ip_remove_entry:
 * Menghapus hanya satu baris yang berisi IP dan USER tertentu.
 */

bool ip_remove_entry(const char *ip, const char *user);

/*
 * ip_count_sessions:
 * Menghitung ada berapa sesi aktif untuk sebuahi IP tertentu.
 * Digunakan saat logout: kalau hasilnya > 0, jangan hapus firewall.
 */

int ip_count_sessions(const char *ip);

#endif