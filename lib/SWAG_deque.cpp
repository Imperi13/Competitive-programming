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

template<typename T>
class SWAG_deque{
  private:

  using F=std::function<T(T,T)>;
  using P=std::pair<T,T>;

  std::stack<P> front,back;

  F f;

  public:

  SWAG(F f_):f(f_){}

  std::size_t size(){return front.size()+back.size();}
  bool empty(){return size()==0;}

  T fold_all(){
    assert(!empty());
    if(front.empty())return back.top().second;
    if(back.empty())return front.top().second;
    return f(front.top().second,back.top().second);
  }

  void push_back(T x){
    if(back.empty())back.emplace(x,x);
    else{
      auto tmp=back.top();
      back.emplace(x,f(tmp.second,x));
    }
  }

  void push_front(T x){
    if(front.empty())front.emplace(x,x);
    else{
      auto tmp=front.top();
      front.emplace(x,f(x,tmp.second));
    }
  }

  void pop_back(){
    assert(!empty());
    if(!back.empty()){
      back.pop();
      return;
    }
    std::size_t n=front.size();
    std::stack<T> tmp;
    for(std::size_t i=0;i<n/2;i++){
      tmp.emplace(front.top().first);
      front.pop();
    }
    while(!front.empty()){
      push_back(front.top().first);
      front.pop();
    }
    while(!tmp.empty()){
      push_front(tmp.top());
      tmp.pop();
    }
    back.pop();
  }

  void pop_front(){
    assert(!empty());
    if(!front.empty()){
      front.pop();
      return;
    }
    std::size_t n=back.size();
    std::stack<T> tmp;
    for(std::size_t i=0;i<n/2;i++){
      tmp.emplace(back.top().first);
      back.pop();
    }
    while(!back.empty()){
      push_front(back.top().first);
      back.pop();
    }
    while(!tmp.empty()){
      push_back(tmp.top());
      tmp.pop();
    }
    front.pop();
  }

};