#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "netutil.h"

int extract_and_validation_ip(const char *ssh_client, char *ip_out, size_t len) {
    if (ssh_client == NULL || ip_out == NULL || len == 0) {
        return -1;
    }

    const char *space = strchr(ssh_client, ' ');
    size_t ip_len;

    if (space == NULL) {
        ip_len = strlen(ssh_client);
    } else {
        ip_len = space - ssh_client;
    }

    if (ip_len >= len) {
        return -1;
    }

    strncpy(ip_out, ssh_client, ip_len);
    ip_out[ip_len] = '\0';

    /* Validasi IPv4 */
    struct in_addr addr4;
    if (inet_pton(AF_INET, ip_out, &addr4) == 1) {
        return 0;
    }

    /* Validasi IPv6 */
    struct in6_addr addr6;
    if (inet_pton(AF_INET6, ip_out, &addr6) == 1) {
        return 0;
    }

    return -1;
}
