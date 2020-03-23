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
#include <memory>
#include <utility>

template<typename T>
class PersistentStack{
  private:

  class Node{
    public:
    T value;
    std::shared_ptr<Node> next;

    Node(T value_,const std::shared_ptr<Node>& next_):value(value_),next(next_){}
  };

  using node_ptr=std::shared_ptr<Node>;

  node_ptr root;

  public:

  PersistentStack():root(){}

  PersistentStack(const node_ptr& root_):root(root_){}

  bool empty(){
    return !root;
  }

  T top(){
    assert(root);
    return root->value;
  }

  PersistentStack push(T value_){
    return PersistentStack(std::make_shared<Node>(value_,root));
  }

  PersistentStack pop(){
    assert(root);
    return PersistentStack(root->next);
  }
};

//永続動的セグ木
template<typename T>
class PersistentSegmentTree{
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

    Node(T i,const std::shared_ptr<Node>& par_):value(i),left(),right(),par(par_){}

    Node(T i,std::shared_ptr<Node>& par_):value(i),left(),right(),par(std::move(par_)){}

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
  PersistentSegmentTree(long long n_,T i,F f):n(n_),init(i),fn(f),root(new Node(init)){
    n0=1;
    while(n0<n_)n0<<=1;
  }

  PersistentSegmentTree(const std::shared_ptr<Node>& root_,long long n_,T i,F f):n(n_),init(i),fn(f),root(root_){
    n0=1;
    while(n0<n_)n0<<=1;
  }

  PersistentSegmentTree():root(){}

  // 更新 Ai=fn(Ai,val) を行う,dest=trueのときは初期化
  PersistentSegmentTree update(long long i,T val,bool dest){
    assert(0<=i&&i<n);
    std::shared_ptr<Node> now(root);
    std::shared_ptr<Node> nr=std::make_shared<Node>(init),root_(nr);

    long long l=0,r=n0;
    bool isok=true;
    while(r-l>1){
      long long mid=l+(r-l)/2;
      if(i<mid){
        if(isok&&now->right)nr->right=now->right;
        nr->left=std::make_shared<Node>(init,nr);

        if(isok){
          if(!now->left)isok=false;
          else now=now->left;
        }

        nr=nr->left;

        r=mid;
      }else{
        if(isok&&now->left)nr->left=now->left;
        nr->right=std::make_shared<Node>(init,nr);

        if(isok){
          if(!now->right)isok=false;
          else now=now->right;
        }

        nr=nr->right;

        l=mid;
      }
    }

    if(dest)nr->value=val;
    else nr->value=fn((isok)?now->value:init,val);

    while(nr->par.lock()){
      nr=nr->par.lock();
      nr->value=fn((nr->left)?nr->left->value:init,(nr->right)?nr->right->value:init);
    }

    return PersistentSegmentTree(nr,n,init,fn);
  }

  //[a,b)の区間演算結果を返す
  T query(long long a,long long b){
    assert(0<=a&&b<=n);
    return query(a,b,root,0,n0);
  }
};