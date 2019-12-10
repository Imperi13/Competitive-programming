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