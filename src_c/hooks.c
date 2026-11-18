#include <stdio.h>
#include "hooks.h"
#include "horloges.h"
#include "journal.h"

/* Branche les horloges et le journal (Membre 2) sur le reseau (Membre 1). */

static Horloge horloge;

static void vecteur_texte(const int* v, int n, char* out, int taille) {
    int pos = 0;
    for (int k = 0; k < n && pos < taille; k++)
        pos += snprintf(out + pos, taille - pos, k ? ",%d" : "%d", v[k]);
}

void hook_debut(int moi, int n) {
    horloge_init(&horloge, moi, n);
    journal_ouvrir(moi);
}

void hook_local(int moi) {
    char v[64];
    horloge_local(&horloge);
    journal_ecrire(moi, "LOCAL", horloge.lamport, horloge.vecteur, horloge.n, 0, 0);
    vecteur_texte(horloge.vecteur, horloge.n, v, sizeof(v));
    printf("[P%d] LOCAL       L=%d V=[%s]\n", moi, horloge.lamport, v);
}

/* Appele AVANT l'envoi sur le socket : la ligne ENVOI est donc deja ecrite */
void hook_envoi(int moi, int dest, Message* m) {
    char v[64];
    horloge_envoi(&horloge);
    m->lamport = horloge.lamport;
    for (int k = 0; k < horloge.n; k++) m->vecteur[k] = horloge.vecteur[k];
    journal_ecrire(moi, "ENVOI", horloge.lamport, horloge.vecteur, horloge.n, m->id, dest);
    vecteur_texte(horloge.vecteur, horloge.n, v, sizeof(v));
    printf("[P%d] ENVOI       id=%d vers P%d   L=%d V=[%s]\n", moi, m->id, dest, horloge.lamport, v);
}

void hook_reception(int moi, const Message* m) {
    char v[64];
    horloge_reception(&horloge, m->lamport, m->vecteur);
    journal_ecrire(moi, "RECEPTION", horloge.lamport, horloge.vecteur, horloge.n, m->id, m->expediteur);
    vecteur_texte(horloge.vecteur, horloge.n, v, sizeof(v));
    printf("[P%d] RECEPTION   id=%d de P%d   L=%d V=[%s]\n", moi, m->id, m->expediteur, horloge.lamport, v);
}

void hook_fin(int moi) {
    (void)moi;
    journal_fermer();
}