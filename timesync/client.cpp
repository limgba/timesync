#include "client.h"
#include <winsock2.h>
#include <stdio.h>
#include "config.h"
#include "protocol.h"

#pragma comment(lib, "ws2_32.lib")

DWORD WINAPI tcp_client_recv_buf(LPVOID lpThreadParameter)
{
	SOCKET sock = (SOCKET)lpThreadParameter;
	char buf[MAX_BUF_LEN];
	int ret = 0;
	while (true)
	{
		CheckTime check_time;
		ret = send(sock, (const char*)&check_time, sizeof(check_time), 0);
		if (ret == SOCKET_ERROR)
		{
			printf("send socket error sock[%d] error[%d]\n", sock, GetLastError());
			break;
		}
		memset(buf, 0, sizeof(buf));
		ret = recv(sock, buf, sizeof(buf), 0);
		if (ret == 0)
		{
			printf("disconnect sock[%d]\n", sock);
			break;
		}
		if (ret == SOCKET_ERROR)
		{
			printf("recv socket error sock[%d] error[%d]\n", sock, GetLastError());
		}
		ProcessMessage(sock, buf, ret);
		Sleep(request_interval_millisecond);
	}
	return 0;
}

void tcp_client()
{
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata))
	{
		printf("startup error %d\n", GetLastError());
		return;
	}
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in client_addr;
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = inet_addr(ip);
	client_addr.sin_port = htons(port);
	memset(client_addr.sin_zero, 0, sizeof(client_addr.sin_zero));

	int ret = connect(sock, (sockaddr*)&client_addr, sizeof(client_addr));
	if (ret)
	{
		printf("connect error %d\n", GetLastError());
		return;
	}

	HANDLE hthread = CreateThread(NULL, 0, tcp_client_recv_buf, (LPVOID)sock, 0, NULL);
	if (NULL == hthread)
	{
		printf("thread error\n");
	}
	while (true)
	{
		Sleep(request_interval_millisecond * 100);
	}

	CloseHandle(hthread);
	closesocket(sock);
	WSACleanup();
}

void udp_client()
{
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata))
	{
		printf("startup error %d\n", GetLastError());
		return;
	}
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	char buf[100];
	sockaddr_in from, to;
	to.sin_family = AF_INET;
	to.sin_addr.s_addr = inet_addr(ip);
	to.sin_port = htons(port);
	memset(to.sin_zero, 0, sizeof(to.sin_zero));
	int fromlen = sizeof(from);
	int ret = 0;
	CheckTime check_time;
	while (true)
	{
		Sleep(request_interval_millisecond);
		ret = sendto(sock, (const char*)&check_time, sizeof(check_time), 0, (sockaddr*)&to, sizeof(to));
		if (ret == SOCKET_ERROR)
		{
			printf("sendto %s error %d\n", ip, GetLastError());
			continue;
		}
		ret = recvfrom(sock, buf, sizeof(buf), 0, (sockaddr*)&from, &fromlen);
		if (ret == SOCKET_ERROR)
		{
			continue;
		}
		last_recvfrom_addr = from;
		ProcessMessage(sock, buf, ret);
	}
	closesocket(sock);
	WSACleanup();
}
