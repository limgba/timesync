#include "server.h"
#include "client.h"
#include <stdio.h>
#include "config.h"
#include "protocol.h"


int main(int argc, char* argv[])
{
	ReadMe();
	ReadConfig();
	switch(process_type)
	{
	case PROCESS_TYPE_SERVER:
	{
		printf("I am timesync server\n");
		switch (protocol)
		{
		case IPPROTO_TCP:
		{
			tcp_server();
		}
		break;
		case IPPROTO_UDP:
		{
			udp_server();
		}
		break;
		}
	}
	break;
	case PROCESS_TYPE_CLIENT:
	{
		printf("I am timesync client\n");
		switch (protocol)
		{
		case IPPROTO_TCP:
		{
			tcp_client();
		}
		break;
		case IPPROTO_UDP:
		{
			udp_client();
		}
		break;
		}
	}
	break;
	}
}

