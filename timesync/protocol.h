#pragma once

#include <WinSock2.h>
#include <process.h>
#include "config.h"

enum SERVER_MESSAGE_NUM
{
	SERVER_MESSAGE_NUM_NONE,
	SERVER_MESSAGE_NUM_SYNC,
};

enum CLIENT_MESSAGE_NUM
{
	CLIENT_MESSAGE_NUM_NONE,
	CLIENT_MESSAGE_NUM_CHECK,
	CLIENT_MESSAGE_NUM_SYNC_RET,
};

struct MessageHeader
{
	MessageHeader() {}
	MessageHeader(unsigned short _process_type, unsigned short _message_num);

	int process_id = _getpid();
	unsigned short process_type = 0;
	unsigned short message_num = 0;
};

struct SyncTime
{
	SyncTime();
	MessageHeader head;
	SYSTEMTIME time;
};
static_assert(MAX_BUF_LEN >= sizeof(SyncTime), "buf len too short");

struct CheckTime
{
	CheckTime();
	MessageHeader head;
};

struct SyncTimeRet
{
	SyncTimeRet();
	MessageHeader head;
	SYSTEMTIME time;
};

void ProcessServerMessage(SOCKET sock, const MessageHeader& head, void* buf, int buf_len);
void ProcessClientMessage(SOCKET sock, const MessageHeader& head, void* buf, int buf_len);
void ProcessMessage(SOCKET sock, void* buf, int buf_len);

extern sockaddr_in last_recvfrom_addr;
