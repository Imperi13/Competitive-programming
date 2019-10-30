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

void replace_all(std::string& s,std::string target,std::string replace){
  if(!target.empty()){
    long long now=0;
    while((now=s.find(target,now))!=std::string::npos){
      s.replace(now,target.length(),replace);
      now+=replace.length();
    }
  }
}