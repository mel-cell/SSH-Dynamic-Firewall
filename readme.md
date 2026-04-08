# SSH Dynamic Firewall (C Implementation)

![image](image.png)

---

Sistem keamanan firewall dinamis berbasis bahasa C yang terintegrasi dengan PAM (Pluggable Authentication Modules). Sistem ini otomatis membuka akses firewall saat user login via SSH dan menutupnya kembali saat logout.

## Fitur Utama
- [x] Auto-whitelist IP saat SSH login berhasil.
- [x] Auto-cleanup: Menutup akses IP saat semua sesi user logout.
- [x] Mendukung IPv4 dan IPv6 secara native.
- [x] Pencegahan Race Condition menggunakan File Locking (flock).
- [x] Keamanan Tinggi: Tanpa shell injection (menggunakan execvp).
- [x] Akses Penuh: Mengizinkan semua port (IN & FWD) untuk IP yang whitelisted.
- [x] Logging: Mencatat aktivitas ke file lokal dan syslog.
- [x] Monitoring CLI: Tool khusus untuk memantau status secara real-time.

## Struktur Project
- `src/main.c`: Entry point yang dipanggil oleh PAM.
- `src/firewall.c`: Logika pengaturan aturan UFW.
- `src/ipstore.c`: Manajemen daftar IP dan user yang aktif.
- `src/monitor.c`: Source code untuk perintah `whitelist`.
- `install.sh`: Script instalasi otomatis.

## Cara Instalasi
1. Clone repository ini ke server.
2. Jalankan `./install.sh` (membutuhkan akses sudo).
3. Pastikan baris berikut ada di `/etc/pam.d/sshd`:
   `session optional pam_exec.so /etc/ssh-firewall/ssh-firewall`

## Penggunaan Monitoring CLI
Gunakan perintah global `whitelist` untuk memantau server:
- `whitelist` : Melihat bantuan penggunaan.
- `whitelist --live` : Memantau user aktif secara real-time.
- `whitelist -port` : Melihat status aturan firewall saat ini.
- `whitelist -logs` : Melihat riwayat log login/logout terakhir.
- `whitelist -logs -port --live` : Dashboard penuh.

## Lokasi Operasional
- Binary Utama: `/etc/ssh-firewall/ssh-firewall`
- Log Aktivitas: `/etc/ssh-firewall/firewall.log`
- Data IP Aktif: `/etc/ssh-firewall/active_ips.txt`

---
```
ssh-firewall/
├── src/
│   ├── main.c        # entry point, baca env PAM
│   ├── firewall.c    # eksekusi UFW via execvp()
│   ├── firewall.h
│   ├── ipstore.c     # baca/tulis active_ips.txt + locking
│   ├── ipstore.h
│   ├── logger.c      # tulis firewall.log + syslog
│   ├── logger.h
│   ├── netutil.c     # parse & validasi IP (IPv4/IPv6)
│   └── netutil.h
├── Makefile
└── install.sh
```
