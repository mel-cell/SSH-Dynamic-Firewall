#ifndef LOCK_H
#define LOCK_H

/*
 * lock_acquire:
 * Mengunci file secara eksklusif (Exclusive Lock).
 * Jika ada proses lain yang sedang mengunci file ini, 
 * fungsi ini akan "menunggu" sampai kuncinya dilepaskan (lock-release).
 * 
 * Parameter:
 *   fd - File Descriptor dari file yang mau dikunci (hasil dari fopen() atau open())
 * 
 * Return:
 *   0  jika sukses mengunci
 *  -1  jika gagal
 */

int lock_acquire(int fd);

/*
 * lock_release:
 * Melepaskan kunci file agar bisa digunakan oleh proses lain.
 * 
 * Parameter:
 *   fd - File Descriptor dari file yang sudah dikunci sebelumnya.
 * 
 * Return:
 *   0  jika sukses melepas kunci
 *  -1  jika gagal
 */

int lock_release(int fd);

int global_lock_acquire(void);
int global_lock_release(void);

#endif
