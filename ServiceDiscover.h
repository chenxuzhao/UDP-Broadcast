#pragma once

#include <WinSock2.h>

#pragma comment (lib ,"ws2_32.lib")



class ServiceDiscover
{
private:
	int  _udp_server;
	int  _udp_socket;
	int  _udp_port;
	sockaddr_in  _client_add;
	sockaddr_in  _server_add;
	
public:

	ServiceDiscover();
	void upd_socketIitiollize();
	void StartResponseService();//同软件主机,回复服务
	void StartAskLiveService();//询问安装同软件主机
	bool close();

	~ServiceDiscover();

	friend DWORD WINAPI Response(LPVOID);
	friend DWORD WINAPI AskLive(LPVOID);
};
