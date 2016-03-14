#include <stdio.h>
#include <iostream>
#include "ServiceDiscover.h"



using namespace std;

ServiceDiscover::ServiceDiscover()
:_udp_socket(-1), _udp_port(8082)
{
	upd_socketIitiollize();
}

ServiceDiscover::~ServiceDiscover()
{
	close();
}

void ServiceDiscover::upd_socketIitiollize(){
	WSADATA wsdata;
	//启动SOCKET库，版本为2.0  
	if (WSAStartup(0x0202, &wsdata) != 0){
		printf("startup is failed\n");
		return;
	}
	////询问socket
	_client_add.sin_family = AF_INET;
	_client_add.sin_addr.s_addr = htons(INADDR_ANY);
	_client_add.sin_port = htons(0);

	this->_udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (this->_udp_socket == INVALID_SOCKET){
		printf("udp socket 初始化错误\n");
		return;
	}
	///设置为广播
	int opt = 1;
	int ret = setsockopt(this->_udp_socket, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(opt));
	if (ret != 0){
		printf("udp socket 初始化错误\n");
		return;
	}
	///设置非阻塞
	u_long optt = 1;
	int iResult = ioctlsocket(this->_udp_socket, FIONBIO, &optt);
	if (iResult != NO_ERROR){
		printf("ioctlsocket failed with error: %ld\n", iResult);
		return;
	}

	/////////////////////消息返回socket
	_server_add.sin_family = AF_INET;
	_server_add.sin_addr.s_addr = htons(INADDR_ANY);
	_server_add.sin_port = htons(this->_udp_port);

	this->_udp_server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (this->_udp_server == INVALID_SOCKET){
		printf("udp server socket 初始化错误\n");
		return;
	}

	if ((setsockopt(this->_udp_server, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(opt))) != 0){
		printf("server socket 初始化错误\n");
		return;
	}

	iResult = ioctlsocket(this->_udp_server, FIONBIO, &optt);
	if (iResult != NO_ERROR){
		printf("ioctlsocket failed with error: %ld\n", iResult);
		return;
	}

	if (bind(this->_udp_server, (sockaddr *)&(this->_server_add), sizeof(sockaddr_in)) != 0){
		printf("bind failed\n");
		return;
	}
}



bool ServiceDiscover::close(){
	WSACleanup();
	return false;
}

DWORD WINAPI AskLive(LPVOID pM){
	ServiceDiscover*  that = (ServiceDiscover*)pM;
	sockaddr_in  addto;

	
	ZeroMemory(&(addto), sizeof(addto));
	(addto).sin_family = AF_INET;
	(addto).sin_addr.s_addr = INADDR_BROADCAST;
	(addto).sin_port = htons(that->_udp_port);
	int nlen = sizeof(addto);

	const char * msg = "client :hello server I am Client";

	while (1){
		int ret = sendto(that->_udp_socket, msg, (int)strlen(msg), 0, (sockaddr *)&(addto), nlen);

		if (ret < 0)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK || err == WSAENOBUFS){
				Sleep(2000);
				
			}
			else{
				printf("scoket errno %d\n", err);
				break;
			}
		}

		DWORD k = ::GetTickCount();
		long beginTime = k / 1000; // se为秒
		long endTime = beginTime + 20;

		while (beginTime < endTime){

			char buf[256];
			ZeroMemory(buf, 256);
			int handle = that->_udp_socket;
			int re = recvfrom(that->_udp_socket, buf, 256, 0, (sockaddr *)&(addto), &nlen);
			if (re>0){
				buf[re] = '\0';
				printf("client : Hi server my recvice back infomation \n");
			}
			else{
				int err = WSAGetLastError();
				if (err == WSAEWOULDBLOCK||err == WSAENOBUFS){
					Sleep(2000);
				}
				else{
					printf("scoket errno %d\n", err);
					break;
				}
				beginTime = ::GetTickCount() / 1000;
			}
			Sleep(2000);
		}

		Sleep(10000);
	}

	return NULL;
}

DWORD WINAPI Response(LPVOID pM){

	ServiceDiscover* that = (ServiceDiscover*)pM;
	sockaddr_in from;

	int nAddrLen = sizeof(from);
	while (1)
	{
		char buf[256];
		ZeroMemory(&from, nAddrLen);
		int ret = recvfrom(that->_udp_server, buf, 256, 0, (struct sockaddr FAR *)&from, (int FAR *)&nAddrLen);
		if (ret > 0){
			char * buffer = (char*)malloc(ret + 1);
			memcpy(buffer, buf, ret);
			buffer[ret] = '\0';
			printf("server : hello client my recvice your infomation \n");
			
			int send = sendto(that->_udp_server, buffer, strlen(buffer), 0, (struct sockaddr FAR *)&from, nAddrLen);
			free(buffer);
			ZeroMemory(buf, 256);
		}
		else{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK||err == WSAENOBUFS){
				Sleep(2000);
			}
			else{
				printf("scoket errno %d\n", err);
				break;
			}
		}
	}
	return NULL;
}

void ServiceDiscover::StartAskLiveService(){
	HANDLE handle = CreateThread(NULL, 0, AskLive, (void*)this, 0, NULL);
}

void ServiceDiscover::StartResponseService(){
	HANDLE handle = CreateThread(NULL, 0, Response, (void*)this, 0, NULL);
}