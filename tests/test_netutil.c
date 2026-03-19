#include <stdio.h>
#include <string.h>
#include "/home/senvada/Projects/port-knocking/src/netutil.h"

int main() {
    char ip_result[64];
    int score = 0;
    
    printf("--- RUNNING NETUTIL TESTS ---\n\n");

    // Test 1: IPv4 Standard
    const char *t1 = "114.12.34.56 54321 22";
    if (extract_and_validation_ip(t1, ip_result, sizeof(ip_result)) == 0 && strcmp(ip_result, "114.12.34.56") == 0) {
        printf("[OK] Test 1: IPv4 extracted correctly: %s\n", ip_result);
        score++;
    } else {
        printf("[FAIL] Test 1: IPv4 extraction failed\n");
    }

    // Test 2: IPv6 Standard
    const char *t2 = "2001:4860:4860::8888 1234 22";
    if (extract_and_validation_ip(t2, ip_result, sizeof(ip_result)) == 0 && strcmp(ip_result, "2001:4860:4860::8888") == 0) {
        printf("[OK] Test 2: IPv6 extracted correctly: %s\n", ip_result);
        score++;
    } else {
        printf("[FAIL] Test 2: IPv6 extraction failed\n");
    }

    // Test 3: Invalid Hostname (Security check)
    const char *t3 = "malicious.host 1234 22";
    if (extract_and_validation_ip(t3, ip_result, sizeof(ip_result)) == -1) {
        printf("[OK] Test 3: Invalid hostname correctly rejected\n");
        score++;
    } else {
        printf("[FAIL] Test 3: Invalid hostname was accepted (DANGEROUS!)\n");
    }

    printf("\n--- FINAL SCORE: %d/3 ---\n", score);
    return (score == 3) ? 0 : 1;
}
