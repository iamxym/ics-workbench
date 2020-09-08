#include "perf.h"
#include <stdio.h>
#include <stdlib.h>
#include "multimod.h"
int64_t multimod_p1(int64_t a, int64_t b, int64_t m);
int64_t multimod_p2(int64_t a, int64_t b, int64_t m);
int64_t multimod_p3(int64_t a, int64_t b, int64_t m);
int64_t bitget(int id);
#define Lar ((1LL<<62))
#define now_bit 7
void dummy() {
}
void print_hello() {
  printf("hello\n");
}

void print_error() {
  printf("invalid subject!\n");
  exit(1);
}

void simple_loop() {
  for (volatile int i = 0; i < 1000000; i++) ;
}

int64_t get(int64_t lim)
{
  int64_t ret=(rand()*RAND_MAX+rand())%lim;
  if (ret<0) ret+=lim;
  return ret;
}
void mul_p1()
{
  //int64_t a=get(Lar),b=get(Lar),m=get(Lar)+1;
  int64_t a=bitget(now_bit),b=bitget(now_bit),m=get(Lar)+1;
  //printf("%ld %ld %ld\n",a,b,m);
  multimod_p1(a,b,m);
}
void mul_p2()
{
  //int64_t a=get(Lar),b=get(Lar),m=get(Lar)+1;
  int64_t a=bitget(now_bit),b=bitget(now_bit),m=get(Lar)+1;
  //printf("%ld %ld %ld %d %d\n",a,b,m,judg(a),judg(b));
  multimod_p2(a,b,m);
}
void mul_p3()
{
  //int64_t a=get(Lar),b=get(Lar),m=get(Lar)+1;
  int64_t a=bitget(now_bit),b=bitget(now_bit),m=get(Lar)+1;
  multimod_p3(a,b,m);
}