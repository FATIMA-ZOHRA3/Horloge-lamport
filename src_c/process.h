#ifndef PROCESS_H
#define PROCESS_H

#include "network/tcp_protocol.h"

/* Un processus et ses connexions vers les n-1 autres */
typedef struct {
    int     moi;                     /* mon numero (1..n) */
    int     n;                       /* nombre de processus (2..5) */
    SOCKET  ecoute;                  /* ma socket d'ecoute (port 5000 + moi) */
    SOCKET  pair[MAX_PROC + 1];      /* pair[k] = connexion vers le processus k */
    Lecteur lecteur[MAX_PROC + 1];   /* un lecteur de lignes par connexion */
    int     actif[MAX_PROC + 1];     /* 1 si la connexion vers k est ouverte */
} Processus;

#define RECV_OK       1
#define RECV_TIMEOUT  0
#define RECV_ERREUR (-1)

/* Ecoute, se connecte a tous les autres. Retourne 1 si le maillage est complet.
 * (network_init() doit avoir ete appele avant) */
int  process_init(Processus* p, int moi, int n);

/* Envoie un message au processus dest. 1 = ok */
int  process_envoyer(Processus* p, int dest, const Message* m);

/* Attend UN message de n'importe quel pair (au plus timeout_ms ms) */
int  process_recevoir(Processus* p, Message* m, int timeout_ms);

void process_fermer(Processus* p);

#endif