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

long long max_rectangle(const std::vector<long long>& hist){
  using ll=long long;
  using P=std::pair<ll,ll>;

  ll w=hist.size();
  ll ret=0;
  std::stack<P> sta;
  for(ll i=0;i<w;i++){
    ll left=i;
    while(!sta.empty()&&sta.top().first>=hist[i]){
      auto e=sta.top();sta.pop();
      left=e.second;
      if((i-e.second)*e.first>ret)ret=(i-e.second)*e.first;
    }
    sta.push(P(hist[i],left));
  }
  while(!sta.empty()){
    auto e=sta.top();sta.pop();
    if((w-e.second)*e.first>ret)ret=(w-e.second)*e.first;
  }
  return ret;
}