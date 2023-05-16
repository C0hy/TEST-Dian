#include<stdio.h>
#include<stdlib.h>
#include <winsock2.h>
#include <time.h>
#include"server-flex.h"
 
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

	//绑定自身接收的IP和端口
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

    //存储用户相关信息
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
		printf("one connect:\n");

		int seesion_id=0;//一个对话只存在一个

		//接收数据
		int ret = recv(sClient, revData, 255, 0);
		while (ret > 0)
		{
			struct messg result= {0,0,' ',0,0,' '};
            struct messg* te = &result;
            aly(revData, te);
			if(te->method==1)
			{
				char sendData[255] = "0.5 200 OK\nCSeq:";
                strcat(sendData,te->CSeq);
				strcat(sendData,"\n");
				strcat(sendData,"method-list:OPTIONS,SETUP,PLAY,TEARDOWN.\n");
				send(sClient, sendData, strlen(sendData), 0);
				printf("done:CSeq %s\n",te->CSeq);
			}
			else if(te->method==2)
			{
                srand((unsigned)time(NULL));
                seesion_id=rand();
				char sendData[255] = "0.5 200 OK\nCSeq:";
                strcat(sendData,te->CSeq);
				strcat(sendData,"\n");
				strcat(sendData,"seesion_id=");
				char str[10];
				itoa(seesion_id,str,10);
				strcat(sendData,str);
				strcat(sendData,"\n");
				send(sClient, sendData, strlen(sendData), 0);
				printf("done:CSeq %s\n",te->CSeq);
			}
			else if(te->method==3)
			{

				char con[255];
				char starTime[10],endTime[10];
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
                }
				{
					for(;con[i]!='=';i++);
					i++;
					{//获取开始播放时间留进一步使用
					    int j=0;
                        starTime[10];
                        for (; con[i] != '-'; i++, j++) starTime[j] = con[i];
                        starTime[j] = '\0';
					}
                    i++;
					{//获取结束播放时间留进一步使用
					    int j=0;
                        endTime[10];
                        for (;isdig(con[i]); i++, j++) endTime[j] = con[i];
                        endTime[j] = '\n';
					}
					//发送视频文件
					{
						char sendData[255] = "0.5 200 OK\nCSeq:";
                        strcat(sendData,te->CSeq);
				        strcat(sendData,"\n");
				        strcat(sendData,"A video file...(100bytes) time:");
						strcat(sendData,starTime);
						strcat(sendData,"to");
						strcat(sendData,endTime);
					    send(sClient, sendData, strlen(sendData), 0);
						printf("done:CSeq %s\n",te->CSeq);
					}
				}


			}
			else if(te->method==4)
			{
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
                }
				if (SI==seesion_id)
				{
					closesocket(sClient);
				}
				printf("done:CSeq %s\n",te->CSeq);
			}

			ret = recv(sClient, revData, 255, 0);
		}
	}
	closesocket(slisten);
	WSACleanup();
	system("pause");
	return 0;
}