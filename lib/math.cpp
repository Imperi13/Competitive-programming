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

//拡張互除法 ax+by=gcd(a,b)となるx,yを参照渡しする
//返り値はgcd(a,b)
long long extgcd(long long a,long long b,long long& x,long long& y){
  for(ll u=y=1,v=x=0;a;){
    ll q=b/a;
    std::swap(x-=q*u,u);
    std::swap(y-=q*v,v);
    std::swap(b-=q*a,a);
  }

  return b;
}

//拡張gcdを使ったmod_inv a,modがcoprimeの必要あり
long long mod_inv(long long a,long long mod){
  long long x,y;
  long long b=mod;
  for(ll u=y=1,v=x=0;a;){
    ll q=b/a;
    std::swap(x-=q*u,u);
    std::swap(y-=q*v,v);
    std::swap(b-=q*a,a);
  }

  if(b!=1)return -1;
  x%=mod;
  if(x<0)x+=mod;
  return x;
}

long long garner(std::vector<long long> b,std::vector<long long> m,long long mod){
  m.emplace_back(mod);
  std::vector<long long> coeffs(m.size(),1);
  std::vector<long long> constants(m.size(),0);
  for(std::size_t i=0;i<b.size();i++){
    long long temp=(b[i]-constants[i])*mod_inv(coeffs[i],m[i])%m[i];
    if(temp<0)temp+=m[i];
    for(std::size_t j=i+1;j<m.size();j++){
      constants[j]+=temp*coeffs[j];
      constants[j]%=m[j];
      if(constants[j]<0)constants[j]+=m[j];
      coeffs[j]*=m[i];
      coeffs[j]%=m[j];
      if(coeffs[j]<0)coeffs[j]+=m[j];
    }
  }

  return constants.back();
}

//非再帰累乗(mod付き)
ll pow_mod(ll a, ll b, ll mod=-1) {
  if ((a == 0)||(mod!=-1&&a%mod==0)) {
    return 0;
  }

  ll x = 1;

  while (b > 0) {
    if (b & 1) {
      x = (mod!=-1)?(x * a) % mod:x*a;
    }
    a = (mod!=-1)?(a * a) % mod:a*a;
    b >>= 1;
  }
  return x;
}

//非再帰累乗
template<typename T>
T pow2(T a, long long b) {
  if (a == 0) {
    return 0;
  }
 
  T x = 1;
 
  while (b > 0) {
    if (b & 1) {
      x = (x * a);
    }
    a = (a * a);
    b >>= 1;
  }
  return x;
}

//原始根を使うFFT 法をとっていることに注意
// max_dには多項式2つの次数の積より大きい2^nを渡す
template <long long modu, long long root>
class NumberTheoreticTransform {

  // (924844033, 5)
  // (998244353, 3)
  // (1012924417, 5)
  // (167772161, 3)
  // (469762049, 3)
  // (1224736769, 3)

  using ll=long long;

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

  ll get_mod(){return modu;}

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
    for (std::size_t i = 0; i < a.size(); i++) A[i] = a[i];
    for (std::size_t i = 0; i < b.size(); i++) B[i] = b[i];

    A = fft(A);
    B = fft(B);
    for (ll i = 0; i < size; i++) A[i] = (A[i] * B[i]) % modu;
    A = fft(A, true);
    A.resize(m);
    return A;
  }
};

template<typename Num,std::size_t ROW,std::size_t COLUMN>
class Mat{
  private:

  public:

  std::vector<std::vector<Num>> a;

  Mat(const Num init=0):a(ROW,std::vector<Num>(COLUMN,init)){}

  constexpr Num& at(std::size_t i,std::size_t j){return a[i][j];}
  constexpr std::size_t get_row(){return ROW;}
  constexpr std::size_t get_column(){return COLUMN;}

  constexpr Mat operator+(const Mat rhs){
    return Mat(*this)+=rhs;
  }

  constexpr Mat operator-(const Mat rhs){
    return Mat(*this)-=rhs;
  }

  template<std::size_t NEWCOLUMN>
  constexpr Mat<Num,ROW,NEWCOLUMN> operator*(const Mat<Num,COLUMN,NEWCOLUMN> rhs){
    Mat<Num,ROW,NEWCOLUMN> tmp;
    for(std::size_t r=0;r<ROW;r++){
      for(std::size_t c=0;c<NEWCOLUMN;c++){
        for(std::size_t i=0;i<COLUMN;i++){
          tmp.a[r][c]+=a[r][i]*rhs.a[i][c];
        }
      }
    }
    return tmp;
  }

  constexpr Mat& operator+=(const Mat<Num,ROW,COLUMN> rhs){
    for(std::size_t i=0;i<ROW;i++){
      for(std::size_t j=0;j<COLUMN;j++){
        a[i][j]+=rhs.a[i][j];
      }
    }
    return *this;
  }

  constexpr Mat& operator-=(const Mat<Num,ROW,COLUMN> rhs){
    for(std::size_t i=0;i<ROW;i++){
      for(std::size_t j=0;j<COLUMN;j++){
        a[i][j]-=rhs.a[i][j];
      }
    }
    return *this;
  }

  constexpr Mat& operator*=(const Mat<Num,COLUMN,COLUMN> rhs){
    std::vector<std::vector<Num>> tmp(ROW,std::vector<Num>(COLUMN,0));
    for(std::size_t r=0;r<ROW;r++){
      for(std::size_t c=0;c<COLUMN;c++){
        for(std::size_t i=0;i<COLUMN;i++){
          tmp[r][c]+=a[r][i]*rhs.a[i][c];
        }
      }
    }
    a=std::move(tmp);
    return *this;
  }

  constexpr static Mat id(){
    static_assert(ROW==COLUMN,"ROW must be equal to COLUMN");
    Mat<Num,ROW,COLUMN> temp;
    for(std::size_t i=0;i<ROW;i++)temp.at(i,i)=1;
    return temp;
  }

  void show(){
    for(std::size_t i=0;i<ROW;i++){
      for(std::size_t j=0;j<COLUMN;j++){
        std::cout<<a[i][j]<<" ";
      }
      std::cout<<"\n";
    }
  }
};