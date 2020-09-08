#include "multimod.h"
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <math.h>
int cnt[10],num[10][256];
//num[i]存储二进制1有i个的数(0~244)
int64_t bitget(int id)//id=0~7
{
    int64_t ret;
    ret=(1<<id)-1;
    for (int t,i=1;i<8;++i)
    {
        t=rand()%(cnt[id]+cnt[id+1]);
        if (t>=cnt[id]) ret=((ret<<8)|num[id+1][t-cnt[id]]);
        else ret=((ret<<8)|num[id][t]);
    }
    return ret&(0x7fffffffffffffffLL);
}
void Init()
{
    srand(time(0));
    for (int cn,t,i=0;i<255;++i)
    {
        t=i;
        cn=0;
        while (t) cn+=(t&1),t>>=1;
        num[cn][++cnt[cn]]=i;
    }
}