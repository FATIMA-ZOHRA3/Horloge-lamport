#ifndef TCP_PROTOCOL_H
#define TCP_PROTOCOL_H

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#include <winsock2.h>
#include <ws2tcpip.h>

#define BASE_PORT    5000      /* processus i -> port 5000 + i */
#define MAX_PROC     5
#define TAILLE_LIGNE 512

/* Message du contrat (docs/02_contrats_formats.md) */
typedef struct {
    int expediteur;
    int id;
    int lamport;
    int vecteur[MAX_PROC];
} Message;

/* Lecteur de lignes : gere les frontieres des messages TCP */
typedef struct {
    SOCKET sock;
    char   buf[TAILLE_LIGNE * 2];
    int    len;
} Lecteur;

/* Winsock */
int    network_init(void);
void   network_cleanup(void);

/* Serveur / client */
SOCKET create_server(int port);
SOCKET accept_client(SOCKET server_socket);
SOCKET connect_to_process(int port, int max_retries, int delay_ms);

/* Envoi */
int  send_all(SOCKET s, const char* data, int len);
int  send_message(SOCKET s, int n, const Message* m);

/* Reception */
void lecteur_init(Lecteur* l, SOCKET s);
int  lecteur_remplir(Lecteur* l);                              /* un recv() : >0 octets, 0 ferme, <0 erreur */
int  lecteur_extraire_ligne(Lecteur* l, char* out, int taille);/* 1 si une ligne complete est dans le tampon */
int  lecteur_extraire(Lecteur* l, int n, Message* m);          /* 1 si un message complet est dans le tampon */
int  recv_message(Lecteur* l, int n, Message* m);              /* bloquant : 1=ok, 0=ferme, -1=erreur */
int  attendre_lisible(SOCKET s, int ms);                       /* 1=lisible, 0=timeout, -1=erreur */

#endif