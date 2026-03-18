![image](image.png)

---

ssh-firewall/
├── src/
│   ├── main.c        ← entry point, baca env PAM
│   ├── firewall.c    ← eksekusi UFW via execvp()
│   ├── firewall.h
│   ├── ipstore.c     ← baca/tulis active_ips.txt + locking
│   ├── ipstore.h
│   ├── logger.c      ← tulis firewall.log + syslog
│   ├── logger.h
│   ├── netutil.c     ← parse & validasi IP (IPv4/IPv6)
│   └── netutil.h
├── Makefile
└── install.sh