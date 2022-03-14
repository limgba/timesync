#pragma once
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

DWORD WINAPI tcp_recv_buf(LPVOID lpThreadParameter);
void tcp_server();
void udp_server();
