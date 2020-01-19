#include <cassert>
#include "limits.h"
#include <limits>
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

/*
    LCAはEuler Tour+RMQで割と簡潔に実装できる
    RMQ部分にSegmentTreeを用いれば
    下記のダブリングの時間計算量そのままに空間計算量を減らせる
*/


//LowestCommonAncestor
//ダブリングを使う 前処理O(Nlog N),クエリO(log N)
class LowestCommonAncestor{
  using ll=long long;
  private:

  ll n,bit;
  std::vector<ll> depth;
  std::vector<std::vector<ll>> par_doubling,graph;

  ll getbit(ll temp){
    ll a=1;
    while(temp>0){
      a++;
      temp>>=1;
    }
    return a;
  }

  void dfs(ll now,ll par,ll dep){
    depth[now]=dep;
    if(par!=-1)par_doubling[now][0]=par;
    else par_doubling[now][0]=now;

    for(auto e:graph[now]){
      if(e!=par){
        dfs(e,now,dep+1);
      }
    }
  }

  void doubling(){
    for(ll i=1;i<bit;i++){
      for(ll j=0;j<n;j++){
        par_doubling[j][i]=par_doubling[par_doubling[j][i-1]][i-1];
      }
    }
  }

  public:

  //隣接リストのグラフと木の根を渡す
  LowestCommonAncestor(const std::vector<std::vector<ll>>& g,ll root_):graph(g),n(g.size()),bit(getbit(g.size())),
                                                              depth(n),par_doubling(n,std::vector<ll>(bit)){
    dfs(root_,-1,0);
    doubling();
  }
  
  ll query(ll a,ll b){
    if(depth[a]<depth[b])std::swap(a,b);

    for(ll i=0;i<bit;i++){
      if(((depth[a]-depth[b])>>i)&1)a=par_doubling[a][i];
    }

    if(a==b)return a;

    for(ll i=bit-1;i>=0;i--){
      if(par_doubling[a][i]!=par_doubling[b][i]){
        a=par_doubling[a][i];
        b=par_doubling[b][i];
      }
    }

    return par_doubling[a][0];
  }

};