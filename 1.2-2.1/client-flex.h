#include<stdio.h>
struct rec
{
    long CSeq;
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