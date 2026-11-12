#include <stdio.h>
#include <string.h>
#include "network/tcp_protocol.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage : process.exe P1  ou  process.exe P2\n");
        return 1;
    }
    if (!network_init()) return 1;

    if (strcmp(argv[1], "P2") == 0) {
        /* P2 = serveur : il attend l'appel de P1 */
        SOCKET server = create_server(5002);
        if (server == INVALID_SOCKET) return 1;

        printf("P2 attend un appel...\n");
        SOCKET client = accept_client(server);

        char buffer[256];
        int n = recv(client, buffer, sizeof(buffer) - 1, 0);
        if (n > 0) {
            buffer[n] = '\0';
            printf("P2 a recu : %s\n", buffer);
        }
        closesocket(client);
        closesocket(server);
    }
    else if (strcmp(argv[1], "P1") == 0) {
        /* P1 = client : il appelle P2 */
        SOCKET s = connect_to_process(5002, 50, 200);
        if (s == INVALID_SOCKET) return 1;

        const char* msg = "Bonjour P2";
        send(s, msg, (int)strlen(msg), 0);
        printf("P1 a envoye : %s\n", msg);
        closesocket(s);
    }

    network_cleanup();
    return 0;
}