#include <stdio.h>
#include <string.h>
#include "tcp_protocol.h"

SOCKET create_server(int port) {
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) {
        printf("Erreur socket : %d\n", WSAGetLastError());
        return INVALID_SOCKET;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((u_short)port);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        printf("Erreur bind (port %d deja utilise ?) : %d\n", port, WSAGetLastError());
        closesocket(s);
        return INVALID_SOCKET;
    }
    if (listen(s, SOMAXCONN) == SOCKET_ERROR) {
        printf("Erreur listen : %d\n", WSAGetLastError());
        closesocket(s);
        return INVALID_SOCKET;
    }
    printf("Serveur en ecoute sur le port %d\n", port);
    return s;
}

SOCKET accept_client(SOCKET server_socket) {
    return accept(server_socket, NULL, NULL);
}