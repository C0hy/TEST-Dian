#include<stdio.h>
#include<string.h>
#include<stdlib.h>
//用于数据接收解析的结构体
struct messg
{
    int method;
    char ip[15];
    int port;
    char CSeq[15];
    char content[500];
};

//用于存储已有用户名和密码的结构体
struct userInfo
{
	char username[20];
	char password[20];
	int sign;//标记有没有存入数据
};


//将字符串转为整数
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

//判断是否为数字字符
int isdig(char ch)
{
    if (ch >= '0' && ch <= '9') return 1;
    else return 0;
}

//字符串复制
void copy(char s[], char str[])
{
    int i = 0, j = 0;
    for (; str[j] != '\0'; j++, i++)
    {
        s[i] = str[j];
    }
    s[i] = '\0';
}

//数据接收解析函数
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

//O方法的返回语句构造函数
void Osend(struct messg* te,char* sendData,int port,int n)
{
    char Data[255] = "0.5 200 OK\nCSeq:";
    strcat(Data,te->CSeq);
	strcat(Data,"\n");
    if(n==1)
    {
        strcat(Data,"method-list:OPTIONS,SETUP,PLAY,TEARDOWN.\n(Your port:");
        char pt[15];
        itoa(port,pt,10);
        strcat(Data,pt);
        strcat(Data,")\n");
        copy(sendData,Data);
    }
    else if(n==0)
    {
        strcat(Data,"This username dose not exist!\n");
        copy(sendData,Data);
    }
    else if(n==2)
    {
        strcat(Data,"Wrong password!\n");
        copy(sendData,Data);
    }
}

//S方法的返回语句构造函数
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

//P方法的返回语句构造函数
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

//T方法的返回语句构造函数
void Tsend(struct messg* te,char* sendData)
{
    char Data[255]= "0.5 200 OK\nCSeq:";
    strcat(Data,te->CSeq);
	strcat(Data,"\n");
	strcat(Data,"The session_id is wrong!");
	strcat(Data,"\n");
    copy(sendData,Data);
}

//已有用户信息的录入函数
void make(struct userInfo userinfo[],char *info,int u)
{
    int i=0,j=0;
    for(;info[i]!=':';i++);
    i++;
    for(;info[i]!='/';i++,j++)
    {
        userinfo[u].username[j]=info[i];
    }
    userinfo[u].username[j]='\0';
    for(;info[i]!=':';i++);
    i++;
    for(j=0;info[i]!='/';i++,j++)
    {
        userinfo[u].password[j]=info[i];
    }
    userinfo[u].password[j]='\0';
    userinfo[u].sign=1;
}

//确认请求连接用户信息是否已有或者是否正确
int check(char* con,struct userInfo userinfo[])
{
    char name[20];
    char pass[20];
    int i=0,j=0;
    for(;con[i]!='=';i++);
    i++;
    for(;con[i]!=',';i++,j++)
    {
        name[j]=con[i];
    }
    name[j]='\0';
    for(;con[i]!='=';i++);
    i++;
    for(j=0;con[i]!='\n';i++,j++)
    {
        pass[j]=con[i];
    }
    pass[j]='\0';
    for(i=0;userinfo[i].sign==1;i++)
    {
        if(strcmp(name,userinfo[i].username)==0)
        {
            if(strcmp(pass,userinfo[i].password)==0)
            {
                return 1;
            }
            else
            {
                return 2;
            }
        }
    }
    return 0;
}