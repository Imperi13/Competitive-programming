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
#include <list>

/*
  Fibonacci_Heap

  insert - Heapに値を追加
  top - 最小値を取得
  pop - 最小値を削除

  WIP: decrement
*/
class Fibonacci_Heap{
  private:

  struct Node{
    long long val;
    std::size_t deg;
    bool mark;
    Node *par,*ch;
    Node *left,*right;
    Node(long long val_=0):val(val_),deg(0),mark(false),par(nullptr),ch(nullptr),left(this),right(this){}
    ~Node(){
      par=nullptr;
      if(ch){delete ch;ch=nullptr;}
      if(left)left->right=nullptr;
      if(right)delete right;
      right=nullptr;
      left=nullptr;
    }
    //thisの子にvを追加する vはparを持たないことが保障される
    Node *insert_ch(Node *v){
      deg++;
      if(!ch){
        ch=v;
        v->par=this;
        return v;
      }
      Node *tmp=ch->right;
      ch->right=v;v->left=ch;
      tmp->left=v;v->right=tmp;
      v->par=this;
      return this;
    }
    //thisの子であるvをcutする thisの子にvが含まれていることが保障される
    Node *cut_ch(Node *v){
      deg--;
      v->par=nullptr;
      if(deg==0){
        ch=nullptr;
        return v;
      }
      Node *l=v->left,*r=v->right;
      l->right=r;r->left=l;
      ch=r;
      v->left=v;v->right=v;
      return this;
    }
  };

  Node *min_ptr;
  std::size_t n;

  public:

  Fibonacci_Heap():min_ptr(nullptr),n(0){}
  Fibonacci_Heap(const Fibonacci_Heap&)=delete;
  Fibonacci_Heap& operator=(const Fibonacci_Heap&)=delete;
  ~Fibonacci_Heap(){
    if(min_ptr)delete min_ptr;
    min_ptr=nullptr;
  }

  bool empty(){return !min_ptr;}
  std::size_t size(){return n;}

  
  long long top(){assert(min_ptr);return min_ptr->val;}

  Node *insert(long long val){
    n++;
    Node *tmp=new Node(val);
    if(!min_ptr){
      min_ptr=tmp;
      return tmp;
    }
    Node *right=min_ptr->right;
    min_ptr->right=tmp;tmp->left=min_ptr;
    right->left=tmp;tmp->right=right;
    if(min_ptr->val>val)min_ptr=tmp;
    return tmp;
  }

  void pop(){
    assert(min_ptr);
    if(n==1){
      delete min_ptr;
      min_ptr=nullptr;
      n--;
      return;
    }

    if(min_ptr->ch){
      Node *tmp=min_ptr->ch;
      tmp->par=nullptr;tmp->mark=false;
      while(tmp->right!=min_ptr->ch){
        tmp=tmp->right;
        tmp->par=nullptr;tmp->mark=false;
      }
      Node *l=min_ptr->ch,*r=min_ptr->ch->left;
      min_ptr->ch=nullptr;
      Node *root_r=min_ptr->right;
      min_ptr->right=l;l->left=min_ptr;
      root_r->left=r;r->right=root_r;
    }

    Node *l=min_ptr->left,*r=min_ptr->right;
    l->right=r;min_ptr->left=nullptr;
    r->left=l;min_ptr->right=nullptr;
    delete min_ptr;//min_ptrの解放
    n--;

    //木のマージ
    std::size_t cnt=0;
    {
      std::size_t temp=n+1;
      while(temp>0){
        cnt++;
        temp>>=1;
      }
    }

    std::vector<Node *> deg_list(2*cnt+1,nullptr);
    Node *now=r->right;
    r->right=r;r->left=r;
    deg_list[r->deg]=r;
    while(now!=r){
      Node *temp=now;
      now=now->right;
      temp->left=temp;temp->right=temp;
      while(deg_list[temp->deg]){
        std::size_t deg=temp->deg;
        if(deg_list[deg]->val < temp->val){
          deg_list[deg]->insert_ch(temp);
          temp=deg_list[deg];
        }else{
          temp->insert_ch(deg_list[deg]);
        }
        deg_list[deg]=nullptr;
      }
      deg_list[temp->deg]=temp;
    }

    Node *banpei=new Node();
    Node *temp=banpei;
    for(auto&& e:deg_list){
      if(e){
        temp->right=e;
        e->left=temp;
        temp=e;
      }
    }
    temp->right=banpei->right;
    banpei->right->left=temp;
    banpei->right=banpei;
    delete banpei;banpei=nullptr;

    min_ptr=temp;
    now=temp->right;
    while(now!=temp){
      if(now->val < min_ptr->val)min_ptr=now;
      now=now->right;
    }

  }

  void decrement(Node *v,long long new_val){
    assert(v->val > new_val);
    v->val = new_val;
    if(!v->par){
      if(min_ptr->val > v->val)min_ptr=v;
      return;
    }

    if(!(v->par->val > v->val))return;

    Node *now=v;
    while(now->par->mark){
      Node *p=now->par;
      p->cut_ch(now);
      Node *next=min_ptr->right;
      min_ptr->right=now;now->left=min_ptr;
      next->left=now;now->right=next;
      now=p;
    }
    
    Node *p=now->par;
    p->cut_ch(now);
    Node *next=min_ptr->right;
    min_ptr->right=now;now->left=min_ptr;
    next->left=now;now->right=next;
    if(!p->par)p->mark=false;

    if(new_val < min_ptr->val)min_ptr=v;
  }

};