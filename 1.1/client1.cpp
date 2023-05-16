#include <WINSOCK2.H>
#include <STDIO.H>
 
#pragma  comment(lib,"ws2_32.lib")
 
int main(int argc, char* argv[])
{
	//初始化WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		return 0;
	}
	//创建套接字
	SOCKET sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sclient == INVALID_SOCKET)
	{
		printf("invalid socket !");
		return 0;
	}
	//绑定IP和端口
	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(8000);
	serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//127.0.0.1 本地地址或者127.0.0.1都可以
	if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		printf("connect error !");
		closesocket(sclient);
		system("pause");
		return 0;
	}
	//发送数据
	char  sendData[] = "Hello server i'm client\n";
	send(sclient, sendData, strlen(sendData), 0);
	printf(sendData);
	char recData[255];
	int ret = recv(sclient, recData, 255, 0);
	if (ret > 0)
	{
		recData[ret] = 0x00;
		printf(recData);
	}
	//关闭
	closesocket(sclient);
	WSACleanup();
	system("pause");
	return 0;
}