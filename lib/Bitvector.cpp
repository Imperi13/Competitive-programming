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
  const u64 LBLOCK=512;
  const u64 SBLOCK=16;//uint16に対応しているから16以下

  std::vector<u64> l;//大ブロック
  std::vector<std::pair<u16,u16>> s;//小ブロックとそのビット列

  public:

  BitVector(u64 n_):n(n_),l(n_/LBLOCK + 1,0),s(n_/SBLOCK +1,std::make_pair(0,0)){}

  //pos番目(0-indexed)のbitを反転する
  void set(u64 pos){
    assert(0<=pos&&pos<n);
    s[pos/SBLOCK].second|=1llu<<(pos%SBLOCK);
  }

  //全部のbitをsetしたあと1回buildを呼ぶ
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

  //pos番目のbitを返す
  u64 access(u64 pos){
    assert(0<=pos&&pos<n);
    return (s[pos/SBLOCK].second>>(pos%SBLOCK))&1;
  }

  //[0,pos)の1の個数を返す
  u64 rank(u64 pos){
    assert(0<=pos&&pos<=n);
    return l[pos/LBLOCK]+s[pos/SBLOCK].first+popcount(s[pos/SBLOCK].second&((1llu<<(pos%SBLOCK))-1));
  }

  //pos個目(1-indexed)のbitの位置を0-indexedで返す
  u64 select(u64 bit,u64 pos){
    assert(bit==0||bit==1);
    if(bit==1){
      assert(0<pos&&pos<=bitcnt);
      u64 num=0,left=0,right=n/LBLOCK + 1;
      while(right-left>1){
        u64 mid=left+(right-left)/2;
        if(l[mid]>=pos)right=mid;
        else left=mid;
      }
      num+=l[left];
      left=left*LBLOCK/SBLOCK;right=std::min(left+LBLOCK/SBLOCK,n/SBLOCK+1);
      while(right-left>1){
        u64 mid=left+(right-left)/2;
        if(num+s[mid].first>=pos)right=mid;
        else left=mid;
      }
      num+=s[left].first;
      for(u64 i=1;i<SBLOCK+1;i++){
        if(num+popcount(s[left].second&((1llu<<i)-1))==pos)return left*SBLOCK+i-1;
      }
      return -1;
    }else{
      assert(0<pos&&pos<=(n-bitcnt));
      u64 num=0,left=0,right=n/LBLOCK + 1;
      while(right-left>1){
        u64 mid=left+(right-left)/2;
        if(mid*LBLOCK-l[mid]>=pos)right=mid;
        else left=mid;
      }
      num+=left*LBLOCK-l[left];
      left*=LBLOCK/SBLOCK;
      u64 left2=0,right2=std::min(LBLOCK/SBLOCK,n/SBLOCK+1-left*LBLOCK/SBLOCK);
      while(right2-left2>1){
        u64 mid=left2+(right2-left2)/2;
        if(num+mid*SBLOCK-s[left+mid].first>=pos)right2=mid;
        else left2=mid;
      }
      num+=left2*SBLOCK-s[left+left2].first;
      for(u64 i=1;i<SBLOCK+1;i++){
        if(num+i-popcount(s[left+left2].second&((1llu<<i)-1))==pos)return (left+left2)*SBLOCK+i-1;
      }
      return -1;
    }
  }
};