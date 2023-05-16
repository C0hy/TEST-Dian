#include<stdio.h>
#include<stdlib.h>
#include <winsock2.h>
#include <time.h>
#include"server-flex.h"
#include<pthread.h>
 
#pragma comment(lib, "ws2_32.lib")
 
 
#define PORT 1500; //端口号
#define MAX 50; //最大监听数

//储存不同进程信息，最多50个
struct sockInfo
{
	int fd;
	struct sockaddr_in addr;
	
};
struct sockInfo infos[50];


//储存已有能接入用户信息，最多50个
struct userInfo userinfo[50]; 

unsigned working(void* p);

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

	//从用户信息文件中读入可以登录的用户信息
	for(int i=0;i<50;i++)
	{
		userinfo[i].sign=0;//标记初始化
	}
	char info[50];
	FILE *fp;
    fp = fopen("userinfo.txt" , "r");
    if(fp == NULL) 
   	{
        perror("userinfo error...");
        return(-1);
    }
	int u=0;//标记读入第几个用户信息
    while(fgets (info, 50, fp)!=NULL)
	{
		make(userinfo,info,u);
		u++;
	}
   	fclose(fp);

	//开始监听
	if (listen(slisten, 5) == SOCKET_ERROR)
	{
		printf("Listen error!\n");
		return 0;
	}
	printf("waiting\n");

	SOCKET sClient;
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	int max=MAX;
	for(int i=0; i<max ; i++)
	{
		infos[i].fd=-1;
	}
	while(1)
	{
		struct sockInfo* pfo;
		for(int i=0;i<max;i++)
		{
			if(infos[i].fd==-1)
			{
				pfo=&infos[i];
				break;
			}
		}
		sClient = accept(slisten, (SOCKADDR *)&pfo->addr, &nAddrlen);
		if (sClient == INVALID_SOCKET)
		{
			printf("accept error");
			continue;
		}
		printf("one connect IP:%s,PORT:%d\n",inet_ntoa(pfo->addr.sin_addr),pfo->addr.sin_port);
        pfo->fd=sClient;
		_beginthreadex(NULL,0,working,pfo,0,NULL);
	}
	closesocket(slisten);
	WSACleanup();
	system("pause");
	return 0;
}


unsigned working(void* p)
{
	struct sockInfo* pfo=(struct sockInfo*)p;
	SOCKET sClient=pfo->fd;
	char revData[255];
	while (true)
	{
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
				int n=check(te->content,userinfo);//标记匹配情况:1成功、0没有用户、2密码错误
				char sendData[255];
				Osend(te,sendData,pfo->addr.sin_port,n);
				send(sClient, sendData, strlen(sendData), 0);
				if(n==1)
				{
					printf("Done:CSeq %s From IP:%s,PORT:%d\n",te->CSeq,inet_ntoa(pfo->addr.sin_addr),pfo->addr.sin_port);
				}
			    if(n==0||n==2)
				{
					printf("Error:CSeq %s From IP:%s,PORT:%d\n------Login failed!---EXIT!\n",te->CSeq,inet_ntoa(pfo->addr.sin_addr),pfo->addr.sin_port);
					closesocket(sClient);
					pfo->fd=-1;
					return 0;
				}
			}
			else if(te->method==2)
			{
				char sendData[255];
                srand((unsigned)time(NULL));
                seesion_id=rand();
			    Ssend(te,sendData,seesion_id);
				send(sClient, sendData, strlen(sendData), 0);
				printf("Done:CSeq %s seesion=%d From IP:%s,PORT:%d\n",te->CSeq,seesion_id,inet_ntoa(pfo->addr.sin_addr),pfo->addr.sin_port);
			}
			else if(te->method==3)
			{

				char con[255];
				char startTime[10],endTime[10];
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
				if(SI==seesion_id)
				{
					for(;con[i]!='=';i++);
					i++;
					{//获取开始播放时间留进一步使用
					    int j=0;
                        startTime[10];
                        for (; con[i] != '-'; i++, j++) startTime[j] = con[i];
                        startTime[j] = '\0';
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
						char sendData[255]; 
						Psend(te,sendData,startTime,endTime);
					    send(sClient, sendData, strlen(sendData), 0);
						printf("Done:CSeq %s From IP:%s,PORT:%d\n",te->CSeq,inet_ntoa(pfo->addr.sin_addr),pfo->addr.sin_port);
					}
				}
				else
				{
					char sendData[255];
					Tsend(te,sendData);
				    send(sClient, sendData, strlen(sendData), 0);
					printf("Error:CSeq %s From IP:%s,PORT:%d\n------Wrong session_id! Do not send!\n",te->CSeq,inet_ntoa(pfo->addr.sin_addr),pfo->addr.sin_port);
					closesocket(sClient);
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
					printf("Done:CSeq %s From IP:%s,PORT:%d\n------EXIT!\n",te->CSeq,inet_ntoa(pfo->addr.sin_addr),pfo->addr.sin_port);
				}
				else
				{
					char sendData[255];
					Tsend(te,sendData);
				    send(sClient, sendData, strlen(sendData), 0);
					printf("Error:CSeq %s From IP:%s,PORT:%d\n------Close error!\n",te->CSeq,inet_ntoa(pfo->addr.sin_addr),pfo->addr.sin_port);
					closesocket(sClient);
				}
			}

			ret = recv(sClient, revData, 255, 0);
		}
	}
	closesocket(sClient);
	pfo->fd=-1;
	return 0;
}