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
//空間O(Q*(logN)^2) 

//2Dセグ木 x,y,id,可換演算fを渡す
template<typename T,typename F>
class SegmentTree2D{
  private:

  class DynamicSegmentTree{
    private:

    long long n,n0;
    F f;
    T id;

    struct Node{
      std::shared_ptr<Node> left,right;
      T value;

      Node(T i):value(i),left(),right(){}
    };

    std::shared_ptr<Node> root;

    T query(long long a,long long b,const std::shared_ptr<Node>& now,long long l,long long r){
      if(a<=l&&r<=b)return now->value;
      if(b<=l||r<=a)return id;

      T lval=(now->left)?query(a,b,now->left,l,l+(r-l)/2):id;
      T rval=(now->right)?query(a,b,now->right,l+(r-l)/2,r):id;

      return f(lval,rval);
    }

    public:

    //要素数の最大値n,単位元i,演算fを渡す
    DynamicSegmentTree(long long n_,T i,F f):n(n_),id(i),f(f),root(new Node(id)){
      n0=1;
      while(n0<n_)n0<<=1;
    }

    // 更新 Ai=fn(Ai,val) を行う
    void update(long long i,T val){
      assert(0<=i&&i<n);
      std::shared_ptr<Node> now(root);

      long long l=0,r=n0;
      while(r-l>1){
        now->value=f(now->value,val);
        long long mid=l+(r-l)/2;

        if(i<mid){
          if(!now->left)now->left=std::make_shared<Node>(id);
          now=now->left;r=mid;
        }else{
          if(!now->right)now->right=std::make_shared<Node>(id);
          now=now->right;l=mid;
        }
      }
      now->value=f(now->value,val);
    }

    //[a,b)の区間演算結果を返す
    T query(long long a,long long b){
      assert(0<=a&&b<=n);
      return query(a,b,root,0,n0);
    }
  };

  long long x,x0,y;
  F f;
  T id;

  struct Node_seq;
  using node_ptr=std::shared_ptr<Node_seq>;

  struct Node_seq{
    node_ptr left,right;
    DynamicSegmentTree seq;

    Node_seq(long long y,T i,F f):seq(y,i,f),left(),right(){}
  };

  node_ptr root;

  T query(long long a,long long b,long long d,long long u,node_ptr& now,long long l,long long r){
    if(a<=l&&r<=b)return now->seq.query(d,u);
    if(b<=l||r<=a)return id;

    T lval=(now->left)?query(a,b,d,u,now->left,l,l+(r-l)/2):id;
    T rval=(now->right)?query(a,b,d,u,now->right,l+(r-l)/2,r):id;

    return f(lval,rval);
  }

  public:

  SegmentTree2D(long long x,long long y,T i,F f):x(x),y(y),id(i),f(f),root(new Node_seq(y,id,f)){
    x0=1;
    while(x0<x)x0<<=1;
  }

  void update(long long a,long long b,T val){
    assert(0<=a&&a<x&&0<=b&&b<y);
    node_ptr now(root);

    long long l=0,r=x0;
    while(r-l>1){
      now->seq.update(b,val);
      long long mid=l+(r-l)/2;

      if(a<mid){
        if(!now->left)now->left=std::make_shared<Node_seq>(y,id,f);
        now=now->left;r=mid;
      }else{
        if(!now->right)now->right=std::make_shared<Node_seq>(y,id,f);
        now=now->right;l=mid;
      }
    }
    now->seq.update(b,val);
  }

  T query(long long l,long long r,long long d,long long u){
    return query(l,r,d,u,root,0,x0);
  }
};