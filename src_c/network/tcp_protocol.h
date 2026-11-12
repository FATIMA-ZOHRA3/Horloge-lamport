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

int    network_init(void);
void   network_cleanup(void);
SOCKET create_server(int port);
SOCKET accept_client(SOCKET server_socket);
SOCKET connect_to_process(int port, int max_retries, int delay_ms);

int  send_all(SOCKET s, const char* data, int len);
int  send_message(SOCKET s, int n, const Message* m);
void lecteur_init(Lecteur* l, SOCKET s);
int  recv_message(Lecteur* l, int n, Message* m);   /* 1=ok, 0=ferme, -1=erreur */

#endif