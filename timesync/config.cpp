#include "config.h"
#include <fstream>
#include <winsock2.h>

const char* config_name = "config.ini";
const char* config_split = "=";
const char* config_ignore = "#";
const char* config_line = "\n";


const char* protocol_str = "protocol";
int protocol = IPPROTO_TCP;

const char* process_type_str = "process_type";
int process_type = PROCESS_TYPE_CLIENT;

const char* ip_str = "ip";
char ip[17] = "127.0.0.1";

const char* port_str = "port";
unsigned short port = 43333;

const char* request_interval_millisecond_str = "request_interval_millisecond";
int request_interval_millisecond = 1000;

std::string config_content_str =
"#进程类型, 服务器:1, 客户端:2\n" +
std::string(process_type_str) + config_split + std::to_string(process_type) + config_line + config_line +
"#服务器地址\n" +
ip_str + config_split + ip + config_line + config_line +
"#服务器端口\n" +
port_str + config_split + std::to_string(port) + config_line + config_line +
"#客户端请求铜不的间隔(毫秒)\n" +
request_interval_millisecond_str + config_split + std::to_string(request_interval_millisecond) + config_line + config_line +
"#协议类型, 默认为6, tcp:6, udp:17\n" +
protocol_str + config_split + std::to_string(protocol) + config_line + config_line;

std::string help_content_str = 
"这是一个时间铜不工具\n"
"原理: 客户端向服务器请求时间, 若与服务器返回的时间差大于5秒, 则设置客户端的电脑时间\n"
"使用方法: 首次使用会生成config.ini, 请查看以下说明\n\n"
"编辑config.ini\n"
"如果是服务器, 则process_type改为1, 如果是客户端, process_type改为2\n"
"填写服务器地址和端口, 其他的选项为可选, 如有需要再改\n"
"以管理员身份启动timesync.exe(因为普通用户没有修改时间的权限)\n"
;

void ReadConfig()
{
	std::ifstream ifs;
	ifs.open(config_name);
	do
	{
		if (ifs.is_open())
		{
			break;
		}
		if (!CreateConfig())
		{
			printf("can't create %s\n", config_name);
			return;
		}
		ifs.open(config_name);
		if (!ifs.is_open())
		{
			printf("can't read %s\n", config_name);
			return;
		}
	} while (false);

	char buf[128];
	std::string str;
	while (ifs.getline(buf, sizeof(buf)))
	{
		str = buf;
		if (std::string::npos != str.find_first_of(config_ignore))
		{
			continue;
		}
		auto index = str.find_first_of(config_split);
		if (index == std::string::npos)
		{
			continue;
		}
		std::string name = str.substr(0, index);
		std::string data = str.substr(index + 1);
		if (name == ip_str)
		{
			if (data.size() > sizeof(ip))
			{
				printf("ip format error\n");
				return;
			}
			memset(ip, 0, sizeof(ip));
			memcpy(ip, data.c_str(), data.size());
		}
		else if (name == port_str)
		{
			port = atoi(data.c_str());
		}
		else if (name == request_interval_millisecond_str)
		{
			request_interval_millisecond = atoi(data.c_str());
		}
		else if (name == process_type_str)
		{
			process_type = atoi(data.c_str());
		}
		else if (name == protocol_str)
		{
			protocol = atoi(data.c_str());
		}
	}
	ifs.close();
}

bool CreateConfig()
{
	std::ofstream ofs;
	ofs.open(config_name);
	if (!ofs.is_open())
	{
		printf("can't open create %s\n", config_name);
		return false;
	}
	ofs.write(config_content_str.c_str(), config_content_str.size());
	ofs.close();
	return true;
}

void ReadMe()
{
	std::ifstream ifs;
	ifs.open("readme.txt", std::ios_base::in);
	if (ifs.is_open())
	{
		ifs.close();
		return;
	}
	std::ofstream ofs;
	ofs.open("readme.txt");
	if (!ofs.is_open())
	{
		return;
	}
	ofs.write(help_content_str.c_str(), help_content_str.size());
	ofs.close();
}
