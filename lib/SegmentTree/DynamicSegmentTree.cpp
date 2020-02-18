#include <cassert>
#include <cmath>
#include <algorithm>
#include <bitset>
#include <cctype>
#include <complex>
#include <deque>
#include <iomanip>
#include <iostream>
#include <functional>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <vector>
#include <memory>
#include <utility>

//動的セグ木 
template<typename T>
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

//omake

//動的セグ木 
//可換演算かつupdateで代入操作をしないときの軽い実装
template<typename T,typename F>
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