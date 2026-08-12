#include "utils/uuid.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "utils/log.h"

#ifdef _WIN32

bool uuid_compare(const uuid_t *a, const uuid_t *b){
    return IsEqualGUID(a, b);
}

int uuid_parse(const char *str, uuid_t *uuid){
    if(strlen(str) != 36) return -1;
    if(str[8]!='-'||str[13]!='-'||str[18]!='-'||str[23]!='-') return -1;
    // Win32 UUID 是结构体字段，不能按 data[] 写入；用 sscanf 解析
    unsigned int d4[8];
    if(sscanf(str, "%08x-%04hx-%04hx-%02x%02x-%02x%02x%02x%02x%02x%02x",
              &uuid->Data1, (unsigned short*)&uuid->Data2, (unsigned short*)&uuid->Data3,
              &d4[0],&d4[1],&d4[2],&d4[3],&d4[4],&d4[5],&d4[6],&d4[7]) != 11)
        return -1;
    for(int i=0;i<8;i++) uuid->Data4[i]=(unsigned char)d4[i];
    return 0;
}

void uuid_format(const uuid_t *uuid, char *out){
    snprintf(out,37,"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
             uuid->Data1,uuid->Data2,uuid->Data3,
             uuid->Data4[0],uuid->Data4[1],uuid->Data4[2],uuid->Data4[3],
             uuid->Data4[4],uuid->Data4[5],uuid->Data4[6],uuid->Data4[7]);
}

void uuid_print(const uuid_t *uuid){
    char buf[37];
    uuid_format(uuid, buf);
    log_debug("%s", buf);
}

#else

bool uuid_compare(const uuid_t *a, const uuid_t *b){
    return memcmp(a->data, b->data, 16) == 0;
}
int uuid_parse(const char *str, uuid_t *uuid){
    if(strlen(str) != 36) return -1;
    if(str[8]!='-'||str[13]!='-'||str[18]!='-'||str[23]!='-') return -1;
    int i,j;
    for(i=0,j=0;i<36&&j<16;){
        if(str[i]=='-'){i++;continue;}
        int hi,lo;
        char c1=str[i],c2=str[i+1];
        if('0'<=c1&&c1<='9')hi=c1-'0';
        else if('a'<=c1&&c1<='f')hi=c1-'a'+10;
        else if('A'<=c1&&c1<='F')hi=c1-'A'+10;
        else return -1;
        if('0'<=c2&&c2<='9')lo=c2-'0';
        else if('a'<=c2&&c2<='f')lo=c2-'a'+10;
        else if('A'<=c2&&c2<='F')lo=c2-'A'+10;
        else return -1;
        uuid->data[j++]=(uint8_t)((hi<<4)|lo);
        i+=2;
    }
    return j==16?0:-1;
}
void uuid_format(const uuid_t *uuid, char *out){
    snprintf(out,37,
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        uuid->data[0],uuid->data[1],uuid->data[2],uuid->data[3],
        uuid->data[4],uuid->data[5],uuid->data[6],uuid->data[7],
        uuid->data[8],uuid->data[9],uuid->data[10],uuid->data[11],
        uuid->data[12],uuid->data[13],uuid->data[14],uuid->data[15]);
}
void uuid_print(const uuid_t *uuid){
    log_debug(
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        uuid->data[0],uuid->data[1],uuid->data[2],uuid->data[3],
        uuid->data[4],uuid->data[5],uuid->data[6],uuid->data[7],
        uuid->data[8],uuid->data[9],uuid->data[10],uuid->data[11],
        uuid->data[12],uuid->data[13],uuid->data[14],uuid->data[15]);
}

#endif