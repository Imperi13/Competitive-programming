#include <assert.h>
#include <limits.h>
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

class Dinic{
  private:

  struct Edge{
    long long to,cap,rev;
    Edge(long long a,long long b,long long c):to(a),cap(b),rev(c){}
  };

  long long MAX_V;
  std::vector<std::vector<Edge>> graph;
  std::vector<long long> level,iter;

  void bfs(long long s){
    level=std::vector<long long>(MAX_V,-1);
    std::queue<long long> que;
    level[s]=0;
    que.push(s);

    while(!que.empty()){
      long long v=que.front();que.pop();
      for(long long i=0;i<graph[v].size();i++){
        const Edge &e = graph[v][i];
        if(e.cap>0&&level[e.to]<0){
          level[e.to]=level[v]+1;
          que.push(e.to);
        }
      }
    }
  }

  long long dfs(long long v,long long t,long long f){
    if(v==t)return f;
    for(long long i=iter[v];i<graph[v].size();i++){
      Edge &e=graph[v][i];
      if(e.cap>0&&level[v]<level[e.to]){
        long long d=dfs(e.to,t,std::min(f,e.cap));
        if(d>0){
          e.cap-=d;
          graph[e.to][e.rev].cap+=d;
          return d;
        }
      }
    }
    return 0;
  }

  public:

  Dinic(long long n):graph(n),level(n),iter(n),MAX_V(n){}

  void add_edge(long long from,long long to,long long cap){
    graph[from].emplace_back(Edge(to,cap,graph[to].size()));
    graph[to].emplace_back(Edge(from,0,graph[from].size()));
  }

  long long max_flow(long long s,long long t){
    long long flow=0;
    while(1){
      bfs(s);
      if(level[t]<0)return flow;

      iter=std::vector<long long>(MAX_V,0);
      long long f;
      while((f=dfs(s,t,LLONG_MAX))>0){
        flow+=f;
      }
    }
  }
};