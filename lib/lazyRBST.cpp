#include <cassert>
#include <algorithm>
#include <bitset>
#include <cctype>
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

//RBST merge/split based 
// 数列操作
template<typename T,typename E>
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
  }rnd;

  struct Node;

  using node_ptr=std::shared_ptr<Node>;
  using F=std::function<T(T,T)>;
  using G=std::function<T(T,E)>;
  using H=std::function<E(E,E)>;
  using P=std::function<E(E,int)>;
  
  F f;
  G g;
  H h;
  P p;
  T id_t;
  E id_e;

  struct Node{
    node_ptr left,right;
    T val,sum;
    E lazy;
    int cnt;
    bool rev;
    Node(T val_,E lazy):val(val_),sum(val_),lazy(lazy),cnt(1),left(),right(),rev(false){}
  };

  node_ptr root;

  T calc(node_ptr t){return t?t->sum:id_t;}
  int count(node_ptr t){return t?t->cnt:0;}

  void push(node_ptr t){
    if(t->lazy!=id_e){
      t->val=g(t->val,t->lazy);
      if(t->left){
        t->left->sum=g(t->left->sum,p(t->lazy,count(t->left)));
        t->left->lazy=h(t->left->lazy,t->lazy);
      }
      if(t->right){
        t->right->sum=g(t->right->sum,p(t->lazy,count(t->right)));
        t->right->lazy=h(t->right->lazy,t->lazy);
      }
      t->lazy=id_e;
    }
    if(t->rev){
      std::swap(t->left,t->right);
      if(t->left)t->left->rev^=1;
      if(t->right)t->right->rev^=1;
      t->rev=false;
    }
  }

  node_ptr update(node_ptr t){
    if(!t)return t;
    t->sum=f(f(calc(t->left),t->val),calc(t->right));
    t->cnt=count(t->left)+count(t->right)+1;
    return t;
  }

  node_ptr merge(node_ptr l,node_ptr r){
    if(l)push(l);
    if(r)push(r);
    if(!l||!r)return l?update(l):update(r);

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

    push(t);

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
      t=std::make_shared<Node>(val_[l],id_e);
      return;
    }

    int mid=l+(r-l)/2;
    t=std::make_shared<Node>(val_[mid],id_e);
    build(t->left,val_,l,mid);
    build(t->right,val_,mid+1,r);
    update(t);
  }

  public:

  RBST(T id_t,E id_e,F f,G g,H h,P p):id_t(id_t),id_e(id_e),f(f),g(g),h(h),p(p),root(){};

  void build(const std::vector<T>& val_){
    build(root,val_,0,val_.size());
  }

  int size(){return count(root);}

  void insert(int k,T val){
    insert(root,std::make_shared<Node>(val,id_e),k);
  }

  void erase(int k){
    erase(root,k);
  }

  void reverse(int a,int b){
    auto temp=split(root,b);
    auto temp2=split(temp.first,a);
    if(temp2.second)temp2.second->rev^=1;

    root=merge(merge(temp2.first,temp2.second),temp.second);
  }

  void set_propergate(int a,int b,E lazy){
    auto temp=split(root,b);
    auto temp2=split(temp.first,a);
    if(temp2.second)temp2.second->lazy=h(temp2.second->lazy,lazy);

    root=merge(merge(temp2.first,temp2.second),temp.second);
  }

  T query(){
    push(root);
    update(root);
    return calc(root);
  }

  T query(int a,int b){
    auto temp=split(root,b);
    auto temp2=split(temp.first,a);
    push(temp2.second);
    update(temp2.second);
    T ret=calc(temp2.second);

    root=merge(merge(temp2.first,temp2.second),temp.second);

    return ret;
  }
};