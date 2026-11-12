#include <stdio.h>
#include <string.h>
#include "network/tcp_protocol.h"

#define N 3

int main(int argc, char* argv[]) {
    if (argc != 2) { printf("Usage : process.exe P1|P2\n"); return 1; }
    if (!network_init()) return 1;

    if (strcmp(argv[1], "P2") == 0) {
        SOCKET server = create_server(BASE_PORT + 2);
        if (server == INVALID_SOCKET) return 1;
        SOCKET client = accept_client(server);

        Lecteur lec;
        lecteur_init(&lec, client);
        Message m;
        while (recv_message(&lec, N, &m) == 1)
            printf("P2 recoit : exp=%d id=%d lamport=%d vecteur=%d,%d,%d\n",
                   m.expediteur, m.id, m.lamport, m.vecteur[0], m.vecteur[1], m.vecteur[2]);
        printf("P1 a ferme la connexion.\n");
        closesocket(client);
        closesocket(server);
    }
    else if (strcmp(argv[1], "P1") == 0) {
        SOCKET s = connect_to_process(BASE_PORT + 2, 50, 200);
        if (s == INVALID_SOCKET) return 1;
        for (int i = 1; i <= 3; i++) {
            Message m = {0};
            m.expediteur = 1;
            m.id = 1000 + i;
            m.lamport = i;
            m.vecteur[0] = i;
            send_message(s, N, &m);
            printf("P1 envoie id=%d\n", m.id);
        }
        closesocket(s);
    }
    network_cleanup();
    return 0;
}