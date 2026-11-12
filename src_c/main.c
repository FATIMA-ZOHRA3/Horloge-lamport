#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"

/* Test etape 3 : chacun envoie un message a tous les autres,
 * puis attend n-1 messages.
 * Usage : process.exe <numero> <n>      ex : process.exe 2 3 */
int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage : %s <numero> <n>\n", argv[0]);
        return 1;
    }
    int moi = atoi(argv[1]);
    int n   = atoi(argv[2]);

    if (!network_init()) return 1;

    Processus p;
    if (!process_init(&p, moi, n)) {
        printf("[P%d] Echec de l'initialisation\n", moi);
        process_fermer(&p);
        network_cleanup();
        return 1;
    }
    printf("[P%d] Connecte aux %d autres processus\n", moi, n - 1);

    /* envoyer un message a chaque autre processus */
    for (int k = 1; k <= n; k++) {
        if (k == moi) continue;
        Message m;
        memset(&m, 0, sizeof(m));
        m.expediteur = moi;
        m.id = moi * 1000 + k;
        printf("[P%d] envoie id=%d a P%d\n", moi, m.id, k);
        process_envoyer(&p, k, &m);
    }

    /* recevoir n-1 messages (de n'importe qui) */
    for (int i = 0; i < n - 1; i++) {
        Message m;
        int r = process_recevoir(&p, &m, 10000);
        if (r == RECV_OK)
            printf("[P%d] RECOIT id=%d de P%d\n", moi, m.id, m.expediteur);
        else if (r == RECV_TIMEOUT)
            printf("[P%d] TIMEOUT : rien recu en 10 s\n", moi);
        else
            printf("[P%d] ERREUR de reception\n", moi);
    }

    process_fermer(&p);
    network_cleanup();
    return 0;
}