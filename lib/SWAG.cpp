#include <cassert>
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
#include <memory>
#include <utility>
#include <limits>
#include "limits.h"

//SWAG スライド区間上の演算ができる
template<typename T>
class SWAG{
  private:

  using F=std::function<T(T,T)>;

  F fn;

  std::stack<T> front,back;
  std::vector<T> front_fold,back_fold;

  T id;

  public:

  SWAG(T id_,F fn_):id(id_),fn(fn_),front_fold(1,id_),back_fold(1,id_){}

  void push(T val){
    back.push(val);
    back_fold.push_back(fn(back_fold.back(),val));
  }

  void pop(){
    if(front.empty()){
      assert(front_fold.size()==1);
      while(!back.empty()){
        T temp=back.top();
        back.pop();
        back_fold.pop_back();

        front.push(temp);
        front_fold.push_back(fn(temp,front_fold.back()));
      }
    }

    assert(!front.empty());

    front.pop();
    front_fold.pop_back();
    
  }

  T fold_all(){
    return fn(front_fold.back(),back_fold.back());
  }
};