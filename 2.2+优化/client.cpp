#include <WINSOCK2.H>
#include <stdio.h>
#include <string.h>
#include <time.h>
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
	
	//读取外部文件，其中存有原来得到的需要通讯的服务器端的IP和PORT
	char url[40],IP[20],PORT[20];
	int port;
	FILE *fp;
    fp = fopen("url.txt" , "r");
    if(fp == NULL) 
    {
        perror("url error...");
        return(-1);
    }
    fgets (url, 40, fp);
    fclose(fp);
	//解析IP和PORT
	{
		int i=0,j=0;
		for(;!isdig(url[i]);i++);
		for(;url[i]!=':';j++,i++)
		{
			IP[j]=url[i];
		}
		IP[j]='\0';
		i++;
		for(j=0;isdig(url[i]);j++,i++)
		{
			PORT[j]=url[i];
		}
		PORT[j]='\0';
		port=ascii_to_integer(PORT);
	}


	//绑定服务端的IP和端口
	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(port);
	serAddr.sin_addr.S_un.S_addr = inet_addr(IP);
	if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		printf("connect error !");
		closesocket(sclient);
		system("pause");
		return 0;
	}


	//对话准备
	srand((unsigned)time(NULL));
    int CSeq=rand();
	char recData[255];
	char cho[10];
	struct rec result= {0,' '};
    struct rec* te = &result;
	int session_id=0;
	char Csession_id[10];//每次对话唯一
	char myport[20];

    //初始化链表
	struct request_record* head,* p1,* p2;
	head=(struct request_record*)malloc(sizeof(request_record));
	p1=head;
	head->next=NULL;

	while(cho[0]!='T')
	{
        printf("Your method:");
	    scanf("%s",cho);

	    //method-OPTIONS
	    if(cho[0]=='O')
	    {
			//按协议发送信息
			CSeq++;
	        char  sendData[255];
		    Os(sendData,url,CSeq);
	        send(sclient, sendData, strlen(sendData), 0);
	        printf("Send:\n%s",sendData);
			//信息填入链表
			p2=(struct request_record*)malloc(sizeof(request_record));
			copy(p2->cont,sendData);
            struct messg* me = &p2->mes;
			aly(sendData, me);
			p1->next=p2;
			p1=p2;
			p1->next=NULL;
			//接收信息，并按协议解析
	        int ret = recv(sclient, recData, 255, 0);
	        if(ret>0)
	        {
		    ana(recData,te);
	        printf("Receive:\n%s\n",te->content);
	        }
			//由服务端返回的内容获得客户端的PORT
			int i=0,j=0;
			for(;te->content[i]!='(';i++);
			for(;te->content[i]!=':';i++);
			i++;
			for(;isdig(te->content[i]);i++,j++)
			{
				myport[j]=te->content[i];
			}
			myport[j]='\0';
	    }

	    //method-SETUP
	    else if(cho[0]=='S')
	    {
			//按协议发送信息
			CSeq++;
			printf("Your Transport(TCP/UDP):");
			char tp[10];
	        scanf("%s",tp);
	        char  sendData[255];
	        Ss(sendData,url,CSeq,tp,myport);
			send(sclient, sendData, strlen(sendData), 0);
	        printf("Send:\n%s",sendData);
			//信息填入链表
			p2=(struct request_record*)malloc(sizeof(request_record));
			copy(p2->cont,sendData);
            struct messg* me = &p2->mes;
			aly(sendData, me);
			p1->next=p2;
			p1=p2;
			p1->next=NULL;
			//接收信息，并按协议解析
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
		        {//收集seesion_id
                    int j = 0;
                    char str[10];
                    for (;isdig(con[i]); i++, j++) str[j] = con[i];
                    str[j] = '\0';
                    SI = ascii_to_integer(str);
			    	copy(Csession_id,str);
                }
			    session_id=SI;
	        }
	    }

	    //method-PLAY
	    else if(cho[0]=='P')
	    {
			//按协议发送信息
			CSeq++;
			printf("Time Range(xxx-xxx):");
			char time[10];
	        scanf("%s",time);
	        char  sendData[255];
			Ps(sendData,url,CSeq,Csession_id,time);
	        send(sclient, sendData, strlen(sendData), 0);
	        printf("Send:\n%s",sendData);
			//信息填入链表
			p2=(struct request_record*)malloc(sizeof(request_record));
			copy(p2->cont,sendData);
            struct messg* me = &p2->mes;
			aly(sendData, me);
			p1->next=p2;
			p1=p2;
			p1->next=NULL;
			//接收信息，并按协议解析
	        int ret = recv(sclient, recData, 255, 0);
	        if(ret>0)
	        {
		        ana(recData,te);
	            printf("Receive:\n%s\n",te->content);
	        }
	    }   

	    //method-TEARDOWN
		else if(cho[0]=='T')
	    {
			//按协议发送信息
			CSeq++;
	        char  sendData[255];;
			Ts(sendData,url,CSeq,Csession_id);
	        send(sclient, sendData, strlen(sendData), 0);
	        printf("Send:\n%s",sendData);
			//信息填入链表
			p2=(struct request_record*)malloc(sizeof(request_record));
			copy(p2->cont,sendData);
            struct messg* me = &p2->mes;
			aly(sendData, me);
			p1->next=p2;
			p1=p2;
			p1->next=NULL;
	    }
	}

	//关闭
	closesocket(sclient);
	//打印本次通话信息
	printf("Request_record:\n");
	p1=head;
	while(p1->next)
	{
		p1=p1->next;
		printf("%s",p1->cont);
	}
	free(head);
	free(p1);
	free(p2);
	WSACleanup();
	system("pause");
	return 0;
}