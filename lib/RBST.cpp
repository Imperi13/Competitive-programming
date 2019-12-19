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

//RBST merge/split based 
// 数列操作
template<typename T>
class RBST{
  private:

  class Xorshift{
    private:
    unsigned int x,y,z,w;

    public:
    Xorshift():x(123456789),y(362436069),z(521288629),w(88675123){}

    unsigned int operator() () {
      unsigned int t=(x^(x<<11));
      x=y;y=z;z=w;
      return w=(w^(w>>19))^(t^(t>>8));
    }
  };

  Xorshift rnd;

  struct Node;

  using node_ptr=std::shared_ptr<Node>;
  using F=std::function<T(T,T)>;

  struct Node{
    node_ptr left,right;
    T val,sum;
    int cnt;
    Node(T val_):val(val_),sum(val_),cnt(1),left(),right(){}
  };

  node_ptr root;
  F fn;
  T id;

  T calc(node_ptr t){return t?t->sum:id;}
  int count(node_ptr t){return t?t->cnt:0;}

  node_ptr update(node_ptr t){
    if(!t)return t;
    t->sum=fn(fn(calc(t->left),t->val),calc(t->right));
    t->cnt=count(t->left)+count(t->right)+1;
    return t;
  }

  node_ptr merge(node_ptr l,node_ptr r){
    if(!l||!r)return l?l:r;

    if(rnd()%(count(l)+count(r))<count(l)){
      l->right=merge(l->right,r);
      return update(l);
    }else{
      r->left=merge(l,r->left);
      return update(r);
    }
  }

  std::pair<node_ptr,node_ptr> split(node_ptr t,int k){
    if(!t)return std::make_pair(t,t);

    if(k<=count(t->left)){
      auto temp=split(t->left,k);
      t->left=temp.second;
      return std::make_pair(temp.first,update(t));
    }else{
      auto temp=split(t->right,k-count(t->left)-1);
      t->right=temp.first;
      return std::make_pair(update(t),temp.second);
    }
  }

  void insert(node_ptr& t,node_ptr newnode,int k){
    auto temp=split(t,k);
    t=merge(merge(temp.first,newnode),temp.second);
  }

  void erase(node_ptr& t,int k){
    auto temp=split(t,k+1);
    auto temp2=split(temp.first,k);
    t=merge(temp2.first,temp.second);
  }

  void build(node_ptr& t,const std::vector<T>& val_,int l,int r){
    if(l==r){
      t=nullptr;
      return;
    }
    if(r==l+1){
      t=std::make_shared<Node>(val_[l]);
      return;
    }

    int mid=l+(r-l)/2;
    t=std::make_shared<Node>(val_[mid]);
    build(t->left,val_,l,mid);
    build(t->right,val_,mid+1,r);
    update(t);
  }

  public:

  RBST(T id_,F fn_):id(id_),fn(fn_),root(){};

  void build(const std::vector<T>& val_){
    build(root,val_,0,val_.size());
  }

  int size(){return count(root);}

  void insert(int k,T val){
    insert(root,std::make_shared<Node>(val),k);
  }

  void erase(int k){
    erase(root,k);
  }

  T query(){
    return calc(root);
  }

  T query(int a,int b){
    auto temp=split(root,b);
    auto temp2=split(temp.first,a);
    T ret=calc(temp2.second);

    root=merge(merge(temp2.first,temp2.second),temp.second);

    return ret;
  }
};