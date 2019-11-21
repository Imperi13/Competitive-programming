#include <assert.h>
#include <math.h>
#include <algorithm>
#include <bitset>
#include <cctype>
#include <complex>
#include <deque>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <vector>

using ll = long long;

//非再帰トポロジカルソート 隣接リストを渡す 返り値のサイズがnじゃないならnot DAG
std::vector<ll> topo_sort(std::vector<std::vector<ll>>& graph) {
  ll n = graph.size();
  std::vector<ll> ret, num(n, 0);
  std::queue<ll> que;
  for(ll i=0;i<n;i++){
    for (ll j : graph[i]) num[j]++;
  }
  for (ll i = 0; i < n; i++) {
    if (num[i] == 0) que.push(i);
  }
  while (!que.empty()) {
    ll u = que.front();
    que.pop();
    ret.push_back(u);
    for (ll i : graph[u]) {
      num[i]--;
      if (num[i] == 0) que.push(i);
    }
  }

  return ret;
}

//強連結成分分解 WIP
class StrongConnectedComponents{
  private:

  std::vector<std::vector<long long>> graph,graph_inv;
  long long v;
  std::vector<long long> compo,order,used;

  //WIP:dfsの戻り掛け順を非再帰で書きたい　バグってる
  /*void calc_order(){
    std::vector<long long> used(v,-1),par(v,-1);
    for(long long i=0;i<v;i++){
      if(used[i]==-1){
        std::stack<long long> que;
        que.push(i);
        while(!que.empty()){
          long long now=que.top();que.pop();
          if(used[now]==-1){
            used[now]=0;
            for(long long next:graph[now]){
              if(used[next]==-1){
                used[now]++;
                que.push(next);
                par[next]=now;
              }
            }
          }
          
          if(used[now]==0){
            order.push_back(now);
            if(par[now]!=-1){
              used[par[now]]--;
              que.push(par[now]);
            }
          }
        }
      }
    }
  }*/

  void dfs(int idx) {
    if(used[idx]) return;
    used[idx] = true;
    for(int to : graph[idx]) dfs(to);
    order.push_back(idx);
  }

  void calc_compo(){
    std::reverse(order.begin(),order.end());
    long long num=0;
    for(long long now:order){
      if(compo[now]==-1){
        std::stack<long long> que;
        que.push(now);
        while(!que.empty()){
          long long e=que.top();que.pop();
          compo[e]=num;
          for(long long next:graph_inv[e]){
            if(compo[next]==-1){
              que.push(next);
            }
          }
        }
        num++;
      }
    }
  }

  public:

  StrongConnectedComponents(std::vector<std::vector<long long>>& g):graph(g.size()),graph_inv(g.size()),v(g.size()),compo(g.size(),-1),used(g.size(),false){
    for(long long i=0;i<v;i++){
      for(long long next:g[i]){
        graph[i].push_back(next);
        graph_inv[next].push_back(i);
      }
    }
  }

  std::vector<long long> calc(){
    for(long long i=0;i<v;i++)dfs(i);
    calc_compo();

    return compo;
  }

};