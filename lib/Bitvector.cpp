#include <cassert>
#include <algorithm>
#include <bitset>
#include <cctype>
#include <cmath>
#include <complex>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
#include <random>
#include <memory>
#include <utility>
#include <limits>

//http://miti-7.hatenablog.com/entry/2018/04/15/155638 
//https://takeda25.hatenablog.jp/entry/20140201/1391250137 を参考にした
class BitVector{
  using u64=std::uint64_t;
  using u16=std::uint16_t;

  private:

  static u64 popcount(u64 x){
    x = (x & 0x5555555555555555ULL) + ((x & 0xAAAAAAAAAAAAAAAAULL)>>1);
    x = (x & 0x3333333333333333ULL) + ((x & 0xCCCCCCCCCCCCCCCCULL)>>2);
    x = (x & 0x0F0F0F0F0F0F0F0FULL) + ((x & 0xF0F0F0F0F0F0F0F0ULL)>>4);
    x *= 0x0101010101010101ULL;
    return x >> 56;
  }

  u64 n;//bitvector全体のサイズ
  u64 bitcnt;//全体の1の個数
  const u64 LBLOCK=512;//SBLOCKの倍数にする
  const u64 SBLOCK=16;//16より大きくするならuint16をuint32とかに変えてね

  std::vector<u64> l;//大ブロック
  std::vector<std::pair<u16,u16>> s;//小ブロックとそのビット列

  public:

  BitVector(u64 n_):n(n_),l(n_/LBLOCK + 1,0),s(n_/SBLOCK +1,std::make_pair(0,0)){}

  //pos番目(0-indexed)のbitを反転する
  void set(u64 pos){
    assert(0<=pos&&pos<n);
    s[pos/SBLOCK].second|=1llu<<(pos%SBLOCK);
  }

  void build(){
    u64 num=0;
    for(u64 i=0;i<=n;i++){
      if(i%LBLOCK==0)l[i/LBLOCK]=num;
      if(i%SBLOCK==0)s[i/SBLOCK].first=num-l[i/LBLOCK];
      if(i!=n&&i%SBLOCK==0){
        num+=popcount(s[i/SBLOCK].second);
      }
    }
    bitcnt=num;
  }

  u64 access(u64 pos){
    assert(0<=pos&&pos<n);
    return (s[pos/SBLOCK].second>>(pos%SBLOCK))&1;
  }

  //[0,pos)の1の個数を返す
  u64 rank(u64 pos){
    assert(0<=pos&&pos<=n);
    return l[pos/LBLOCK]+s[pos/SBLOCK].first+popcount(s[pos/SBLOCK].second&((1llu<<(pos%SBLOCK))-1));
  }

  //[0,k)にbitがちょうどpos個(1-indexed)入るような最小のkを返す
  //pos個ないならuint64::max()を返す
  u64 select(u64 bit,u64 pos){
    assert(bit==0||bit==1);
    if(bit==1){
      if(pos<=0||bitcnt<pos)return std::numeric_limits<u64>::max();

      u64 num=0,l1=0,r1=n/LBLOCK+1;
      while(r1-l1>1){
        u64 mid=l1+(r1-l1)/2;
        if(l[mid]>=pos)r1=mid;
        else l1=mid;
      }
      num+=l[l1];
      u64 l2=l1*LBLOCK/SBLOCK,r2=std::min(l2+LBLOCK/SBLOCK,n/SBLOCK+1);
      while(r2-l2>1){
        u64 mid=l2+(r2-l2)/2;
        if(num+s[mid].first>=pos)r2=mid;
        else l2=mid;
      }
      num+=s[l2].first;
      u64 l3=0,r3=SBLOCK;
      while(r3-l3>1){
        u64 mid=l3+(r3-l3)/2;
        if(num+popcount(s[l2].second&((1llu<<mid)-1))>=pos)r3=mid;
        else l3=mid;
      }
      return l2*SBLOCK+l3+1;
    }else{
      if(pos<=0||n-bitcnt<pos)return std::numeric_limits<u64>::max();

      u64 num=0,l1=0,r1=n/LBLOCK+1;
      while(r1-l1>1){
        u64 mid=l1+(r1-l1)/2;
        if(mid*LBLOCK-l[mid]>=pos)r1=mid;
        else l1=mid;
      }
      num+=l1*LBLOCK-l[l1];
      u64 l2=l1*LBLOCK/SBLOCK,r2=std::min(l2+LBLOCK/SBLOCK,n/SBLOCK+1);
      while(r2-l2>1){
        u64 mid=l2+(r2-l2)/2;
        if(num+(l2*SBLOCK-l1*LBLOCK)-s[mid].first>=pos)r2=mid;
        else l2=mid;
      }
      num+=(l2*SBLOCK-l1*LBLOCK)-s[l2].first;
      u64 l3=0,r3=SBLOCK;
      while(r3-l3>1){
        u64 mid=l3+(r3-l3)/2;
        if(num+mid-popcount(s[l2].second&((1llu<<mid)-1))>=pos)r3=mid;
        else l3=mid;
      }
      return l2*SBLOCK+l3+1;
    }
  }
};