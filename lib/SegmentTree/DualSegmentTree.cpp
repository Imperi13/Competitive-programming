#include <cassert>
#include <algorithm>
#include <array>
#include <bitset>
#include <cctype>
#include <cstdint>
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

//双対セグ木 可換モノイドを載せる
template<typename T>
class DualSegmentTree{
  private:

  using F=std::function<T(T,T)>;

  long long n,n0;
  F f;
  T init;

  class Node{
    public:
    std::shared_ptr<Node> left,right;
    T value;

    Node(T i):value(i),left(),right(){}
  };

  std::shared_ptr<Node> root;

  void update(long long a,long long b,T val,const std::shared_ptr<Node>& now,long long l,long long r){
    if(a<=l&&r<=b){
      now->value=f(now->value,val);
      return;
    }
    if(b<=l||r<=a)return;

    if(!now->left)now->left=std::make_shared<Node>(init);
    if(!now->right)now->right=std::make_shared<Node>(init);
    update(a,b,val,now->left,l,l+(r-l)/2);
    update(a,b,val,now->right,l+(r-l)/2,r);
  }

  public:

  //要素数の最大値n,単位元i,演算fを渡す
  DualSegmentTree(long long n_,T i,F f_):n(n_),init(i),f(f_),root(new Node(init)){
    n0=1;
    while(n0<n_)n0<<=1;
  }

  void update(long long l,long long r,T val){
    update(l,r,val,root,0,n0);
  }

  // 更新 Ai=fn(Ai,val) を行う,dest=trueのときは初期化
  T get(long long i){
    assert(0<=i&&i<n);
    std::shared_ptr<Node> now(root);
    T ret=now->value;

    long long l=0,r=n0;
    while(r-l>1){
      long long mid=l+(r-l)/2;

      if(i<mid){
        if(!now->left)break;

        now=now->left;

        r=mid;
      }else{
        if(!now->right)break;

        now=now->right;

        l=mid;
      }
      ret=f(ret,now->value);
    }
    return ret;
  }
};