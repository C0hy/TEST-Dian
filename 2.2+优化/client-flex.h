#include<stdio.h>
#include"server-flex.h"
struct rec
{
    long CSeq;
    char content[500];
};

struct request_record
{
    struct request_record* next;
    char cont[255];
    struct messg mes;
};


void ana(char s[], struct rec* result)
{
    int i = 0;
    //定位到CSeq
    for (; s[i] != 'C'; i++);
    if (s[i]=='C') i=i+5;
    {//收集CSeq
        int j = 0;
        char str[10];
        for (;isdig(s[i]); i++, j++) str[j] = s[i];
        str[j] = '\0';
        result->CSeq = ascii_to_integer(str);
    }

    //定位到content
    for (; s[i] == '\n'||s[i]==' '; i++);
    {//收集content
        int j = 0;
        for (; s[i] != '\r' && s[i] != '\n'; i++, j++) result->content[j] = s[i];
        result->content[j] = '\0';
    }

}

void Os(char* sendData,char* url,int CSeq)
{
    char Data[255] = "OPTIONS ";
    strcat(Data,url);
	strcat(Data," 0.5\nCSeq:");
    char cseq[15];
    itoa(CSeq,cseq,10);
	strcat(Data,cseq);
    strcat(Data,"\n");
    copy(sendData,Data);
}

void Ss(char* sendData,char* url,int CSeq,char* tp,char* myport) 
{
    char Data[255] = "SETUP ";
    strcat(Data,url);
	strcat(Data," 0.5\nCSeq:");
    char cseq[15];
    itoa(CSeq,cseq,10);
	strcat(Data,cseq);
    strcat(Data,"\nTransport:");
    strcat(Data,tp);
    strcat(Data,",client_port=");
    strcat(Data,myport);
    strcat(Data,"\n");
    copy(sendData,Data);
}

void Ps(char* sendData,char* url,int CSeq,char* Csession_id,char* time)
{
    char Data[255] = "PLAY ";
    strcat(Data,url);
	strcat(Data," 0.5\nCSeq:");
    char cseq[15];
    itoa(CSeq,cseq,10);
	strcat(Data,cseq);
    strcat(Data,"\nsession_id=");
    strcat(Data,Csession_id);
    strcat(Data,",Range:ntp=");
    strcat(Data,time);
    strcat(Data,"\n");
    copy(sendData,Data);
}

void Ts(char* sendData,char* url,int CSeq,char* Csession_id)
{
    char Data[255] = "TEARDOWN ";
    strcat(Data,url);
	strcat(Data," 0.5\nCSeq:");
    char cseq[15];
    itoa(CSeq,cseq,10);
	strcat(Data,cseq);
    strcat(Data,"\nsession_id=");
    strcat(Data,Csession_id);
    strcat(Data,"\n");
    copy(sendData,Data);
}
