#include "multimod.h"
int64_t multimod_p1(int64_t a, int64_t b, int64_t m) {
  // TODO: implement
  int wa[140],wb[140],wc[140],la,lb,lc;//10^38 len=39
  for (int i=0;i<128;++i) wa[i]=wb[i]=wc[i]=0;
  la=lb=lc=0;
  int64_t x=a;
  do
  {
	wa[la++]=(x&1);
	x>>=1;
  }while (x);
  x=b;
  do
  {
	  wb[lb++]=(x&1);
	  x>>=1;
  }while (x);
  for (int i=0;i<la;++i)
	  for (int j=0;j<lb;++j)
		  wc[i+j]+=(wa[i]&wb[j]);
  for (int i=0;i<la+lb;++i) wc[i+1]+=(wc[i]/2),wc[i]&=1;
  lc=la+lb;
  while (lc>=2&&wc[lc-1]==0) --lc;
  uint64_t ans=0;
  for (int i=lc-1;i>=0;--i) ans=(ans*2+wc[i])%m;
  return (int64_t)ans;
}
