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

//NまでのMOD逆元をコンパイル時計算する
template<long long N,long long MOD>
class ModInv_calc{
  public:
  long long inv[N+1];
  constexpr ModInv_calc():inv(){
    inv[1]=1;
    for(int i=2;i<=N;i++)inv[i]=inv[MOD%i]*(MOD-MOD/i)%MOD;
  }

  constexpr long long operator[](size_t i)const noexcept{return inv[i];}
};

constexpr ModInv_calc<1000,1000000007> inv{ModInv_calc<1000,1000000007>()};