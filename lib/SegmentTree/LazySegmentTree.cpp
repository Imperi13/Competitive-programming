#include <assert.h>
#include <math.h>
#include <algorithm>
#include <bitset>
#include <cctype>
#include <complex>
#include <deque>
#include <iomanip>
#include <iostream>
#include<functional>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <vector>

//遅延セグ木
template<typename T,typename E>
class LazySegmentTree{
  private:
  int n,n0;
  std::vector<T> dat;
  std::vector<E> lazy;

  T init_t;
  E init_e;

  using F=std::function<T(T,T)>;
  using G=std::function<T(T,E)>;
  using H=std::function<E(E,E)>;
  using P=std::function<E(E,int)>;

  F f;
  G g;
  H h;
  P p;

  T update(int a,int b,E val,int l,int r,int k){
    if(lazy[k]!=init_e){
      if(k<n0-1){
        lazy[2*k+1]=h(lazy[2*k+1],lazy[k]);
        lazy[2*k+2]=h(lazy[2*k+2],lazy[k]);
      }
      dat[k]=g(dat[k],p(lazy[k],r-l));
      lazy[k]=init_e;
    }

    if(r<=a||b<=l)return dat[k];
    if(a<=l&&r<=b){
      lazy[k]=h(lazy[k],val);
      return g(dat[k],p(lazy[k],r-l));
    }

    return dat[k]=f(update(a,b,val,l,l+(r-l)/2,2*k+1),update(a,b,val,l+(r-l)/2,r,2*k+2));
  }

  T query(int a,int b,int l,int r,int k){
    if(lazy[k]!=init_e){
      if(k<n0-1){
        lazy[2*k+1]=h(lazy[2*k+1],lazy[k]);
        lazy[2*k+2]=h(lazy[2*k+2],lazy[k]);
      }
      dat[k]=g(dat[k],p(lazy[k],r-l));
      lazy[k]=init_e;
    }

    if(r<=a||b<=l)return init_t;
    if(a<=l&&r<=b)return dat[k];
    T lval=query(a,b,l,l+(r-l)/2,2*k+1);
    T rval=query(a,b,l+(r-l)/2,r,2*k+2);
    return f(lval,rval);
  }

  public:

  LazySegmentTree(int n_,F f_,G g_,H h_,T t,E e,std::vector<T> dat_=std::vector<T>(),P p_=[](E a,int n){return a;})
    :n(n_),init_t(t),init_e(e),f(f_),g(g_),h(h_),p(p_){

    n0=1;
    while(n0<n)n0<<=1;
    dat=std::vector<T>(2*n0-1,init_t);
    lazy=std::vector<E>(2*n0-1,init_e);
    if(n_==dat_.size()){
      for(int i=0;i<n_;i++)dat[i+n0-1]=dat_[i];
      for(int i=n0-2;i>=0;i--)dat[i]=f(dat[2*i+1],dat[2*i+2]);
    }
  }

  void update(int a,int b,E val){
    update(a,b,val,0,n0,0);
  }

  T query(int a,int b){
    return query(a,b,0,n0,0);
  }
};