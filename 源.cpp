#include <stdio.h>
#include <iostream>
#include "ServiceDiscover.h"

using namespace std;

int main(){
	ServiceDiscover server;
	server.StartAskLiveService();
	server.StartResponseService();
	while (1){
	Sleep(2000);
	}
	
}