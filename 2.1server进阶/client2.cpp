#include <WINSOCK2.H>
#include <stdio.h>
#include"client-flex.h"
 
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

	//绑定服务端的IP和端口
	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(8000);
	serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		printf("connect error !");
		closesocket(sclient);
		system("pause");
		return 0;
	}

	char recData[255];
	struct rec result= {0,' '};
    struct rec* te = &result;
	int seesion_id=0;
	char Cseesion_id[10];//每次对话唯一

	//method-OPTIONS
	{
	    char  sendData[] = "OPTIONS diantp://127.0.0.1:8001 0.5\nCSeq:200\n";
	    send(sclient, sendData, strlen(sendData), 0);
	    printf("Send:\n%s",sendData);
	    int ret = recv(sclient, recData, 255, 0);
	    if(ret>0)
	    {
		    ana(recData,te);
	        printf("Receive:\n%s\n",te->content);
	    }
	}

	//method-SETUP
	{
	    char  sendData[] = "SETUP diantp://127.0.0.1:8001 0.5\nCSeq:201\nTransport:TCP,client_prot=9000\n";
	    send(sclient, sendData, strlen(sendData), 0);
	    printf("Send:\n%s",sendData);
	    int ret = recv(sclient, recData, 255, 0);
	    if(ret>0)
	    {
		    ana(recData,te);
	        printf("Receive:\n%s\n",te->content);
			char con[255];
			copy(con,te->content);
			int i=0,SI;
			for(;con[i]!='=';i++);
			i++;
		    {//确认seesion_id
                int j = 0;
                char str[10];
                for (;isdig(con[i]); i++, j++) str[j] = con[i];
                str[j] = '\0';
                SI = ascii_to_integer(str);
				copy(Cseesion_id,str);
            }
			seesion_id=SI;
	    }
	}

	//method-PLAY
	{
	    char  sendData[] = "PLAY diantp://127.0.0.1:8001 0.5\nCSeq:202\nseesion_id=";
		strcat(sendData,Cseesion_id);
		strcat(sendData,",Range:ntp=123-456\n");
	    send(sclient, sendData, strlen(sendData), 0);
	    printf("Send:\n%s",sendData);
	    int ret = recv(sclient, recData, 255, 0);
	    if(ret>0)
	    {
		    ana(recData,te);
	        printf("Receive:\n%s\n",te->content);
	    }
	}

	//method-TEARDOWN
	{
	    char  sendData[] = "TEARDOWN diantp://127.0.0.1:8001 0.5\nCSeq:203\nseesion_id=";
		strcat(sendData,Cseesion_id);
		strcat(sendData,"\n");
	    send(sclient, sendData, strlen(sendData), 0);
	    printf("Send:\n%s",sendData);
	}

	//关闭
	closesocket(sclient);
	WSACleanup();
	system("pause");
	return 0;
}