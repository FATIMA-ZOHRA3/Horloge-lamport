#include <string.h>
#include "horloges.h"

void horloge_init(Horloge* h, int moi, int n) {
    memset(h, 0, sizeof(*h));
    h->moi = moi;
    h->n = n;
}

/* Local : C = C + 1 ; V[moi] = V[moi] + 1 */
void horloge_local(Horloge* h) {
    h->lamport++;
    h->vecteur[h->moi - 1]++;
}

/* Envoi : memes incrementations. Le message portera ensuite C et V. */
void horloge_envoi(Horloge* h) {
    horloge_local(h);
}

/* Reception : C = max(C, C_msg) + 1 ; V[k] = max(V[k], W[k]) ; V[moi] += 1 */
void horloge_reception(Horloge* h, int lamport_msg, const int* vecteur_msg) {
    h->lamport = (h->lamport > lamport_msg ? h->lamport : lamport_msg) + 1;
    for (int k = 0; k < h->n; k++)
        if (vecteur_msg[k] > h->vecteur[k]) h->vecteur[k] = vecteur_msg[k];
    h->vecteur[h->moi - 1]++;
}

/* A precede B : toutes les cases de A <= celles de B, et A != B */
int comparer_vecteurs(const int* a, const int* b, int n) {
    int a_plus_petit = 0, a_plus_grand = 0;
    for (int k = 0; k < n; k++) {
        if (a[k] < b[k]) a_plus_petit = 1;
        else if (a[k] > b[k]) a_plus_grand = 1;
    }
    if (!a_plus_petit && !a_plus_grand) return VEC_EGAUX;
    if (a_plus_petit && !a_plus_grand)  return VEC_AVANT;
    if (a_plus_grand && !a_plus_petit)  return VEC_APRES;
    return VEC_CONCURRENTS;
}