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


