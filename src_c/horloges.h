#ifndef HORLOGES_H
#define HORLOGES_H

#include "network/tcp_protocol.h"   /* pour MAX_PROC */

/* Horloges d'UN processus : Lamport (scalaire) + vecteur.
 * vecteur[k-1] = ce que ce processus sait du processus k. */
typedef struct {
    int moi;                   /* mon numero (1..n) */
    int n;                     /* nombre de processus */
    int lamport;
    int vecteur[MAX_PROC];
} Horloge;

/* Resultat de comparer_vecteurs(a, b, n) */
#define VEC_EGAUX        0
#define VEC_AVANT        1     /* a precede b */
#define VEC_APRES        2     /* b precede a */
#define VEC_CONCURRENTS  3

void horloge_init(Horloge* h, int moi, int n);               /* tout a 0 */
void horloge_local(Horloge* h);                              /* evenement local */
void horloge_envoi(Horloge* h);                              /* a appeler AVANT d'envoyer */
void horloge_reception(Horloge* h, int lamport_msg, const int* vecteur_msg);
int  comparer_vecteurs(const int* a, const int* b, int n);

#endif