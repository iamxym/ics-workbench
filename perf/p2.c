#include "multimod.h"

int64_t multimod_p2(int64_t a, int64_t b, int64_t m) {
  // TODO: implement
  uint64_t ta=a,tb=b,ans=0,tm=m;
  for (;tb;tb>>=1,ta=(ta+ta)%tm)
	  if (tb&1) ans=(ans+ta)%tm;
  return (int64_t)ans;
//  return -2;
}
