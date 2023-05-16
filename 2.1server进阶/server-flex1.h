#include<stdio.h>
struct messg
{
    int method;
    char ip[15];
    int port;
    char CSeq[15];
    char content[500];
};

int ascii_to_integer(char* string) {
    int x = 0;
    if (!string)
    {
        return 0;
    }
    char ch;
    while (ch = *string++)
    {
        if (ch >= '0' && ch <= '9')
        {
            ch -= '0';
            x = (x * 10 + ch);
        }
        else return 0;
    }
    return x;
}

int isdig(char ch)
{
    if (ch >= '0' && ch <= '9') return 1;
    else return 0;
}

void copy(char s[], char str[])
{
    int i = 0, j = 0;
    for (; str[j] != '\0'; j++, i++)
    {
        s[i] = str[j];
    }
    s[i] = '\0';
}

void aly(char s[], struct messg* result)
{
    int i = 0;
    //确定method
    if (s[i] == 'O')
    {
        i = i + 7;
        result->method = 1;
    }
    else if (s[i] == 'S')
    {
        i = i + 5;
        result->method = 2;
    }
    else if (s[i] == 'P')
    {
        i = i + 4;
        result->method = 3;
    }
    else if (s[i] == 'T')
    {
        i = i + 8;
        result->method = 4;
    }

    for (; s[i] != '/'; i++);
    i=i+2;

    {//收集IP
        int j = 0;
        for (; s[i] != ':'; i++, j++) result->ip[j] = s[i];
        result->ip[j] = '\0';
        i++;
    }
    {//收集port
        int j = 0;
        char str[10];
        for (; s[i] != ' '; i++, j++) str[j] = s[i];
        str[j] = '\0';
        result->port = ascii_to_integer(str);
    }
    for (; s[i] != 'C'; i++);
    i = i + 5;
    {//收集CSeq
        int j = 0;
        for (; s[i] != ' '&&s[i]!='\n'; i++, j++) result->CSeq[j] = s[i];
        result->CSeq[j] = '\0';
    }
    if (result->method == 1) return;
    for (; s[i] == ' '||s[i] == '\n'; i++); 
    {//收集content
        int j = 0;
        for (; s[i] != '\r' && s[i] != '\n'; i++, j++) result->content[j] = s[i];
        result->content[j] = '\0';
    }
}

void Osend(struct messg* te,char* sendData)
{
    char Data[255] = "0.5 200 OK\nCSeq:";
    strcat(Data,te->CSeq);
	strcat(Data,"\n");
	strcat(Data,"method-list:OPTIONS,SETUP,PLAY,TEARDOWN.\n");
    copy(sendData,Data);
}

void Ssend(struct messg* te,char* sendData,int seesion_id)
{
    char Data[255]= "0.5 200 OK\nCSeq:";
    strcat(Data,te->CSeq);
	strcat(Data,"\n");
	strcat(Data,"seesion_id=");
	char str[10];
	itoa(seesion_id,str,10);
	strcat(Data,str);
	strcat(Data,"\n");
    copy(sendData,Data);
}

void Psend(struct messg* te,char* sendData,char* startTime,char* endTime)
{
    char Data[255] = "0.5 200 OK\nCSeq:";
    strcat(Data,te->CSeq);
	strcat(Data,"\n");
	strcat(Data,"A video file...(100bytes) time:");
	strcat(Data,startTime);
	strcat(Data," to ");
	strcat(Data,endTime);
	copy(sendData,Data);
}
