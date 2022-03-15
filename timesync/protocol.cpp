#include "protocol.h"
#include <stdio.h>
#include <cmath>
#include "config.h"

sockaddr_in last_recvfrom_addr;
MessageHeader::MessageHeader(unsigned short _process_type, unsigned short _message_num)
{
	process_type = _process_type;
	message_num =  _message_num;
}

void ProcessServerMessage(SOCKET sock, const MessageHeader& head, void* buf, int buf_len)
{
	if (process_type != PROCESS_TYPE_CLIENT)
	{
		return;
	}
	switch (head.message_num)
	{
	case SERVER_MESSAGE_NUM_SYNC:
	{
		if (buf_len != sizeof(SyncTime))
		{
			printf("ProcessServerMessage recv error message_num[%d] buf_len[%d]\n", head.message_num, buf_len);
			return;
		}
		SyncTime* msg = (SyncTime*)buf;
		SYSTEMTIME time;
		GetLocalTime(&time);

		if (msg->time.wYear != time.wYear ||
			msg->time.wMonth != time.wMonth ||
			msg->time.wDay != time.wDay ||
			msg->time.wHour != time.wHour ||
			msg->time.wMinute != time.wMinute ||
			std::abs(msg->time.wSecond - time.wSecond) > 4
			)
		{
			int ret = SetLocalTime(&msg->time);
			if (ret)
			{
				printf("设置时间成功 %04d-%02d-%02d %02d:%02d:%02d\n",
					(int)msg->time.wYear,
					(int)msg->time.wMonth,
					(int)msg->time.wDay,
					(int)msg->time.wHour,
					(int)msg->time.wMinute,
					(int)msg->time.wSecond
				);

				SyncTimeRet sync_time_ret;
				memcpy(&sync_time_ret.time, &msg->time, sizeof(msg->time));
				int ret = SOCKET_ERROR;
				switch (protocol)
				{
				case IPPROTO_TCP:
				{
					ret = send(sock, (const char*)&sync_time_ret, sizeof(sync_time_ret), 0);
				}
				break;
				case IPPROTO_UDP:
				{
					ret = sendto(sock, (const char*)&sync_time_ret, sizeof(sync_time_ret), 0, (sockaddr*)&last_recvfrom_addr, sizeof(last_recvfrom_addr));
				}
				break;
				}
				if (ret == SOCKET_ERROR)
				{
					printf("socket error sock[%d] error[%d]\n", sock, GetLastError());
				}
			}
			else
			{
				printf("设置时间失败, 没有权限, 请使用管理员身份运行\n");
			}
		}
	}
	break;
	}
}

void ProcessClientMessage(SOCKET sock, const MessageHeader& head, void* buf, int buf_len)
{
	if (process_type != PROCESS_TYPE_SERVER)
	{
		return;
	}
	switch (head.message_num)
	{
	case CLIENT_MESSAGE_NUM_CHECK:
	{
		if (buf_len != sizeof(CheckTime))
		{
			printf("ProcessClientMessage recv error message_num[%d] buf_len[%d]\n", head.message_num, buf_len);
			return;
		}
		SyncTime sync_time;
		GetLocalTime(&sync_time.time);
		int ret = SOCKET_ERROR;
		switch (protocol)
		{
		case IPPROTO_TCP:
		{
			ret = send(sock, (const char*)&sync_time, sizeof(sync_time), 0);
		}
		break;
		case IPPROTO_UDP:
		{
			ret = sendto(sock, (const char*)&sync_time, sizeof(sync_time), 0, (sockaddr*)&last_recvfrom_addr, sizeof(last_recvfrom_addr));
		}
		break;
		}
		if (ret == SOCKET_ERROR)
		{
			printf("socket error sock[%d] error[%d]\n", sock, GetLastError());
		}
	}
	break;
	case CLIENT_MESSAGE_NUM_SYNC_RET:
	{
		if (buf_len != sizeof(SyncTimeRet))
		{
			printf("ProcessServerMessage recv error message_num[%d] buf_len[%d]\n", head.message_num, buf_len);
			return;
		}
		SyncTimeRet* msg = (SyncTimeRet*)buf;

		printf("sock[%d] 设置时间成功 %04d-%02d-%02d %02d:%02d:%02d\n",
			sock,
			(int)msg->time.wYear,
			(int)msg->time.wMonth,
			(int)msg->time.wDay,
			(int)msg->time.wHour,
			(int)msg->time.wMinute,
			(int)msg->time.wSecond
		);
	}
	break;
	}
}

void ProcessMessage(SOCKET sock, void* buf, int buf_len)
{
	MessageHeader head;
	if (buf_len < sizeof(head))
	{
		printf("ProcessMessage recv error buf_len[%d]\n", buf_len);
		return;
	}
	memcpy(&head, buf, sizeof(head));
	switch (head.process_type)
	{
	case PROCESS_TYPE_SERVER:
	{
		ProcessServerMessage(sock, head, buf, buf_len);
	}
	break;
	case PROCESS_TYPE_CLIENT:
	{
		ProcessClientMessage(sock, head, buf, buf_len);
	}
	break;
	}
}

SyncTime::SyncTime() : head(PROCESS_TYPE_SERVER, SERVER_MESSAGE_NUM_SYNC)
{
	memset(&time, 0, sizeof(time));
}

CheckTime::CheckTime() : head(PROCESS_TYPE_CLIENT, CLIENT_MESSAGE_NUM_CHECK)
{
}

SyncTimeRet::SyncTimeRet() : head(PROCESS_TYPE_CLIENT, CLIENT_MESSAGE_NUM_SYNC_RET)
{
	memset(&time, 0, sizeof(time));
}
