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

/* ---------- envoi ---------- */

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

/* ---------- reception ---------- */

void lecteur_init(Lecteur* l, SOCKET s) { l->sock = s; l->len = 0; }

int lecteur_remplir(Lecteur* l) {
    if (l->len >= (int)sizeof(l->buf)) { l->len = 0; return -1; }   /* ligne trop longue */
    int r = recv(l->sock, l->buf + l->len, (int)sizeof(l->buf) - l->len, 0);
    if (r > 0) l->len += r;
    return r;
}

int lecteur_extraire_ligne(Lecteur* l, char* out, int taille) {
    char* nl = (char*)memchr(l->buf, '\n', l->len);
    if (!nl) return 0;
    int longueur = (int)(nl - l->buf);
    int copie = longueur < taille - 1 ? longueur : taille - 1;
    memcpy(out, l->buf, copie);
    if (copie > 0 && out[copie - 1] == '\r') copie--;
    out[copie] = '\0';
    int consomme = longueur + 1;
    memmove(l->buf, l->buf + consomme, l->len - consomme);
    l->len -= consomme;
    return 1;
}

/* "exp=1;id=1001;lamport=2;vecteur=2,0,0" -> Message */
static int decoder_ligne(const char* ligne, int n, Message* m) {
    int lu = 0;
    memset(m, 0, sizeof(*m));
    if (sscanf(ligne, "exp=%d;id=%d;lamport=%d;vecteur=%n",
               &m->expediteur, &m->id, &m->lamport, &lu) < 3 || lu == 0)
        return 0;
    const char* p = ligne + lu;
    for (int k = 0; k < n; k++) {
        char* fin;
        m->vecteur[k] = (int)strtol(p, &fin, 10);
        if (fin == p) return 0;
        p = (*fin == ',') ? fin + 1 : fin;
    }
    return m->expediteur >= 1 && m->expediteur <= n;
}

int lecteur_extraire(Lecteur* l, int n, Message* m) {
    char ligne[TAILLE_LIGNE];
    while (lecteur_extraire_ligne(l, ligne, sizeof(ligne))) {
        if (decoder_ligne(ligne, n, m)) return 1;
        printf("Ligne mal formee ignoree : %s\n", ligne);
    }
    return 0;
}

int recv_message(Lecteur* l, int n, Message* m) {
    for (;;) {
        if (lecteur_extraire(l, n, m)) return 1;
        int r = lecteur_remplir(l);
        if (r <= 0) return (r == 0) ? 0 : -1;
    }
}

int attendre_lisible(SOCKET s, int ms) {
    fd_set set;
    struct timeval tv;
    FD_ZERO(&set);
    FD_SET(s, &set);
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    int r = select((int)s + 1, &set, NULL, NULL, &tv);
    return r < 0 ? -1 : (r == 0 ? 0 : 1);
}