#include "asm.h"
#include <string.h>

int64_t asm_add(int64_t a, int64_t b) {
  asm (
        "addq %%rbx,%%rax\n\t"
        :"+a"(a):"b"(b)
      );
  return a;
}

int asm_popcnt(uint64_t n) {
  int s;
  asm (
        "movl $0,%%eax;"
        "movl $0,%%edi;"
        "st:cmpl $0x40,%%edi;"
        "jge end;"//i>63 结束
        "movq %%rbx,%%rcx;"
        "and $1,%%rcx;"
        "add %%ecx,%%eax;"
        "shr $1,%%rbx;"
        "add $1,%%edi;"
        "jmp st;"
        "end:"
        :"=a"(s)
        :"b"(n)
        :"%edi","%rcx","cc"
      );
  return s;
}
void *asm_memcpy(void *dest, const void *src, size_t n) {
  // TODO: implement
  asm volatile (
    "mov $0,%%edi;"//%edi=i
    "sst:cmpl %%edx,%%edi;"
    "jge eend;"//i>=n 结束
    "movzbl (%%rcx),%%eax;"
    "mov %%al,(%%rbx);"
    "add $1,%%rbx;"
    "add $1,%%rcx;"
    "add $1,%%edi;"
    "jmp sst;"
    "eend:"
    :
    :"b"(dest),"c"(src),"d"(n)
    :"%edi","%eax","memory","cc"
  );
  return dest;
}
int asm_setjmp(asm_jmp_buf env) {
  asm volatile(
      ".globl setjmp;"
      "setjmp:;"
      "push %%rbp;"
      "mov %%rsp,%%rbp;"
      "mov %%rdi,%%rax;"//x86-64用寄存器传递参数，rdi就是env
      "mov %%rbx,(%%rax);"//env[0]=rbx;
      "mov %%rcx,8(%%rax);"//env[1]=rcx;
      "mov %%rdx,16(%%rax);"//env[2]=rdx;
      "mov %%rsi,24(%%rax);"//env[3]=rsi;
      "mov (%%rbp),%%rdi;"
      "mov %%rdi,32(%%rax);"//env[4]=rbp;
      "lea 16(%%rbp),%%rdi;"
      "mov %%rdi,40(%%rax);"//env[5]=rsp;
      "mov 8(%%rbp),%%rdi;"
      "mov %%rdi,48(%%rax);"//env[6]=pc;
      "xor %%rax,%%rax;"
      "pop %%rbp;"
      "ret;"
      :
      :
      :
  );
  return 0;
}

void asm_longjmp(asm_jmp_buf env, int val) {
  // TODO: implement
  asm volatile(
      ".globl longjmp;"
      "longjmp:;"
      "mov %%esi,%%eax;"
      "test %%eax,%%eax;"
		/*
		"cmpl $0,%%eax;"
        "jne vp;"
		*/
      "jnz vp;"
      "inc %%eax;"
      "vp: mov (%%rdi),%%rbx;"
      "mov 8(%%rdi),%%rcx;"
      "mov 16(%%rdi),%%rdx;"
      "mov 24(%%rdi),%%rsi;"
      "mov 32(%%rdi),%%rbp;"
      "mov 40(%%rdi),%%rsp;"
      "jmp *48(%%rdi);"
      :
      :
      :
  );

}
