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

//最大値セグ木 0-indexed 初期値はminで指定
template<class T>
struct SegmentTree {
  T n, low;
  std::vector<T> dat;

  SegmentTree(T i, T min) {
    n = 1;
    while (n < i) {
      n *= 2;
    }
    low = min;
    dat = std::vector<T>(2 * n - 1, low);
  }

  // k番目(0-indexed)を値aで更新
  void update(T k, T a) {
    k += n - 1;
    dat[k] = a;
    while (k > 0) {
      k = (k - 1) / 2;
      dat[k] = std::max(dat[k * 2 + 1], dat[k * 2 + 2]);
    }
  }

  T query(T a, T b, T k, T l, T r) {
    if (r <= a || b <= l) {
      return low;
    }
    if (a <= l && r <= b) {
      return dat[k];
    } else {
      T vl = query(a, b, k * 2 + 1, l, (l + r) / 2);
      T vr = query(a, b, k * 2 + 2, (l + r) / 2, r);
      return std::max(vl, vr);
    }
  }

  //[a,b)での最大値を返す
  T query(T a, T b) { return query(a, b, 0, 0, n); }
};

//とりあえず1-indexedでつくったBIT
template <class T>
struct BinaryIndexedTree {
  T n;
  std::vector<T> bit;

  BinaryIndexedTree(T i) : n(i), bit(i + 1, 0){};

  // a番目にwを加算
  void add(T a, T w) {
    for (T x = a; x <= n; x += (x & -x)) {
      bit[x] += w;
    }
  }

  //[1,a]の和
  T sum(T a) {
    T ret = 0;
    for (T x = a; x > 0; x -= (x & -x)) {
      ret += bit[x];
    }
    return ret;
  }
};

// UnionFind
template<class T>
struct UnionFind {
  std::vector<T> uni;
  T num;
  UnionFind(T s) : uni(s, -1), num(s) {}

  //頂点aが所属するグループ
  T root(T a) {
    if (uni[a] < 0) {
      return a;
    }
    return uni[a] = root(uni[a]);
  }

  // a,bをつなげる
  bool connect(T a, T b) {
    a = root(a);
    b = root(b);
    if (a == b) {
      return false;
    }

    if (uni[a] > uni[b]) {
      std::swap(a, b);
    }

    uni[a] += uni[b];
    uni[b] = a;
    return true;
  }

  // a,bが同グループか確認
  bool isConnect(T a, T b) { return root(a) == root(b); }

  // aが属するグループの要素数
  T size(T a) { return -uni[root(a)]; }

  //グループ数を返す
  T groups() {
    std::set<T> b;
    for (T i = 0; i < num; i++) {
      b.insert(root(i));
    }
    return b.size();
  }
};


