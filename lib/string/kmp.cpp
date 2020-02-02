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

std::vector<long long> kmp(std::string s){
  long long n=s.size();
  std::vector<long long> ret(n+1);
  ret[0]=-1;
  long long j=-1;
  for(long long i=0;i<n;i++){
    while(j>=0&&s[i]!=s[j])j=ret[j];
    j++;
    ret[i+1]=j;
  }
  return ret;
}

std::vector<long long> kmp_search(const std::string& str,const std::string& word){
  std::vector<long long> table=kmp(word),ret;
  long long m=0,i=0,n=str.size();
  while(m+i<n){
    if(word[i]==str[m+i]){
      if(++i==word.size()){
        ret.push_back(m);
        m+=i-table[i];
        i=table[i];
      }
    }else{
      m+=i-table[i];
      if(i>0)i=table[i];
    }
  }
  return ret;
}