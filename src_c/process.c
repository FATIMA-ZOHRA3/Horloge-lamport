#include <stdio.h>
#include <string.h>
#include "process.h"

#ifdef _WIN32
  #define SHUTDOWN_ENVOI SD_SEND
#else
  #define SHUTDOWN_ENVOI SHUT_WR
#endif

static void installer_pair(Processus* p, int k, SOCKET s, const Lecteur* deja_lu) {
    p->pair[k] = s;
    if (deja_lu) p->lecteur[k] = *deja_lu;      /* garde les octets deja recus */
    else lecteur_init(&p->lecteur[k], s);
    p->lecteur[k].sock = s;
    p->actif[k] = 1;
}

int process_init(Processus* p, int moi, int n) {
    memset(p, 0, sizeof(*p));
    p->moi = moi;
    p->n = n;
    p->ecoute = INVALID_SOCKET;
    for (int k = 0; k <= MAX_PROC; k++) p->pair[k] = INVALID_SOCKET;

    if (n < 2 || n > MAX_PROC || moi < 1 || moi > n) {
        printf("Parametres invalides (moi=%d, n=%d)\n", moi, n);
        return 0;
    }

    /* 1. J'ecoute d'abord : les autres pourront se connecter a moi */
    p->ecoute = create_server(BASE_PORT + moi);
    if (p->ecoute == INVALID_SOCKET) return 0;

    /* 2. Je me connecte aux processus de numero SUPERIEUR (plus petit -> plus grand) */
    for (int k = moi + 1; k <= n; k++) {
        SOCKET s = connect_to_process(BASE_PORT + k, 150, 200);   /* ~30 s max */
        if (s == INVALID_SOCKET) return 0;
        char hello[32];
        snprintf(hello, sizeof(hello), "HELLO %d\n", moi);        /* je m'identifie */
        if (!send_all(s, hello, (int)strlen(hello))) { closesocket(s); return 0; }
        installer_pair(p, k, s, NULL);
    }

    /* 3. J'accepte les connexions des processus de numero INFERIEUR */
    for (int attendus = moi - 1; attendus > 0; ) {
        if (attendre_lisible(p->ecoute, 30000) <= 0) {
            printf("[P%d] Timeout : il manque %d connexion(s) entrante(s)\n", moi, attendus);
            return 0;
        }
        SOCKET s = accept_client(p->ecoute);
        if (s == INVALID_SOCKET) continue;

        /* lire la ligne "HELLO k" pour savoir qui vient de se connecter */
        Lecteur tmp;
        char ligne[TAILLE_LIGNE];
        int k = 0, ok = 0;
        lecteur_init(&tmp, s);
        while (attendre_lisible(s, 5000) == 1) {
            if (lecteur_extraire_ligne(&tmp, ligne, sizeof(ligne))) { ok = 1; break; }
            if (lecteur_remplir(&tmp) <= 0) break;
            if (lecteur_extraire_ligne(&tmp, ligne, sizeof(ligne))) { ok = 1; break; }
        }
        if (!ok || sscanf(ligne, "HELLO %d", &k) != 1 || k < 1 || k >= moi || p->actif[k]) {
            printf("[P%d] Connexion entrante invalide, fermee\n", moi);
            closesocket(s);
            continue;
        }
        installer_pair(p, k, s, &tmp);
        attendus--;
    }
    return 1;
}

int process_envoyer(Processus* p, int dest, const Message* m) {
    if (dest < 1 || dest > p->n || dest == p->moi || !p->actif[dest]) {
        printf("[P%d] Envoi impossible vers P%d\n", p->moi, dest);
        return 0;
    }
    return send_message(p->pair[dest], p->n, m);
}

int process_recevoir(Processus* p, Message* m, int timeout_ms) {
    /* GetTickCount64 n'existe pas partout : on decompte le temps par tranches */
    int reste = timeout_ms;
    for (;;) {
        /* a) un message complet est-il deja dans un tampon ? */
        for (int k = 1; k <= p->n; k++)
            if (p->actif[k] && lecteur_extraire(&p->lecteur[k], p->n, m)) return RECV_OK;

        if (reste <= 0) return RECV_TIMEOUT;

        /* b) sinon on attend des donnees sur TOUTES les connexions (select) */
        fd_set set;
        int maxfd = 0, nb = 0;
        FD_ZERO(&set);
        for (int k = 1; k <= p->n; k++) {
            if (!p->actif[k]) continue;
            FD_SET(p->pair[k], &set);
            if ((int)p->pair[k] > maxfd) maxfd = (int)p->pair[k];
            nb++;
        }
        if (nb == 0) return RECV_ERREUR;           /* plus aucun pair connecte */

        int tranche = reste < 200 ? reste : 200;
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = tranche * 1000;
        int s = select(maxfd + 1, &set, NULL, NULL, &tv);
        if (s < 0) { printf("Erreur select : %d\n", WSAGetLastError()); return RECV_ERREUR; }
        if (s == 0) { reste -= tranche; continue; }

        for (int k = 1; k <= p->n; k++) {
            if (!p->actif[k] || !FD_ISSET(p->pair[k], &set)) continue;
            if (lecteur_remplir(&p->lecteur[k]) <= 0) {   /* pair ferme */
                printf("[P%d] P%d s'est deconnecte\n", p->moi, k);
                closesocket(p->pair[k]);
                p->pair[k] = INVALID_SOCKET;
                p->actif[k] = 0;
            }
        }
    }
}

void process_fermer(Processus* p) {
    for (int k = 1; k <= p->n; k++)
        if (p->actif[k]) shutdown(p->pair[k], SHUTDOWN_ENVOI);
    Sleep(300);          /* laisse partir les derniers octets */
    for (int k = 1; k <= p->n; k++) {
        if (p->actif[k]) closesocket(p->pair[k]);
        p->actif[k] = 0;
    }
    if (p->ecoute != INVALID_SOCKET) closesocket(p->ecoute);
    p->ecoute = INVALID_SOCKET;
}