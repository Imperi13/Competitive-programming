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

template<typename T,typename F>
class FibonacciHeap{
  private:

  struct Node;
  using node_ptr = std::shared_ptr<Node>;
  //using node_ptr=Node *;

  struct Node{
    T val;
    node_ptr left,right;
    std::vector<node_ptr> ch;

    Node(T val_):val(val_),left(nullptr),right(nullptr),ch(0){}
    size_t degree(){return ch.size();}
  };

  node_ptr min_ptr;
  F comp;

  void update(){
    if(!min_ptr)return;

    std::vector<node_ptr> roots;
    roots.emplace_back(min_ptr);
    while(roots.back()->right!=min_ptr){
      auto temp=roots.back();
      roots.emplace_back(temp);
    }

    std::vector<node_ptr> size(roots.size()+1,nullptr);

    for(auto root:roots){
      while(size[root.degree()]){
        node_ptr temp=size[root.degree()];
        size[root.degree()]=nullptr;
        if(comp(root->val,temp->val)){
          root->ch.emplace_back(temp);
        }else{
          temp->ch.emplace_back(root);
          root=temp;
        }
      }
    }

  }

  public:

  FibonacciHeap(F comp_):min_ptr(nullptr),comp(comp_){}

  bool empty(){return !min_ptr;}
};