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

//LiChaoSegmentTree 直線追加 O(logN) 線分追加 O(log^2 N)? 最小値クエリ O(log N)
//templateのMAXはLineの初期値として使う (直線の値域は[-INF,MAX)となる必要がある)
template<typename T,T MAX>
class LiChaoSegmentTree{
  using P=std::pair<T,T>;

  struct Line{
    P a;
    T value(T x){return a.first*x+a.second;}
    Line():a(0,MAX){}
    Line(P a_):a(a_){}
  };

  struct Node;
  using node_ptr=std::shared_ptr<Node>;

  struct Node{
    node_ptr left,right;
    Line line;
    Node():left(),right(),line(){}
  };

  private:

  long long n,n0;
  node_ptr root;
  std::vector<T> x;

  void eval(Line f,long long a,long long b,node_ptr now,long long l,long long r){
    if(a==b)return;
    if(r<=a||b<=l)return;
    if(f.value(x[l]) >= now->line.value(x[l]) && f.value(x[r-1]) >= now->line.value(x[r-1]))return;
    if(r-l==1){
      if(f.value(x[l]) < now->line.value(x[l])){
        std::swap(f,now->line);
        return;
      }
    }

    long long mid=l+(r-l)/2;

    if(a<=l&&r<=b){
      if(f.value(x[l]) < now->line.value(x[l]) && f.value(x[r-1]) < now->line.value(x[r-1])){
        std::swap(f,now->line);
        return;
      }
      if(f.value(x[mid-1]) < now->line.value(x[mid-1]))std::swap(f,now->line);
      if(f.value(x[l]) < now->line.value(x[l])){
        if(!now->left)now->left=std::make_shared<Node>();
        eval(f,a,b,now->left,l,mid);
      }else{
        if(!now->right)now->right=std::make_shared<Node>();
        eval(f,a,b,now->right,mid,r);
      }
    }else{
      if(mid>a&&b>l){
        if(!now->left)now->left=std::make_shared<Node>();
        eval(f,a,b,now->left,l,mid);
      }
      if(r>a&&b>mid){
        if(!now->right)now->right=std::make_shared<Node>();
        eval(f,a,b,now->right,mid,r);
      }
    }
  }

  public:

  //x座標を[0,n_)で初期化
  LiChaoSegmentTree(long long n_):n(n_),root(new Node()){
    n0=1;
    while(n0<n)n0<<=1;
    x=std::vector<T>(n0,n-1);
    for(long long i=0;i<n;i++)x[i]=i;
  }

  //ソート済みx座標を渡す
  LiChaoSegmentTree(const std::vector<T>& x_):n(x_.size()),root(new Node()){
    n0=1;
    while(n0<n)n0<<=1;
    x=std::vector<T>(n0,x_[n-1]);
    for(long long i=0;i<n;i++)x[i]=x_[i];
  }

  void add_line(P line_){
    eval(Line(line_),0,n0,root,0,n0);
  }

  void add_segment(P line_,T lval,T rval){
    long long l=(std::lower_bound(x.begin(),x.end(),lval)-x.begin());
    long long r=(std::lower_bound(x.begin(),x.end(),rval)-x.begin());
    eval(Line(line_),l,r,root,0,n0);
  }

  T query(T val){
    long long itr=(std::lower_bound(x.begin(),x.end(),val)-x.begin());
    assert(itr<n&&x[itr]==val);

    T ans=MAX;
    node_ptr now(root);
    long long l=0,r=n0;

    while(r-l>1){
      if(ans>now->line.value(val))ans=now->line.value(val);
      long long mid=l+(r-l)/2;
      if(itr<mid){
        if(!now->left)return ans;
        now=now->left;
        r=mid;
      }else{
        if(!now->right)return ans;
        now=now->right;
        l=mid;
      }
    }
    if(ans>now->line.value(val))ans=now->line.value(val);
    return ans;
  }
};