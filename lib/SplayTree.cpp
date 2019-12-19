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

template<typename T>
class SplayTree{
  private:

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

  node_ptr rotr(node_ptr t){
    node_ptr newroot=t->left;
    node_ptr temp=newroot->right;
    t->left=temp;
    newroot->right=update(t);
    return update(newroot);
  }

  node_ptr rotl(node_ptr t){
    node_ptr newroot=t->right;
    node_ptr temp=newroot->left;
    t->right=temp;
    newroot->left=update(t);
    return update(newroot);
  }

  node_ptr splay(node_ptr t,int k){
    if(!t)return t;

    assert(0<=k&&k<count(t));
    if(count(t->left)==k)return t;

    if(count(t->left)>k){
      assert(t->left);
      if(count(t->left->left)==k)return rotr(t);
      if(count(t->left->left)>k){
        t->left->left=splay(t->left->left,k);update(t->left);
        return rotr(rotr(t));
      }else{
        t->left->right=splay(t->left->right,k-count(t->left->left)-1);update(t->left);
        t->left=rotl(t->left);update(t);
        return rotr(t);
      }
    }else{
      assert(t->right);
      if(count(t->right->left)==k-count(t->left)-1)return rotl(t);
      if(count(t->right->left)>k-count(t->left)-1){
        t->right->left=splay(t->right->left,k-count(t->left)-1);update(t->right);
        t->right=rotr(t->right);update(t);
        return rotl(t);
      }else{
        t->right->right=splay(t->right->right,k-count(t->left)-count(t->right->left)-2);update(t->right);
        return rotl(rotl(t));
      }
    }
  }

  node_ptr merge(node_ptr l,node_ptr r){
    if(!l||!r)return l?l:r;

    node_ptr newroot=splay(l,count(l)-1);
    newroot->right=r;
    return update(newroot);
  }

  std::pair<node_ptr,node_ptr> split(node_ptr t,int k){
    assert(0<=k&&k<=count(t));
    if(!t)return std::make_pair(t,t);

    if(k==0)return std::make_pair(nullptr,t);
    if(k==count(t))return std::make_pair(t,nullptr);

    node_ptr newroot=splay(t,k-1);
    node_ptr temp=newroot->right;
    newroot->right=nullptr;
    return std::make_pair(update(newroot),temp);
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
    }

    int mid=l+(r-l)/2;
    t=std::make_shared<Node>(val_[mid]);
    build(t->left,val_,l,mid);
    build(t->right,val_,mid+1,r);
    update(t);
  }

  public:

  SplayTree(T id_,F fn_):id(id_),fn(fn_),root(){};

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

  T query(){return calc(root);}

  T query(int a,int b){
    auto temp=split(root,b);
    auto temp2=split(temp.first,a);
    T ret=calc(temp2.second);

    root=merge(merge(temp2.first,temp2.second),temp.second);

    return ret;
  }
};