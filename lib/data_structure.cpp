#include <assert.h>
#include <math.h>
#include <algorithm>
#include <bitset>
#include <cctype>
#include <complex>
#include <deque>
#include <iomanip>
#include <iostream>
#include<functional>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <vector>

using ll = long long;

//非再帰抽象セグ木 0-indexed 単位元はiで指定
template <class T>
class SegmentTree {
 private:
  long long n;
  T init;
  std::vector<T> dat;
  std::function<T(T, T)> fn;

 public:
  SegmentTree(long long i, T para, std::function<T(T, T)> fun)
      : init(para), fn(fun) {
    n = 1;
    while (n < i) {
      n *= 2;
    }
    dat = std::vector<T>(2 * n - 1, init);
  }

  SegmentTree(const std::vector<T>& seq,T para,std::function<T(T,T)> fun)
      : init(para),fn(fun){
    n=1;
    while(n<seq.size())n<<=1;
    dat=std::vector<T>(2*n-1,init);
    for(long long i=0;i<seq.size();i++)dat[i+n-1]=seq[i];
    for(long long i=n-2;i>=0;i--)dat[i]=fn(dat[i*2+1],dat[i*2+2]);
  }

  // k番目(0-indexed)を値aで更新,dest=trueのときは更新前を破壊して初期化する
  void update(long long k, T a, bool dest) {
    assert(0<=k&&k<n);
    k += n - 1;
    if (dest) dat[k] = a;
    else dat[k] = fn(dat[k], a);

    while (k > 0) {
      k = (k - 1) / 2;
      dat[k] = fn(dat[k * 2 + 1], dat[k * 2 + 2]);
    }
  }

  //[a,b)の値を返す
  T query(long long a, long long b){
    assert(0<=a&&b<=n);
    T val=init;

    long long l=a+n-1,r=b+n-1;
    for(;l<r;l=(l>>1),r=(r>>1)){
      if(!(r&1)){
        r--;
        val=fn(val,dat[r]);
      }
      if(!(l&1)){
        val=fn(val,dat[l]);
        l++;
      }
    }

    return val;
  }
};

//とりあえず1-indexedでつくったBIT
template <class T>
class BinaryIndexedTree {
  private:
  T n;
  std::vector<T> bit;

  public:
  BinaryIndexedTree(T i) : n(i), bit(i + 1, 0){};

  // a番目にwを加算
  void add(T a, T w) {
    for (T x = a; x <= n; x += (x & -x)) {
      bit[x] += w;
    }
  }

  //[1,a]の和
  T sum(T a) {
    T ret = 0;
    for (T x = a; x > 0; x -= (x & -x)) {
      ret += bit[x];
    }
    return ret;
  }

    //各項がすべて非負のものに対して、sum[1,a]>=keyを満たす最小のaを返す 存在しないなら-1返す
  T lower_bound(T key){
    T len=1,temp=n,now=0,value=0,flag;
    while(temp/2!=0){
      len*=2;
      temp/=2;
    }

    while(len>0){
      while(now+len>n)len/=2;

      if(value+bit[now+len]>=key){
        len/=2;
      }else{
        now+=len;
        value+=bit[now];
        len/=2;
        if(now==n){
          return -1;
        }
      }
    }

    now++;
    return now;
  }
};

// UnionFind
class UnionFind {
  private:
  std::vector<long long> uni;
  long long num;

  public:
  UnionFind(long long s) : uni(s, -1), num(s) {}

  //頂点aが所属するグループ
  long long root(long long a) {
    if (uni[a] < 0) {
      return a;
    }
    return uni[a] = root(uni[a]);
  }

  // a,bをつなげる
  bool connect(long long a, long long b) {
    a = root(a);
    b = root(b);
    if (a == b) {
      return false;
    }

    if (uni[a] > uni[b]) {
      std::swap(a, b);
    }

    uni[a] += uni[b];
    uni[b] = a;
    return true;
  }

  // a,bが同グループか確認
  bool isConnect(long long a, long long b) { return root(a) == root(b); }

  // aが属するグループの要素数
  long long size(long long a) { return -uni[root(a)]; }

  //グループ数を返す
  long long groups() {
    std::set<long long> b;
    for (long long i = 0; i < num; i++) {
      b.insert(root(i));
    }
    return b.size();
  }
};