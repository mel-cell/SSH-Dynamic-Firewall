#ifndef FIREWALL_H
#define FIREWALL_H

/*
 * firewall_allow:
 * Memanggil UFW untuk mengizinkan (Allow) IP tertentu.
 */

 int firewall_allow(const char *ip);

 /*
 * firewall_deny:
 * Memanggil UFW untuk menghapus (Delete) rule allow buat IP tersebut.
 */

 int firewall_deny(const char *ip);

#endif