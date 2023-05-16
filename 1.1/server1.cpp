#include<stdio.h>
#include<stdlib.h>
#include <winsock2.h>
 
#pragma comment(lib, "ws2_32.lib")
 
 
#define PORT 1500; //端口号
#define BACKLOG 5; //最大监听数
 
int main(int argc, char* argv[])
{
	//初始化WSA windows下异步套接字的启动命令
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}
	//创建套接字
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET)
	{
		printf("socket error !");
		return 0;
	}
	//绑定IP和端口
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8000);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("bind error !");
	}
	//开始监听
	if (listen(slisten, 5) == SOCKET_ERROR)
	{
		printf("Listen error!\n");
		return 0;
	}
	//循环接收数据
	SOCKET sClient;
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	char revData[255];
	while (true)
	{
		printf("waiting\n");
		sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
		if (sClient == INVALID_SOCKET)
		{
			printf("accept error");
			continue;
		}
		printf("one connect %s \r\n", inet_ntoa(remoteAddr.sin_addr));
 
		//接收数据
		int ret = recv(sClient, revData, 255, 0);
		if (ret > 0)
		{
			revData[ret] = 0x00;
			printf(revData);
		}
		//发送数据
		char  sendData[] = "Hello client\n";
		send(sClient, sendData, strlen(sendData), 0);
		closesocket(sClient);
	}
	closesocket(slisten);
	WSACleanup();
	system("pause");
	return 0;
}