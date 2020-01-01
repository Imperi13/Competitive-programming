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

//LiChaoSegmentTree
//minを返すCHTができる 線分追加に対応した
//PS:線分追加バグってる
template<typename T,T id>
class LiChaoSegmentTree{

  struct Line{
    std::pair<T,T> coeffi;

    Line(std::pair<T,T> a):coeffi(a){}

    T value(T x){
      return coeffi.first*x+coeffi.second;
    }
  };

  struct Node{

    std::shared_ptr<Node> left,right;
    Line line;

    Node():line(std::pair<T,T>(0,id)),left(),right(){}

    Node(Line line_):line(line_),left(),right(){}
  };

  private:

  long long n,n0;
  std::vector<T> num;

  std::shared_ptr<Node> root;

  void eval(Line f,long long a,long long b,std::shared_ptr<Node> now,long long l,long long r){

    if(a==b)return;

    if(r-l>1){
      if(f.value(num[l])>=now->line.value(num[l])&&f.value(num[std::min(n,r)-1])>=now->line.value(num[std::min(n,r)-1])){
        return;
      }

      long long mid=l+(r-l)/2;

      if(a<=l&&r<=b){

        if(n<=mid){
          if(now->line.value(num[n-1]) > f.value(num[n-1])){
            std::swap(now->line,f);
          }

          if(!now->left)now->left=std::make_shared<Node>();
          eval(f,a,b,now->left,l,mid);
        }else{
          if(now->line.value(num[mid-1]) > f.value(num[mid-1])){
            std::swap(now->line,f);
          }

          if(now->line.value(num[l]) > f.value(num[l])){
            if(!now->left)now->left=std::make_shared<Node>();
            eval(f,a,b,now->left,l,mid);
          }else{
            if(!now->right)now->right=std::make_shared<Node>();
            eval(f,a,b,now->right,mid,r);
          }
        }
      }else{
        if(b>l&&mid>a){
          if(!now->left)now->left=std::make_shared<Node>();
          eval(f,a,b,now->left,l,mid);
        }
        if(b>mid&&r>a){
          if(!now->right)now->right=std::make_shared<Node>();
          eval(f,a,b,now->right,mid,r);
        }
      }
    }else{
      if(now->line.value(l) > f.value(l)){
        now->line=f;
        return;
      }
    }
  }

  public:

  LiChaoSegmentTree(long long n_):n(n_),root(new Node()),num(std::vector<T>(n_,0)){
    for(ll i=1;i<n_;i++)num[i]=num[i-1]+1;

    n0=1;
    while(n0<n_)n0<<=1;
  }

  LiChaoSegmentTree(const std::vector<T>& num_):n(num_.size()),root(new Node()),num(num_){
    n0=1;
    while(n0<n)n0<<=1;
  }



  void addline(std::pair<T,T> line_){
    Line f=Line(line_);
    eval(f,0,n0,root,0,n0);
  }

  void addsegment(std::pair<T,T> line_,T a,T b){
    Line f=Line(line_);
    long long first=(std::lower_bound(all(num),a)-num.begin());
    long long second=(std::lower_bound(all(num),b)-num.begin());
    eval(f,first,second,root,0,n0);
  }

  T query(T val){
    long long i=std::lower_bound(all(num),val)-num.begin();
    std::shared_ptr<Node> now(root);

    T ans=now->line.value(num[i]);

    long long l=0,r=n0;
    while(r-l>1){
      if(ans > now->line.value(num[i]))ans=now->line.value(num[i]);
      long long mid=l+(r-l)/2;

      if(i<mid){
        r=mid;
        if(!now->left)return ans;

        now=now->left;
      }else{
        l=mid;
        if(!now->right)return ans;

        now=now->right;
      }

    }

    if(now->line.value(num[i]) < ans)ans=now->line.value(num[i]);

    return ans;
  }
};