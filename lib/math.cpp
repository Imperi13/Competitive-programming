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

//非再帰累乗(mod付き)
ll pow_mod(ll a, ll b, ll mod) {
  if (a % mod == 0) {
    return 0;
  }

  ll x = 1;

  while (b > 0) {
    if (b & 1) {
      x = (x * a) % mod;
    }
    a = (a * a) % mod;
    b >>= 1;
  }
  return x;
}

//原始根を使うFFT 法をとっていることに注意
// max_dには多項式2つの次数の積より大きい2^nを渡す
template <ll modu, ll root>
class NumberTheoreticTransform {
  /*
   modとrootと次数の最大値の組
   1224736769 , 3 , 2^24
   998244353 , 3 , 2^23
  */

 private:

  ll pow_mod(ll a, ll b, ll mod) {
    if (a % mod == 0) {
      return 0;
    }

    ll x = 1;

    while (b > 0) {
      if (b & 1) {
        x = (x * a) % mod;
      }
      a = (a * a) % mod;
      b >>= 1;
    }
    return x;
  }

  std::vector<ll> zeta, temp;
  ll size = 1;

  public:

  std::vector<ll> fft(std::vector<ll>& a, bool inv = false) {
    ll mask = size - 1;
    ll p = 0;
    for (ll i = size >> 1; i >= 1; i >>= 1) {
      auto& cur = (p & 1) ? temp : a;
      auto& nex = (p & 1) ? a : temp;
      ll e = zeta[inv ? (size - i) % size : i];
      ll w = 1;
      for (ll j = 0; j < size; j += i) {
        for (ll k = 0; k < i; k++) {
          nex[j + k] = (cur[((j << 1) & mask) + k] +
                        w * cur[(((j << 1) + i) & mask) + k]) %
                       modu;
        }
        w = (w * e) % modu;
      }
      p++;
    }
    if (p & 1) std::swap(a, temp);
    if (inv)
      for (ll i = 0; i < size; i++)
        a[i] = (a[i] * pow_mod(size, modu - 2, modu)) % modu;
    return a;
  }

  std::vector<ll> mul(std::vector<ll>& a, std::vector<ll>& b) {
    ll m = a.size() + b.size() - 1;
    size = 1;
    while (m > size) {
      size <<= 1;
    }
    ll ze = pow_mod(root, (modu - 1) / size, modu);
    temp.resize(size);
    zeta.resize(size);
    zeta[0] = 1;
    for (ll i = 1; i < size; i++) zeta[i] = (zeta[i - 1] * ze) % modu;
    std::vector<ll> A(size), B(size);
    for (ll i = 0; i < a.size(); i++) A[i] = a[i];
    for (ll i = 0; i < b.size(); i++) B[i] = b[i];

    A = fft(A);
    B = fft(B);
    for (ll i = 0; i < size; i++) A[i] = (A[i] * B[i]) % modu;
    A = fft(A, true);
    A.resize(m);
    return A;
  }
};
