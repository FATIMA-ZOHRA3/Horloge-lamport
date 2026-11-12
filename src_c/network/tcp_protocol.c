#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tcp_protocol.h"

int network_init(void) {
    WSADATA wsa;
    int r = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (r != 0) { printf("Erreur WSAStartup : %d\n", r); return 0; }
    return 1;
}

void network_cleanup(void) { WSACleanup(); }

/* send() peut n'envoyer qu'une partie : on boucle jusqu'au bout */
int send_all(SOCKET s, const char* data, int len) {
    int total = 0;
    while (total < len) {
        int n = send(s, data + total, len - total, 0);
        if (n == SOCKET_ERROR || n <= 0) {
            printf("Erreur send : %d\n", WSAGetLastError());
            return 0;
        }
        total += n;
    }
    return 1;
}

/* Message -> "exp=1;id=1001;lamport=2;vecteur=2,0,0\n" puis envoi */
int send_message(SOCKET s, int n, const Message* m) {
    char ligne[TAILLE_LIGNE];
    int pos = snprintf(ligne, sizeof(ligne), "exp=%d;id=%d;lamport=%d;vecteur=",
                       m->expediteur, m->id, m->lamport);
    for (int k = 0; k < n; k++)
        pos += snprintf(ligne + pos, sizeof(ligne) - pos, k ? ",%d" : "%d", m->vecteur[k]);
    pos += snprintf(ligne + pos, sizeof(ligne) - pos, "\n");
    return send_all(s, ligne, pos);
}

void lecteur_init(Lecteur* l, SOCKET s) { l->sock = s; l->len = 0; }

/* Lit une ligne complete, la decode dans *m. 1=ok, 0=connexion fermee, -1=erreur */
int recv_message(Lecteur* l, int n, Message* m) {
    for (;;) {
        char* nl = (char*)memchr(l->buf, '\n', l->len);
        if (nl) {
            *nl = '\0';
            int lu = 0;
            memset(m, 0, sizeof(*m));
            int ok = sscanf(l->buf, "exp=%d;id=%d;lamport=%d;vecteur=%n",
                            &m->expediteur, &m->id, &m->lamport, &lu) >= 3 && lu > 0;
            if (ok) {
                const char* p = l->buf + lu;
                for (int k = 0; k < n && ok; k++) {
                    char* fin;
                    m->vecteur[k] = (int)strtol(p, &fin, 10);
                    if (fin == p) ok = 0;
                    p = (*fin == ',') ? fin + 1 : fin;
                }
            }
            int consomme = (int)(nl - l->buf) + 1;
            memmove(l->buf, l->buf + consomme, l->len - consomme);
            l->len -= consomme;
            if (!ok) { printf("Ligne mal formee ignoree\n"); continue; }
            return 1;
        }
        if (l->len >= (int)sizeof(l->buf)) return -1;      /* ligne trop longue */
        int r = recv(l->sock, l->buf + l->len, (int)sizeof(l->buf) - l->len, 0);
        if (r > 0) { l->len += r; continue; }
        return (r == 0) ? 0 : -1;
    }
}