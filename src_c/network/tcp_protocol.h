#ifndef TCP_PROTOCOL_H
#define TCP_PROTOCOL_H

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#include <winsock2.h>
#include <ws2tcpip.h>

#define BASE_PORT 5000   /* P1=5001, P2=5002, P3=5003 */

int    network_init(void);
void   network_cleanup(void);
SOCKET create_server(int port);
SOCKET accept_client(SOCKET server_socket);
SOCKET connect_to_process(int port, int max_retries, int delay_ms);

#endif