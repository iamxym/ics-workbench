//0 10000 1576854509
#include "common.h"
#include <inttypes.h>
//#include <sys/time.h>
#include <time.h>
#include <unistd.h>
void mem_read(uintptr_t block_num, uint8_t *buf);
void mem_write(uintptr_t block_num, const uint8_t *buf);
struct timespec time_now[2]={{0,0},{0,0}};
int64_t read_time=0,write_time=0;
int64_t cache_rdtime=0,cache_wrtime=0,mem_rdtime=0,mem_wrtime=0;
//void mem_uncache_write(uintptr_t addr, uint32_t data, uint32_t wmask);
typedef struct {
  uint8_t data[64];//一块cache为64B
  uint32_t tag;
  bool valid;//1合法，0非法
  bool dirty;//1修改，0未修改
}cache_block;
cache_block Cache[64][4];//4路组相连,4*64*64B=16KB
//内存共2^14块，而cache有64组，因此20位地址中，中间6位为组号，
//低6位为块内地址，高8位为标记tag
static uint64_t cycle_cnt = 0;
static uint64_t tot_cnt=0;
static uint64_t hit_cnt=0;
void cycle_increase(int n) { cycle_cnt += n; }
void tot_increase(int n){tot_cnt+=n;}
void hit_increase(int n){hit_cnt+=n;}
// TODO: implement the following functions
void write_it(uintptr_t grp_id,int i,uintptr_t block_addr,uint32_t data,uint32_t wmask)
{
  uint32_t *p = (void *)(Cache[grp_id][i].data) + (block_addr & ~0x3);
  *p = (*p & ~wmask) | (data & wmask);
}

uint32_t cache_read(uintptr_t addr) {
  clock_gettime(CLOCK_REALTIME, &time_now[0]);  
  //time_begin = time_now.tv_sec*1000*1000*1000 + time_now.tv_nsec;
  //if (tot_cnt<=5)printf("CLOCK_REALTIME: %ld, %ld\n", time_now.tv_sec, time_now.tv_nsec);  
  tot_increase(1);
  uintptr_t block_addr=(addr&63),//取低6位
            grp_id=((addr>>6)&63),//取中间6位
            tag=(addr>>12);//取高8位
  bool is_hit=false;
  uint32_t *ret;
  for (int i=0;i<4;++i)
  if (Cache[grp_id][i].tag==tag&&Cache[grp_id][i].valid==true)
  {
    //命中
    is_hit=true;hit_increase(1);
    //ret=Cache[grp_id][i].data[block_addr];
    ret = (void *)(Cache[grp_id][i].data) + (block_addr & ~0x3);
    clock_gettime(CLOCK_REALTIME, &time_now[1]);
    cache_rdtime+=(time_now[1].tv_sec-time_now[0].tv_sec)*1000000000+(time_now[1].tv_nsec-time_now[0].tv_nsec);
    break;
    
  }
  if (is_hit==false)
  { 
    //uint8_t tmp[64];
    //寻找是否有空行
    bool has_empty=false;
    for (int i=0;i<4;++i)
      if (Cache[grp_id][i].valid==false)
      {
        has_empty=true;
        mem_read(addr>>6,Cache[grp_id][i].data);
        ret = (void *)(Cache[grp_id][i].data) + (block_addr & ~0x3);
        Cache[grp_id][i].tag=tag;
        Cache[grp_id][i].valid=true;
        Cache[grp_id][i].dirty=false;
        break;
      }
    if (has_empty==false)
    {
      //随机替换
      int repl=rand()%4;
      if (Cache[grp_id][repl].dirty==true)//需要写回
        mem_write((Cache[grp_id][repl].tag<<6)|grp_id,Cache[grp_id][repl].data);
      mem_read(addr>>6,Cache[grp_id][repl].data);
      ret = (void *)(Cache[grp_id][repl].data) + (block_addr & ~0x3);
      Cache[grp_id][repl].tag=tag;
      Cache[grp_id][repl].valid=true;
      Cache[grp_id][repl].dirty=false;
    }
  }
  
  if (is_hit==false)
  {
    clock_gettime(CLOCK_REALTIME, &time_now[1]);  
    read_time+=(time_now[1].tv_sec-time_now[0].tv_sec)*1000000000+(time_now[1].tv_nsec-time_now[0].tv_nsec);
  }
  return *ret;
}

void cache_write(uintptr_t addr, uint32_t data, uint32_t wmask) {
  clock_gettime(CLOCK_REALTIME, &time_now[0]); 
  tot_increase(1);
  uintptr_t block_addr=(addr&63),//取低6位
            grp_id=((addr>>6)&63),//取中间6位
            tag=(addr>>12);
  bool is_hit=false;
  for (int i=0;i<4;++i)
    if (Cache[grp_id][i].tag==tag&&Cache[grp_id][i].valid==true)
    {
      is_hit=true;
      write_it(grp_id,i,block_addr,data,wmask);
      Cache[grp_id][i].dirty=true;//写入cache但还没写入内存
      clock_gettime(CLOCK_REALTIME, &time_now[1]);
      cache_wrtime+=(time_now[1].tv_sec-time_now[0].tv_sec)*1000000000+(time_now[1].tv_nsec-time_now[0].tv_nsec);
      break;
    }
  if (is_hit==false)
  {
    //寻找空行
    bool is_empty=false;
    for (int i=0;i<4;++i)
      if (Cache[grp_id][i].valid==false)
      {
        is_empty=true;hit_increase(1);
        //mem_uncache_write(addr,data,wmask);//先在内存中修改
        mem_read(addr>>6,Cache[grp_id][i].data);//再从内存读入cache
        write_it(grp_id,i,block_addr,data,wmask);//将data写入cache
        Cache[grp_id][i].tag=tag;
        Cache[grp_id][i].valid=true;
        Cache[grp_id][i].dirty=false;
        mem_write(addr>>6,Cache[grp_id][i].data);//在cache中修改完成之后写回内存
        break;
      }
    if (is_empty==false)
    {
      //随机替换
      int repl=rand()%4;
      if (Cache[grp_id][repl].dirty==true)//需要写回
        mem_write((Cache[grp_id][repl].tag<<6)|grp_id,Cache[grp_id][repl].data);
      //mem_uncache_write(addr,data,wmask);
      mem_read(addr>>6,Cache[grp_id][repl].data);
      write_it(grp_id,repl,block_addr,data,wmask);
      Cache[grp_id][repl].tag=tag;
      Cache[grp_id][repl].valid=true;
      Cache[grp_id][repl].dirty=false;
      mem_write(addr>>6,Cache[grp_id][repl].data);
    }
  }
  if (is_hit==false)
  {
    clock_gettime(CLOCK_REALTIME, &time_now[1]);  
    write_time+=(time_now[1].tv_sec-time_now[0].tv_sec)*1000000000+(time_now[1].tv_nsec-time_now[0].tv_nsec);
  }
}

void init_cache(int total_size_width, int associativity_width) {
  int grp_tot=(1<<associativity_width),//组数
      block_tot=(1<<total_size_width)/BLOCK_SIZE/grp_tot;//块数
  //printf("%d %d\n",grp_tot,block_tot);
  for (int i=0;i<block_tot;++i)
  for (int j=0;j<grp_tot;++j)
    {
      Cache[i][j].dirty=false;
      Cache[i][j].valid=false;
    }
}

void display_statistic(void) {
  puts("Statistic:");
  printf("cycle:%ld\n",cycle_cnt);
  printf("total:%ld\n",tot_cnt);
  printf("hit:%ld\n",hit_cnt);
  printf("miss:%ld\n",tot_cnt-hit_cnt);
  printf("hit rate=%.6lf\n",hit_cnt*1.0/tot_cnt);
  printf("cache miss read averge time:%.6lf\n",read_time*1.0/(tot_cnt-hit_cnt));
  printf("cache miss write averge time:%.6lf\n",write_time*1.0/(tot_cnt-hit_cnt));
  printf("cache hit read averge time:%.6lf\n",cache_rdtime*1.0/hit_cnt);
  printf("cache hit write averge time:%.6lf\n",cache_wrtime*1.0/hit_cnt);
}
