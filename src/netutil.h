#ifndef NETUTIL_H
#define NETUTIL_H

#include <stddef.h>

/*
 * Fungsi extract_and_validate_ip:
 * Mengambil IP address dari string format SSH_CLIENT ("IP PORT_SRC PORT_DST").
 * Memvalidasi apakah IP tersebut adalah IPv4 atau IPv6 yang valid.
 * 
 * Parameter:
 *   ssh_client - String asli dari environment variable SSH_CLIENT
 *   ip_out     - Buffer tempat kita akan menyimpan IP yang sudah dibersihkan
 *   len        - Ukuran maksimal dari ip_out
 * 
 * Return:
 *   0  jika sukses
 *  -1  jika gagal (format salah atau IP tidak valid)
 */

int extract_and_validation_ip(const char *ssh_client, char *ip_out, size_t len);

#endif