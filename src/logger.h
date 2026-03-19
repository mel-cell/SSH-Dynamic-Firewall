#ifndef LOGGER_H
#define LOGGER_H
#define LOGGER_H
/*
 * log_event:
 * Mencatat kejadian (Login/Logout) ke 2 tempat:
 * 1. File firewall.log (format yang lama)
 * 2. System Journal (syslog) - agar muncul di journalctl
 * 
 * Parameter:
 *   action - misal: "OPEN" atau "CLOSE"
 *   user   - username (pam_user)
 *   ip     - ip address user
 *   status - pesan tambahan (misal: "rule ADDED" atau "rule REMOVED")
 */

 void log_event(const char *action, const char *user, const char *ip, const char *status);

 #endif