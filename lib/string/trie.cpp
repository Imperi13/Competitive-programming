#include <cassert>
#include <algorithm>
#include <array>
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

//Trie木 prefix検索ができる 配列で実装しているので空間計算量が大きめ
//CHAR_SIZEは文字種類数,CHAR_BEGINにはその文字種の先頭文字を渡す
template<int CHAR_SIZE,int CHAR_BEGIN>
class Trie{
  private:

  struct Node{
    std::vector<int> next;
    int id;//単語が存在しないときは-1
    Node():next(CHAR_SIZE,-1),id(-1){}
  };

  std::vector<Node> tree;

  public:

  Trie():tree(1,Node()){}

  //辞書に単語を追加,id(>0)を割り振る
  void add_word(const std::string& word,int id_){
    int now=0;
    for(char c:word){
      if(tree[now].next[c-CHAR_BEGIN]==-1){
        int temp=tree.size();
        tree[now].next[c-CHAR_BEGIN]=temp;
        tree.emplace_back();
        now=temp;
      }else{
        now=tree[now].next[c-CHAR_BEGIN];
      }
    }
    if(tree[now].id==-1)tree[now].id=id_;
  }

  //prefixと一致する辞書単語のidを返す
  std::vector<int> search(const std::string& word){
    std::vector<int> ret;
    int now=0;
    for(char c:word){
      if(tree[now].next[c-CHAR_BEGIN]==-1)return ret;
      now=tree[now].next[c-CHAR_BEGIN];
      if(tree[now].id!=-1)ret.push_back(tree[now].id);
    }
    return ret;
  }
};