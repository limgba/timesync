#pragma once
#pragma warning(disable:4996)
#include <string>

enum PROCESS_TYPE
{
	PROCESS_TYPE_NONE,
	PROCESS_TYPE_SERVER,
	PROCESS_TYPE_CLIENT,
};

void ReadConfig();
bool CreateConfig();
void ReadMe();

const int MAX_BUF_LEN = 24;

extern const char* config_name;
extern const char* config_split;
extern const char* config_line;

extern const char* protocol_str;
extern int protocol;

extern const char* process_type_str;
extern int process_type;

extern const char* ip_str;
extern char ip[17];

extern const char* port_str;
extern unsigned short port;

extern const char* request_interval_millisecond_str;
extern int request_interval_millisecond;

extern std::string config_content_str;
extern std::string help_content_str;
