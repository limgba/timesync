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

