#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "scenario.h"
#include "hooks.h"

#define TIMEOUT_RECEPTION_MS 30000   /* une reception ne bloque jamais indefiniment */

int scenario_charger(Scenario* sc, const char* chemin, int moi, int n) {
    FILE* f = fopen(chemin, "r");
    char ligne[256];
    int numero = 0;

    sc->nb = 0;
    if (!f) {
        printf("Impossible d'ouvrir le scenario : %s\n", chemin);
        return 0;
    }
    while (fgets(ligne, sizeof(ligne), f)) {
        numero++;
        char* s = ligne;
        while (isspace((unsigned char)*s)) s++;
        if (*s == '\0' || *s == '#') continue;          /* vide ou commentaire */

        int pid = 0, dest = 0;
        char act = 0;
        int lus = sscanf(s, "%d %c %d", &pid, &act, &dest);
        if (lus < 2 || pid < 1 || pid > n ||
            (act != ACT_LOCAL && act != ACT_ENVOI && act != ACT_RECEP) ||
            (act == ACT_ENVOI && (lus < 3 || dest < 1 || dest > n || dest == pid))) {
            printf("%s, ligne %d : action invalide : %s", chemin, numero, ligne);
            fclose(f);
            return 0;
        }
        if (pid != moi) continue;                        /* action d'un autre processus */
        if (sc->nb >= MAX_ACTIONS) {
            printf("Scenario trop long (max %d actions)\n", MAX_ACTIONS);
            fclose(f);
            return 0;
        }
        sc->actions[sc->nb].type = act;
        sc->actions[sc->nb].dest = (act == ACT_ENVOI) ? dest : 0;
        sc->nb++;
    }
    fclose(f);
    return 1;
}

int scenario_executer(const Scenario* sc, Processus* p) {
    int moi = p->moi;
    int compteur = 0;                                    /* P1 : 1001, 1002, ... */

    hook_debut(moi, p->n);
    for (int i = 0; i < sc->nb; i++) {
        const Action* a = &sc->actions[i];

        if (a->type == ACT_LOCAL) {
            hook_local(moi);

        } else if (a->type == ACT_ENVOI) {
            Message m;
            memset(&m, 0, sizeof(m));
            m.expediteur = moi;
            m.id = moi * 1000 + (++compteur);
            hook_envoi(moi, a->dest, &m);                /* journal AVANT l'envoi */
            if (!process_envoyer(p, a->dest, &m)) {
                printf("[P%d] Echec de l'envoi a P%d\n", moi, a->dest);
                hook_fin(moi);
                return 0;
            }

        } else {                                          /* ACT_RECEP */
            Message m;
            int r = process_recevoir(p, &m, TIMEOUT_RECEPTION_MS);
            if (r == RECV_OK) {
                hook_reception(moi, &m);
            } else {
                printf("[P%d] %s a l'action %d (R)\n", moi,
                       r == RECV_TIMEOUT ? "Timeout" : "Erreur de reception", i + 1);
                hook_fin(moi);
                return 0;
            }
        }
    }
    hook_fin(moi);
    return 1;
}