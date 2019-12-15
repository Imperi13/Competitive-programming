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
#include "limits.h"

//verifyしてない O(log^2 N)

//2Dセグ木 x,y,id,可換演算fを渡す
template <class T>
class SegmentTree {
 private:

  //動的セグ木 
  class DynamicSegmentTree{
    private:

    using F=std::function<T(T,T)>;

    long long n,n0;
    F fn;
    T init;

    class Node{
      public:
      std::shared_ptr<Node> left,right;
      std::weak_ptr<Node> par;
      T value;

      Node(T i,const std::shared_ptr<Node>& par_):value(i),left(),right(){
        par=par_;
      }

      Node(T i):value(i),left(),right(),par(){}
    };

    std::shared_ptr<Node> root;

    T query(long long a,long long b,const std::shared_ptr<Node>& now,long long l,long long r){
      if(a<=l&&r<=b){
        return now->value;
      }
      if(b<=l||r<=a){
        return init;
      }

      T lval=(now->left)?query(a,b,now->left,l,l+(r-l)/2):init;
      T rval=(now->right)?query(a,b,now->right,l+(r-l)/2,r):init;

      return fn(lval,rval);
    }

    public:

    //要素数の最大値n,単位元i,演算fを渡す
    DynamicSegmentTree(long long n_,T i,F f):n(n_),init(i),fn(f),root(new Node(init)){
      n0=1;
      while(n0<n_)n0<<=1;
    }

    // 更新 Ai=fn(Ai,val) を行う,dest=trueのときは初期化
    void update(long long i,T val,bool dest){
      assert(0<=i&&i<n);
      std::shared_ptr<Node> now(root);

      long long l=0,r=n0;
      while(r-l>1){
        long long mid=l+(r-l)/2;

        if(i<mid){
          if(!now->left){
            now->left=std::make_shared<Node>(init,now);
          }

          now=now->left;

          r=mid;
        }else{
          if(!now->right){
            now->right=std::make_shared<Node>(init,now);
          }

          now=now->right;

          l=mid;
        }
      }

      if(dest)now->value=val;
      else now->value=fn(now->value,val);

      while(now->par.lock()){
        now=now->par.lock();
        now->value=fn((now->left)?now->left->value:init,(now->right)?now->right->value:init);
      }
    }

    //[a,b)の区間演算結果を返す
    T query(long long a,long long b){
      assert(0<=a&&b<=n);
      return query(a,b,root,0,n0);
    }
  };

  using F=std::function<T(T,T)>;

  long long n;
  T init;
  std::vector<DynamicSegmentTree> dat;
  F fn;

 public:
  SegmentTree(long long x,long long y, T para, F fun)
      : init(para), fn(fun) {
    n = 1;
    while (n < x) {
      n *= 2;
    }
    for(ll i=0;i<2*n-1;i++)dat.emplace_back(DynamicSegmentTree(y,para,fun));
  }

  // k番目(0-indexed)を値aで更新,dest=trueのときは更新前を破壊して初期化する
  void update(long long k,long long y, T a) {
    assert(0<=k&&k<n);
    k += n - 1;
    dat[k].update(y,a,false);

    while (k > 0) {
      k = (k - 1) / 2;
      dat[k].update(y,a,false);
    }
  }

  //[a,b)の値を返す
  T query(long long xa, long long xb,long long ya,long long yb){
    assert(0<=xa&&xb<=n);
    T rval=init,lval=init;

    long long l=xa+n-1,r=xb+n-1;
    for(;l<r;l=(l>>1),r=(r>>1)){
      if(!(r&1)){
        r--;
        rval=fn(rval,dat[r].query(ya,yb));
      }
      if(!(l&1)){
        lval=fn(lval,dat[l].query(ya,yb));
        l++;
      }
    }

    return fn(lval,rval);
  }
};