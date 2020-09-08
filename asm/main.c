#include "asm.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

static void setjmp_test0();
static void setjmp_test1();

int main() {
  // TODO: add more tests here!
  assert(asm_add(1, 2) == 3);
  assert(asm_add(-34, -21) == -55);
  assert(asm_add(-2147483648, 2147483647) == -1);
  assert(asm_add(0xffffffffffffffff,0x0fffffffffffffff)==(int64_t)0xffffffffffffffff+(int64_t)0x0fffffffffffffff);
  assert(asm_popcnt(0xffffffffffffffffull) == 64);
  assert(asm_popcnt(0)==0);assert(asm_popcnt(1)==1);
  assert(asm_popcnt(0x1000000000000000ull)==1);
  assert(asm_popcnt(0x0fffffff0fffffffull) == 56);
  assert(asm_popcnt(0xffffffffff9fffffull) == 62);
  // TODO: add memcpy tests here!
/*  assert(asm_memcpy(NULL, NULL, 0) == NULL);
  char aa[15]="0123456789",bb[15]="NO!NO!NO!";
  asm_memcpy(bb,aa,1);
  printf("%s\n%s\n",aa,bb);
  asm_memcpy(aa,bb,9);
  printf("%s\n%s\n",aa,bb);
  asm_memcpy(bb,aa,10);
  printf("%s\n%s\n",aa,bb);
  int cc[13]={0xff,0xf0,0x0f,0x00,233},dd[14]={0};
  asm_memcpy(dd,cc,16);
  for (int i=0;i<5;++i) printf("%d%c",cc[i]," \n"[i==4]);
  for (int i=0;i<5;++i) printf("%d%c",dd[i]," \n"[i==4]);*/
  // setjmp test starts here
  setjmp_test0();
  setjmp_test1();
}

struct log_entry {
  const char *str;
  int value;
};
static asm_jmp_buf bufferA, bufferB;
static struct log_entry expected[] = {
  // expected output sequence for setjmp tests
  { "A", 0 },
  { "B", 999 },
  { "A1", 0 },
  { "B1", 0 },
  { "A2", 10001 },
  { "B2", 20001 },
  { "A3", 10002 },
  { "B3", 20002 },
  { "A4", 10003 },
  { NULL, 0 },
};
static struct log_entry *head = expected;

static void setjmp_log(const char *str, int value) {
  assert(head->str != NULL);
  assert(strcmp(head->str, str) == 0);
  assert(head->value == value);
  head++;

}

static void setjmp_test2();
static void setjmp_test1() {
  int r = 0;
  setjmp_log("A1", r);
  if ((r = asm_setjmp(bufferA)) == 0) {
    setjmp_test2();
  }
  setjmp_log("A2", r);
  if ((r = asm_setjmp(bufferA)) == 0) {
    asm_longjmp(bufferB, 20001);
  }
  setjmp_log("A3", r);
  if ((r = asm_setjmp(bufferA)) == 0) {
    asm_longjmp(bufferB, 20002);
  }
  setjmp_log("A4", r);
  assert(head->str == NULL);
}

static void setjmp_test2() {
  int r = 0;
  setjmp_log("B1", r);
  if ((r = asm_setjmp(bufferB)) == 0) {
    asm_longjmp(bufferA, 10001);
  }
  setjmp_log("B2", r);
  if ((r = asm_setjmp(bufferB)) == 0) {
    asm_longjmp(bufferA, 10002);
  }
  setjmp_log("B3", r);
  if ((r = asm_setjmp(bufferB)) == 0) {
    asm_longjmp(bufferA, 10003);
  }
}

static asm_jmp_buf buf;

static void setjmp_test_simple() {
  
  asm_longjmp(buf, 999);
}

static void setjmp_test0() {
  int r;
  if ((r = asm_setjmp(buf)) == 0) {
    setjmp_log("A", r);
    setjmp_test_simple();
  }
  setjmp_log("B", r);
}
