#ifndef JOURNAL_H
#define JOURNAL_H

/* Journal CSV : journaux/log_<pid>.csv
 * Format : pid,type,temps_ms,lamport,vecteur,msg_id,pair  (docs/02_contrats_formats.md) */

int  journal_ouvrir(int pid);     /* cree le dossier journaux/ et le fichier + en-tete. 1 = ok */

/* type : "LOCAL", "ENVOI" ou "RECEPTION".
 * msg_id = 0 et pair = 0 pour LOCAL : les colonnes restent vides. */
void journal_ecrire(int pid, const char* type, int lamport,
                    const int* vecteur, int n, int msg_id, int pair);

void journal_fermer(void);

#endif