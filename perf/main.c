#include "perf.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <stdint.h>

#define DECL(fn) void fn();

PROGRAMS(DECL)

int comp(const void*a,const void*b)//用来做比较的函数。
{
    return *(int*)a-*(int*)b;
}

void Init();
static void run(void (*func)(), int rounds);
static int gettime();
static void (*lookup(const char *fn))();

int main(int argc, char **argv) {
  // TODO: parse arguments: set @func and @rounds
  //ok?
  srand(time(0));
  Init();
  char *opt=argv[1];
  
  int rounds=1;
  if (argc>=3&&opt[0]=='-'&&opt[1]=='r')
  {
      if (argc==4)
      {
        sscanf(argv[2],"%d",&rounds);
        void (*func)()= lookup(argv[3]);
        run(func, rounds);
      }
      else
      {
        void (*func)()= lookup(argv[2]);
        run(func, rounds);
      }
      
  }

  
}

static int gettime() {
  // TODO: implement me!
  return clock();
}

static void (*lookup(const char *fn))() {
  #define PAIR(fn) { #fn, fn },
  struct pair {
    const char *name;
    void *ptr;
  };
  struct pair lut[] = {
    PROGRAMS(PAIR)
  };

  for (int i = 0; i < LENGTH(lut); i++) {
    struct pair *p = &lut[i];
    if (strcmp(fn, p->name) == 0) return p->ptr;
  }
  return print_error;
}

static void run(void (*func)(), int rounds) {
  int *elapsed = malloc(sizeof(int) * rounds);
  if (!elapsed) {
    perror("elapsed");
    return;
  }
  
  long long sum=0;
  int maxn=0,minn=2147483647;
  double aver,vari=0,stds;
  for (int round = 0; round < rounds; round++) {
    int st = gettime();
    func();
    int ed = gettime();
    elapsed[round] = ed - st;
    sum+=elapsed[round];
  }
  // TODO: display runtime statistics
  qsort(elapsed,rounds,sizeof(int),comp);
  maxn=elapsed[rounds-1];
  minn=elapsed[0];
  aver=sum*1.0/rounds;
  printf("sum:%lldus\n",sum);
  printf("maximum:%dus minimum:%dus\n",maxn,minn);
  printf("average:%.6lfus\n",aver);
  if (rounds&1) printf("median:%dus\n",elapsed[rounds/2]);
  else printf("median:%dus\n",(elapsed[rounds/2]+elapsed[rounds/2-1])/2);
  for (int round=0;round<rounds;round++)
    vari+=pow(aver-elapsed[round],2)/rounds;
  printf("variance:%.6lfus\nstandard:%.6lfus\n",vari,stds=sqrt(vari));
  double alpha=0,beta=0;//偏度与峰度
  for (int round=0;round<rounds;round++)
    alpha+=pow((aver-elapsed[round])/stds,3)/rounds,
    beta+=pow((aver-elapsed[round])/stds,4)/rounds;
  if (rounds>=100)
  {
    if (alpha>4) puts("Positive skew");
    else if (alpha<-4) puts("Negative skew");
    else puts("Normal distribution skew");
    if (beta>10) puts("Peak kurtosis");
    else if (beta<-10) puts("Flat kurtosis");
    else puts("Normal distribution kurtosis");
  }
  else puts("Too few datas to analyze the normal distribution");
  free(elapsed);
}
