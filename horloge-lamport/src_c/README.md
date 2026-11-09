# src_c : les processus en C

Un seul programme (proc), lancé en 3 à 5 exemplaires. Chaque exemplaire :
1. reçoit en paramètre son numéro, le nombre de processus et son fichier de scénario ;
2. écoute sur le port 5000 + son numéro ;
3. exécute ses actions (L, S k, R) dans l'ordre ;
4. écrit son journal dans journaux/log_<numéro>.csv (format : docs/02_contrats_formats.md).

| Fichier | Rôle | Responsable |
|---|---|---|
| proc.c | Programme principal, lecture du scénario | [Nom 1] |
| reseau.h / reseau.c | Sockets TCP : écouter, envoyer, recevoir | [Nom 1] |
| horloges.h / horloges.c | Lamport et vecteur | [Nom 2] |
| journal.h / journal.c | Écriture du journal CSV | [Nom 2] |
| Makefile | Compilation | [Nom 1] |

Chaque fichier contient un en-tête TODO qui décrit ce qu'il faut écrire.
