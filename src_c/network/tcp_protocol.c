#include <stdio.h>
#include "tcp_protocol.h"

int network_init(void) {
    WSADATA wsa;
    int r = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (r != 0) {
        printf("Erreur WSAStartup : %d\n", r);
        return 0;
    }
    return 1;
}

void network_cleanup(void) {
    WSACleanup();
}