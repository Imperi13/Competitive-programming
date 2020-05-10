#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <complex>
#include <chrono>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
#include <random>
#include <utility>
#include <limits>
#include <list>

template<typename T>
class PersistentArray{
  private:

  struct Node;
  using node_ptr=std::shared_ptr<Node>;

  struct Node{
    T val;
    node_ptr left,right;
    Node():val(),left(nullptr),right(nullptr){}
    Node(T val_):val(val_),left(nullptr),right(nullptr){}
  };

  std::size_t n,n0;
  node_ptr root;

  PersistentArray(std::size_t n_,std::size_t n0_,node_ptr root_):n(n_),n0(n0_),root(root_){}

  void build(node_ptr now,std::size_t l,std::size_t r,T val_){
    if(r-l>1){
      now->left=std::make_shared<Node>(val_);
      now->right=std::make_shared<Node>(val_);
      build(now->left,l,(r+l)/2,val_);
      build(now->right,(r+l)/2,r,val_);
    }
  }

  public:

  PersistentArray(std::size_t n_,T init=T()):n(n_),root(new Node(init)){
    n0=1;
    while(n0<n_)n0<<=1;
    build(root,0,n0,init);
  }

  T operator[](std::size_t pos){
    assert(0<=pos&&pos<n);
    std::size_t l=0,r=n0;
    node_ptr now=root;
    while(r-l>1){
      if(pos<(l+r)/2){
        now=now->left;
        r=(l+r)/2;
      }else{
        now=now->right;
        l=(l+r)/2;
      }
    }
    return now->val;
  }

  PersistentArray update(std::size_t pos,T val_){
    node_ptr new_root(new Node(val_));
    node_ptr now=root;
    node_ptr make=new_root;
    std::size_t l=0,r=n0;
    while(r-l>1){
      if(pos<(l+r)/2){
        make->right=now->right;
        make->left=std::make_shared<Node>(val_);
        make=make->left;
        now=now->left;
        r=(l+r)/2;
      }else{
        make->left=now->left;
        make->right=std::make_shared<Node>(val_);
        make=make->right;
        now=now->right;
        l=(l+r)/2;
      }
    }

    return PersistentArray(n,n0,new_root);
  }
};

class PersistentUnionFind{
  private:

  PersistentArray<long long> uni;
  long long num,group;

  PersistentUnionFind(PersistentArray<long long> uni_,long long num_,long long group_):uni(uni_),num(num_),group(group_){}

  public:
  PersistentUnionFind(long long s=0):uni(s,-1),num(s),group(s){}

  long long root(long long a){
    if(uni[a]<0)return a;

    long long tmp=root(uni[a]);
    uni=uni.update(a,tmp);
    return tmp;
  }

  PersistentUnionFind unite(long long a,long long b){
    a=root(a);
    b=root(b);
    if(a==b)return *this;
    group--;
    if(uni[a]>uni[b])std::swap(a,b);

    PersistentArray<long long> newuni=uni;
    long long va=uni[a],vb=uni[b];
    newuni=newuni.update(a,va+vb);
    newuni=newuni.update(b,a);
    return PersistentUnionFind(newuni,num,group);
  }

  bool isconnect(long long a,long long b){return root(a)==root(b);}
  long long size(long long a){return -uni[root(a)];}
  long long groups(){return group;}
};