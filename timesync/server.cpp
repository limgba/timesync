#include "server.h"
#include <stdio.h>
#include "protocol.h"
#include "config.h"

DWORD WINAPI tcp_recv_buf(LPVOID lpThreadParameter)
{
	SOCKET sock = (SOCKET)lpThreadParameter;
	char buf[MAX_BUF_LEN];
	int ret = 0;
	while (true)
	{
		memset(buf, 0, sizeof(buf));
		ret = recv(sock, buf, sizeof(buf), 0);
		if (ret == 0)
		{
			printf("disconnect sock[%d]\n", sock);
			break;
		}
		if (ret == SOCKET_ERROR)
		{
			printf("socket error sock[%d] error[%d]\n", sock, GetLastError());
			break;
		}
		ProcessMessage(sock, buf, ret);
	}
	return 0;
}

void tcp_server()
{
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata))
	{
		printf("startup error %d\n", GetLastError());
		return;
	}
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in server_addr, client_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);
	memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

	int ret = bind(sock, (sockaddr*)&server_addr, sizeof(server_addr));
	if (ret)
	{
		printf("bind error %d\n", GetLastError());
		return;
	}
	ret = listen(sock, 1);
	if (ret)
	{
		printf("listen error %d\n", GetLastError());
		return;
	}

	while (true)
	{
		int addr_len = sizeof(client_addr);
		SOCKET client_sock = accept(sock, (sockaddr*)&client_addr, &addr_len);
		if (client_sock == INVALID_SOCKET)
		{
			printf("listen error %d\n", GetLastError());
			break;
		}
		printf("accept from sock[%d] ip[%d.%d.%d.%d] port [%d]\n",
			client_sock,
			(int)client_addr.sin_addr.S_un.S_un_b.s_b1,
			(int)client_addr.sin_addr.S_un.S_un_b.s_b2,
			(int)client_addr.sin_addr.S_un.S_un_b.s_b3,
			(int)client_addr.sin_addr.S_un.S_un_b.s_b4,
			ntohs(client_addr.sin_port)
		);
		HANDLE hthread = CreateThread(NULL, 0, tcp_recv_buf, (LPVOID)client_sock, 0, NULL);
		if (hthread == nullptr)
		{
			printf("thread error\n");
			break;
		}
		CloseHandle(hthread);
	}
	closesocket(sock);
	WSACleanup();
}

void udp_server()
{
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata))
	{
		printf("startup error %d\n", GetLastError());
		return;
	}
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);
	memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));
	int ret = bind(sock, (sockaddr*)&server_addr, sizeof(server_addr));
	if (ret)
	{
		printf("bind error %d\n", GetLastError());
		return;
	}
	char buf[100];
	sockaddr_in from;
	int fromlen = sizeof(from);
	while (true)
	{
		Sleep(request_interval_millisecond);
		ret = recvfrom(sock, buf, sizeof(buf), 0, (sockaddr*)&from, &fromlen);
		if (ret == SOCKET_ERROR)
		{
			printf("recvfrom error %d\n", GetLastError());
			continue;
		}
		last_recvfrom_addr = from;
		ProcessMessage(sock, buf, ret);
	}
	closesocket(sock);
	WSACleanup();
}