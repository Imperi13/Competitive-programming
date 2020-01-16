#include <cassert>
#include <algorithm>
#include <bitset>
#include <cctype>
#include <cmath>
#include <complex>
#include <chrono>
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

//WIP:access,rank,selectまでは実装した
//    数値を扱う型と添え字を扱う型分けた方が良くない???

//http://miti-7.hatenablog.com/entry/2018/04/28/152259を参考にしている

//数列の要素は [0,1<<BITLEN) に収まっている必要がある (0<BITLEN<=64)
//無効値をtemplate引数に渡す
template<uint64_t BITLEN,uint64_t NONE>
class WaveletMatrix{

  private:

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
        if(pos==0)return 0;
        if(pos<0||bitcnt<pos)return NONE;

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
        if(pos==0)return 0;
        if(pos<0||n-bitcnt<pos)return NONE;

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

    void test_out(){
      for(int i=n/SBLOCK;i>=0;i--)std::cout<<std::bitset<16>(s[i].second);
      std::cout<<std::endl;
    }
  };

  using u64=std::uint64_t;

  u64 n;//数列の要素数
  std::vector<BitVector> bitvec;//bitvec[i]が2^iのbitを管理する
  std::vector<u64> zerocnt;//zerocnt[i]が2^iのbitのゼロ個数を管理する

  std::map<u64,std::pair<u64,u64>> index;//index[num] = {numの始点index,個数}

  //[first,last)の区間中にnum未満の値がいくつあるか返す
  u64 less(u64 first,u64 last,u64 num){
    assert(0<=first&&first<=last&&last<=n);

    u64 ret=0;
    for(int bit=BITLEN-1;bit>=0;bit--){
      if((num>>bit)&1){
        ret+=(last-bitvec[bit].rank(last))-(first-bitvec[bit].rank(first));
        first=zerocnt[bit]+bitvec[bit].rank(first);
        last=zerocnt[bit]+bitvec[bit].rank(last);
      }else{
        first-=bitvec[bit].rank(first);last-=bitvec[bit].rank(last);
      }
    }
    return ret;
  }

  public:

  //暫定的にuint64の配列しか受け取らないようにしている 構築 O(BITLEN*N)
  WaveletMatrix(const std::vector<u64>& seq):n(seq.size()),bitvec(BITLEN,seq.size()),zerocnt(BITLEN){
    std::vector<u64> temp=seq;
    for(int bit=BITLEN-1;bit>=0;bit--){
      std::vector<u64> zero,one;
      zero.reserve(n);
      for(int j=0;j<n;j++){
        if((temp[j]>>bit)&1){
          bitvec[bit].set(j);
          one.push_back(temp[j]);
        }else{
          zero.push_back(temp[j]);
        }
      }
      bitvec[bit].build();
      std::copy(one.begin(),one.end(),std::back_inserter(zero));
      temp=zero;
      zerocnt[bit]=n-bitvec[bit].rank(n);
    }
    index[temp[0]]={0,0};
    u64 num=temp[0];
    for(int i=0;i<n;i++){
      if(temp[i]!=num){
        index[temp[i]]={i,1};
        num=temp[i];
      }else{
        index[num].second++;
      }
    }
  }

  //a_pos(0-indexed)を返す O(BITLEN)
  u64 access(u64 pos){
    assert(0<=pos&&pos<n);
    u64 num=0;
    for(int bit=BITLEN-1;bit>=0;bit--){
      if(bitvec[bit].access(pos)){
        num+=(1llu<<bit);
        pos=zerocnt[bit]+bitvec[bit].rank(pos);
      }else{
        pos=pos-bitvec[bit].rank(pos);
      }
    }
    return num;
  }

  //[0,pos)にnumが何個あるかを返す O(BITLEN)
  u64 rank(u64 num,u64 pos){
    assert(0<=pos&&pos<=n);
    if(index.find(num)==index.end())return 0;//indexを無駄に作らないように 計算量はO(BITLEN)のまま
    for(int bit=BITLEN-1;bit>=0;bit--){
      if((num>>bit)&1)pos=zerocnt[bit]+bitvec[bit].rank(pos);
      else pos=pos-bitvec[bit].rank(pos);
    }
    if(pos<=index[num].first)return 0;
    else return std::min(pos-index[num].first,index[num].second);
  }

  //[0,k)にnumがちょうどpos個(1-indexed)入るような最小のkを返す O(BITLEN*loglogN)
  u64 select(u64 num,u64 pos){
    if(index.find(num)==index.end())return NONE;//数列に含まれてない値を除外
    if(pos<=0||index[num].second<pos)return NONE;//posがその値の個数より大きいときを除外

    u64 itr=index[num].first+pos;
    for(int bit=0;bit<BITLEN;bit++){
      if(zerocnt[bit]<itr)itr=bitvec[bit].select(1,itr-zerocnt[bit]);
      else itr=bitvec[bit].select(0,itr);
    }
    return itr;
  }

  //[first,last)の中でpos番目に小さい数字を返すO(BITLEN)
  u64 quantile(u64 first,u64 last,u64 pos){
    assert(0<=first&&first<last&&last<=n);
    assert(0<pos&&pos<=last-first);

    u64 ret=0;
    for(int bit=BITLEN-1;bit>=0;bit--){
      u64 zero=last-bitvec[bit].rank(last)-first+bitvec[bit].rank(first);
      if(pos<=zero){
        first=first-bitvec[bit].rank(first);last=first+zero;
      }else{
        ret+=(1llu<<bit);
        pos-=zero;
        u64 one=last-first-zero;
        first=zerocnt[bit]+bitvec[bit].rank(first);last=first+one;
      }
    }
    return ret;
  }

  //[first,last)区間内に[a,b)の値がいくつあるか返す
  u64 rangefreq(u64 first,u64 last,u64 a,u64 b){
    assert(a<=b);
    return less(first,last,b)-less(first,last,a);
  }

  void test(){
    for(int i=BITLEN-1;i>=0;i--){
      debug(i);
      bitvec[i].test_out();
      debug(zerocnt[i]);
    }

    for(auto e:index){
      debug(e.first,e.second);
    }
  }

};