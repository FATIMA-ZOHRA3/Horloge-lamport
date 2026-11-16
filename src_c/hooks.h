#ifndef HOOKS_H
#define HOOKS_H

#include "network/tcp_protocol.h"

/* Points d'accroche entre le reseau (Membre 1) et les horloges/journal (Membre 2).
 * Le reseau appelle ces fonctions ; le Membre 2 les remplit dans hooks.c
 * avec horloges.c et journal.c. Le Membre 1 n'a plus a y toucher.
 *
 * moi / n : numero du processus et nombre de processus. */

/* Une fois au debut, avant la premiere action */
void hook_debut(int moi, int n);

/* Evenement local (action L) */
void hook_local(int moi);

/* Envoi (action S k). Appele AVANT l'envoi sur le socket.
 * m->expediteur et m->id sont deja remplis ; le Membre 2 doit remplir
 * m->lamport et m->vecteur (valeurs APRES le +1), et ecrire la ligne ENVOI. */
void hook_envoi(int moi, int dest, Message* m);

/* Reception (action R), apres recv. Le Membre 2 met a jour les horloges
 * avec m->lamport et m->vecteur, puis ecrit la ligne RECEPTION. */
void hook_reception(int moi, const Message* m);

/* Une fois a la fin (fermer le journal) */
void hook_fin(int moi);

#endif