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
#include "limits.h"

//SparseTable 実装上無効値を渡すこと
//(a*b)*c=a*(b*c) a*a=a を満たす演算と数列を渡すと区間のfoldができる
//前計算O(NlogN) query O(1)
template<typename T,T null>
class SparseTable{
  private:

  using F=std::function<T(T,T)>;

  std::vector<int> ln;
  std::vector<std::vector<T>> table;

  int n;
  F f;

  public:

  SparseTable(const std::vector<T>& a,F f):n(a.size()),f(f){
    ln=std::vector<int>(n+1,0);
    for(int i=1;i<n+1;i++)ln[i]=ln[i-1]+(i>=1ll<<(ln[i-1]+1));
    table=std::vector<std::vector<T>>(n,std::vector<T>(ln[n]+1,null));
    for(int i=0;i<n;i++)table[i][0]=a[i];
    for(int j=1;j<ln[n]+1;j++){
      for(int i=0;i<n;i++){
        if(i+(1ll<<j)>n)table[i][j]=null;
        else table[i][j]=f(table[i][j-1],table[i+(1ll<<(j-1))][j-1]);
      }
    }
  }

  T query(int a,int b){
    assert(a<b);
    int m=b-a;
    return f(table[a][ln[m]],table[b-(1ll<<ln[m])][ln[m]]);
  }

};