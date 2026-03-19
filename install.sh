#!/bin/bash

# 1. Buat folder dan file pendukung
sudo mkdir -p /etc/ssh-firewall
sudo touch /etc/ssh-firewall/active_ips.txt
sudo touch /etc/ssh-firewall/firewall.log

# 2. Atur Kepemilikan (Ganti senvada jika usernamemu beda)
sudo chown -R senvada:senvada /etc/ssh-firewall
sudo chmod 664 /etc/ssh-firewall/active_ips.txt
sudo chmod 664 /etc/ssh-firewall/firewall.log

# 3. Compile dan Pindahkan Binary
make clean
make
sudo cp ssh-firewall /etc/ssh-firewall/ssh-firewall
sudo chown root:root /etc/ssh-firewall/ssh-firewall
sudo chmod +x /etc/ssh-firewall/ssh-firewall

# 4. Tambahkan Aturan Sudoers Khusus UFW
# (Ini penting agar binary bisa sudo ufw tanpa password)
SUDOERS_FILE="/etc/sudoers.d/ssh-firewall"
if [ ! -f "$SUDOERS_FILE" ]; then
    echo "senvada ALL=(ALL) NOPASSWD: /usr/sbin/ufw" | sudo tee "$SUDOERS_FILE"
fi

echo "--- INSTALL Selesai ---"
echo "Sekarang tambahkan baris ini ke /etc/pam.d/sshd:"
echo "session optional pam_exec.so /etc/ssh-firewall/ssh-firewall"
