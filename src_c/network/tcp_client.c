#include <stdio.h>
#include <string.h>
#include "tcp_protocol.h"

/* Se connecte a 127.0.0.1:port. Reessaie si l'autre n'est pas encore lance. */
SOCKET connect_to_process(int port, int max_retries, int delay_ms) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((u_short)port);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    int i;
    for (i = 0; i < max_retries; i++) {
        SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s == INVALID_SOCKET) return INVALID_SOCKET;
        if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
            printf("Connexion reussie au port %d\n", port);
            return s;
        }
        closesocket(s);
        Sleep(delay_ms);
    }
    printf("Echec : impossible de se connecter au port %d\n", port);
    return INVALID_SOCKET;
}