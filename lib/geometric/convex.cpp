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

//geometry library

using Point=std::complex<long double>;

const long double EPS=1e-10;
const long double PI=std::acos(-1.0l);

bool EQ(long double a,long double b){return std::abs(a-b)<EPS;}
bool EQP(Point a,Point b){return EQ(a.real(),b.real())&&EQ(a.imag(),b.imag());}

Point unitvec(Point a){return a/std::abs(a);}

//dot a・b = |a||b|cosθ
long double dot(Point a,Point b){
  return a.real()*b.real()+a.imag()*b.imag();
}

//cross a×b = |a||b|sinθ
long double cross(Point a,Point b){
  return a.real()*b.imag()-a.imag()*b.real();
}

//直線a,b上に点cがあるか
bool on_line(Point a,Point b,Point c){
  return EQ(cross(c-a,b-a),0.0l);
}

// [a,b]上に点cがあるか
bool on_segment(Point a,Point b,Point c){
  return std::abs(c-a)+std::abs(b-c)<std::abs(b-a)+EPS;
}

//どの2点も異なる点集合の凸包を求める
std::vector<std::pair<Point,long long>> convex(std::vector<std::pair<Point,long long>> set){
  using Pa=std::pair<Point,long long>;
  if(set.size()==1)return {set[0]};
  auto comp=[&](Pa a,Pa b){
    return (EQ(a.first.real(),b.first.real()))?(a.first.imag()>b.first.imag()):(a.first.real()<b.first.real());
  };

  std::sort(set.begin(),set.end(),comp);

  long long n=set.size();

  std::vector<Pa> lower,upper;

  lower.push_back(set[0]);
  long long lowernow=0;
  for(long long i=1;i<n;i++){
    while(lowernow>0){
      Point add=set[i].first-lower[lowernow].first;
      Point last=lower[lowernow-1].first-lower[lowernow].first;
      if(0<std::arg(last/add)&&std::arg(last/add)<PI-EPS){
        break;
      }else{
        lower.pop_back();
        lowernow--;
      }
    }
    if(i!=n-1)lower.push_back(set[i]);
    lowernow++;
  }

  upper.push_back(set[n-1]);
  long long uppernow=0;
  for(long long i=n-2;i>=0;i--){
    while(uppernow>0){
      Point add=set[i].first-upper[uppernow].first;
      Point last=upper[uppernow-1].first-upper[uppernow].first;
      if(0<std::arg(last/add)&&std::arg(last/add)<PI-EPS){
        break;
      }else{
        upper.pop_back();
        uppernow--;
      }
    }
    if(i!=0)upper.push_back(set[i]);
    uppernow++;
  }

  //for(auto low:lower)debug(low.first);
  //for(auto up:upper)debug(up.first);

  lower.reserve(lowernow+uppernow);
  std::copy(upper.begin(),upper.end(),std::back_inserter(lower));
  return lower;
}