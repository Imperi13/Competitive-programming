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
#include <memory>
#include <utility>

template<typename T>
class PersistentStack{
  private:

  class Node{
    public:
    T value;
    std::shared_ptr<Node> next;

    Node(T value_,const std::shared_ptr<Node>& next_):value(value_),next(next_){}
  };

  std::shared_ptr<Node> root;

  public:

  PersistentStack():root(){}

  PersistentStack(const std::shared_ptr<Node>& root_):root(root_){}

  bool empty(){
    return !root;
  }

  T top(){
    assert(root);
    return root->value;
  }

  PersistentStack push(T value_){
    return PersistentStack(std::make_shared<Node>(value_,root));
  }

  PersistentStack pop(){
    assert(root);
    return PersistentStack(root->next);
  }
};