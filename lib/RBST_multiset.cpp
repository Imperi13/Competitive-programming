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
// multiset
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

  class Node;

  using node_ptr=std::shared_ptr<Node>;
  using F=std::function<bool(T,T)>;

  struct Node{
    node_ptr left,right;
    T val;
    int cnt;
    Node(T val_):val(val_),cnt(1),left(),right(){}
  };

  node_ptr root;
  F comp,equal;

  int count(node_ptr t){return t?t->cnt:0;}

  node_ptr update(node_ptr t){
    if(!t)return t;
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

  bool find(node_ptr& t,T key){
    if(!t)return false;
    if(equal(t->val,key))return true;

    if(comp(key,t->val))return find(t->left,key);
    else return find(t->right,key);
  }

  int lower_bound(node_ptr& t,T key){
    if(!t)return 0; 

    if(comp(key,t->val)||equal(key,t->val))return lower_bound(t->left,key);
    else return lower_bound(t->right,key)+count(t->left)+1;
  }

  int upper_bound(node_ptr& t,T key){
    if(!t)return 0;
    if(comp(key,t->val))return upper_bound(t->left,key);
    else return upper_bound(t->right,key)+count(t->left)+1;
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

  T topk(node_ptr& t,int k){
    if(count(t->left)==k)return t->val;
    if(k<count(t->left))return topk(t->left,k);
    else return topk(t->right,k-count(t->left)-1);
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

  RBST(F comp_=[](T a,T b){return a<b;},F equal_=[](T a,T b){return a==b;}):comp(comp_),equal(equal_),root(){};

  void build(const std::vector<T>& val_){
    build(root,val_,0,val_.size());
  }

  int size(){return count(root);}

  bool find(T key){return find(root,key);}

  int count(T key){return upper_bound(root,key)-lower_bound(root,key);}

  int lower_bound(T key){return lower_bound(root,key);}

  int upper_bound(T key){return upper_bound(root,key);}

  void insert(T key){
    insert(root,std::make_shared<Node>(key),lower_bound(root,key));
  }

  void erase(T key){
    if(count(key)==0)return;
    erase(root,lower_bound(root,key));
  }

  //0-indexedになってるので注意
  T topk(int k){return topk(root,k);}
};