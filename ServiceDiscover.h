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
	void StartResponseService();//ͬ�������,�ظ�����
	void StartAskLiveService();//ѯ�ʰ�װͬ�������
	bool close();

	~ServiceDiscover();

	friend DWORD WINAPI Response(LPVOID);
	friend DWORD WINAPI AskLive(LPVOID);
};
