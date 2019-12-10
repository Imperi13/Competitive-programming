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

using ll = long long;

//非再帰抽象セグ木 0-indexed 単位元はiで指定
template <class T>
class SegmentTree {
 private:
  long long n;
  T init;
  std::vector<T> dat;
  std::function<T(T, T)> fn;

 public:
  SegmentTree(long long i, T para, std::function<T(T, T)> fun)
      : init(para), fn(fun) {
    n = 1;
    while (n < i) {
      n *= 2;
    }
    dat = std::vector<T>(2 * n - 1, init);
  }

  SegmentTree(const std::vector<T>& seq,T para,std::function<T(T,T)> fun)
      : init(para),fn(fun){
    n=1;
    while(n<seq.size())n<<=1;
    dat=std::vector<T>(2*n-1,init);
    for(long long i=0;i<seq.size();i++)dat[i+n-1]=seq[i];
    for(long long i=n-2;i>=0;i--)dat[i]=fn(dat[i*2+1],dat[i*2+2]);
  }

  // k番目(0-indexed)を値aで更新,dest=trueのときは更新前を破壊して初期化する
  void update(long long k, T a, bool dest) {
    assert(0<=k&&k<n);
    k += n - 1;
    if (dest) dat[k] = a;
    else dat[k] = fn(dat[k], a);

    while (k > 0) {
      k = (k - 1) / 2;
      dat[k] = fn(dat[k * 2 + 1], dat[k * 2 + 2]);
    }
  }

  //[a,b)の値を返す
  T query(long long a, long long b){
    assert(0<=a&&b<=n);
    T val=init;

    long long l=a+n-1,r=b+n-1;
    for(;l<r;l=(l>>1),r=(r>>1)){
      if(!(r&1)){
        r--;
        val=fn(val,dat[r]);
      }
      if(!(l&1)){
        val=fn(val,dat[l]);
        l++;
      }
    }

    return val;
  }
};

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

//動的セグ木 
template<typename T>
class DynamicSegmentTree{
  private:

  long long n,n0;
  std::function<T(T,T)> fn;
  T init;

  class Node{
    public:
    std::shared_ptr<Node> left,right;
    std::weak_ptr<Node> par;
    T value;

    Node(T i,const std::shared_ptr<Node>& par_):value(i),left(),right(){
      par=par_;
    }

    Node(T i):value(i),left(),right(),par(){}
  };

  std::shared_ptr<Node> root;

  T query(long long a,long long b,const std::shared_ptr<Node>& now,long long l,long long r){
    if(a<=l&&r<=b){
      return now->value;
    }
    if(b<=l||r<=a){
      return init;
    }

    T lval=(now->left)?query(a,b,now->left,l,l+(r-l)/2):init;
    T rval=(now->right)?query(a,b,now->right,l+(r-l)/2,r):init;

    return fn(lval,rval);
  }

  public:

  //要素数の最大値n,単位元i,演算fを渡す
  DynamicSegmentTree(long long n_,T i,std::function<T(T,T)> f):n(n_),init(i),fn(f),root(new Node(init)){
    n0=1;
    while(n0<n_)n0<<=1;
  }

  // 更新 Ai=fn(Ai,val) を行う,dest=trueのときは初期化
  void update(long long i,T val,bool dest){
    assert(0<=i&&i<n);
    std::shared_ptr<Node> now(root);

    long long l=0,r=n0;
    while(r-l>1){
      long long mid=l+(r-l)/2;

      if(i<mid){
        if(!now->left){
          now->left=std::make_shared<Node>(init,now);
        }

        now=now->left;

        r=mid;
      }else{
        if(!now->right){
          now->right=std::make_shared<Node>(init,now);
        }

        now=now->right;

        l=mid;
      }
    }

    if(dest)now->value=val;
    else now->value=fn(now->value,val);

    while(now->par.lock()){
      now=now->par.lock();
      now->value=fn((now->left)?now->left->value:init,(now->right)?now->right->value:init);
    }
  }

  //[a,b)の区間演算結果を返す
  T query(long long a,long long b){
    assert(0<=a&&b<=n);
    return query(a,b,root,0,n0);
  }
};

//LiChaoSegmentTree
//minを返すCHTができる 線分追加に対応した
template<typename T,T id>
class LiChaoSegmentTree{

  struct Line{
    std::pair<T,T> coeffi;

    Line(std::pair<T,T> a):coeffi(a){}

    T value(T x){
      return coeffi.first*x+coeffi.second;
    }
  };

  struct Node{

    std::shared_ptr<Node> left,right;
    Line line;

    Node():line(std::pair<T,T>(0,id)),left(),right(){}

    Node(Line line_):line(line_),left(),right(){}
  };

  private:

  long long n,n0;
  std::vector<T> num;

  std::shared_ptr<Node> root;

  void eval(Line f,long long a,long long b,std::shared_ptr<Node> now,long long l,long long r){

    if(a==b)return;

    if(r-l>1){
      if(f.value(num[l])>=now->line.value(num[l])&&f.value(num[std::min(n,r)-1])>=now->line.value(num[std::min(n,r)-1])){
        return;
      }

      long long mid=l+(r-l)/2;

      if(a<=l&&r<=b){

        if(n<=mid){
          if(now->line.value(num[n-1]) > f.value(num[n-1])){
            std::swap(now->line,f);
          }

          if(!now->left)now->left=std::make_shared<Node>();
          eval(f,a,b,now->left,l,mid);
        }else{
          if(now->line.value(num[mid-1]) > f.value(num[mid-1])){
            std::swap(now->line,f);
          }

          if(now->line.value(num[l]) > f.value(num[l])){
            if(!now->left)now->left=std::make_shared<Node>();
            eval(f,a,b,now->left,l,mid);
          }else{
            if(!now->right)now->right=std::make_shared<Node>();
            eval(f,a,b,now->right,mid,r);
          }
        }
      }else{
        if(b>l&&mid>a){
          if(!now->left)now->left=std::make_shared<Node>();
          eval(f,a,b,now->left,l,mid);
        }
        if(b>mid&&r>a){
          if(!now->right)now->right=std::make_shared<Node>();
          eval(f,a,b,now->right,mid,r);
        }
      }
    }else{
      if(now->line.value(l) > f.value(l)){
        now->line=f;
        return;
      }
    }
  }

  public:

  LiChaoSegmentTree(long long n_):n(n_),root(new Node()),num(std::vector<T>(n_,0)){
    for(ll i=1;i<n_;i++)num[i]=num[i-1]+1;

    n0=1;
    while(n0<n_)n0<<=1;
  }

  LiChaoSegmentTree(const std::vector<T>& num_):n(num_.size()),root(new Node()),num(num_){
    n0=1;
    while(n0<n)n0<<=1;
  }



  void addline(std::pair<T,T> line_){
    Line f=Line(line_);
    eval(f,0,n0,root,0,n0);
  }

  void addsegment(std::pair<T,T> line_,T a,T b){
    Line f=Line(line_);
    long long first=(std::lower_bound(all(num),a)-num.begin());
    long long second=(std::upper_bound(all(num),b)-num.begin());
    eval(f,first,second,root,0,n0);
  }

  T query(T val){
    long long i=std::lower_bound(all(num),val)-num.begin();
    std::shared_ptr<Node> now(root);

    T ans=now->line.value(num[i]);

    long long l=0,r=n0;
    while(r-l>1){
      if(ans > now->line.value(num[i]))ans=now->line.value(num[i]);
      long long mid=l+(r-l)/2;

      if(i<mid){
        r=mid;
        if(!now->left)return ans;

        now=now->left;
      }else{
        l=mid;
        if(!now->right)return ans;

        now=now->right;
      }

    }

    if(now->line.value(num[i]) < ans)ans=now->line.value(num[i]);

    return ans;
  }
};

//SWAG スライド区間上の演算ができる
template<typename T>
class SWAG{
  private:

  std::function<T(T,T)> fn;

  std::stack<T> front,back;
  std::vector<T> front_fold,back_fold;

  T id;

  public:

  SWAG(T id_,std::function<T(T,T)> fn_):id(id_),fn(fn_),front_fold(1,id_),back_fold(1,id_){}

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

//RBST merge/split based 
// 数列操作
template<typename T>
class RBST{
  private:

  class Xorshift{
    private:
    unsigned int x,y,z,w;

    public:
    Xorshift():x(123456789),y(362436069),z(521288629),w(88675123){}

    unsigned int operator() () {
      unsigned int t=(x^(x<<11));
      x=y;y=z;z=w;
      return w=(w^(w>>19))^(t^(t>>8));
    }
  };

  Xorshift rnd;

  class Node;

  using node_ptr=std::shared_ptr<Node>;
  using F=std::function<T(T,T)>;

  struct Node{
    node_ptr left,right;
    T val,sum;
    int cnt;
    Node(T val_):val(val_),sum(val_),cnt(1),left(),right(){}
  };

  node_ptr root;
  F fn;
  T id;

  T calc(node_ptr t){return t?t->sum:id;}
  int count(node_ptr t){return t?t->cnt:0;}

  node_ptr update(node_ptr t){
    if(!t)return t;
    t->sum=fn(fn(calc(t->left),t->val),calc(t->right));
    t->cnt=count(t->left)+count(t->right)+1;
    return t;
  }

  node_ptr merge(node_ptr l,node_ptr r){
    if(!l||!r)return l?l:r;

    if(rnd()%(count(l)+count(r))<count(l)){
      l->right=merge(l->right,r);
      return update(l);
    }else{
      r->left=merge(l,r->left);
      return update(r);
    }
  }

  std::pair<node_ptr,node_ptr> split(node_ptr t,int k){
    if(!t)return std::make_pair(t,t);

    if(k<=count(t->left)){
      auto temp=split(t->left,k);
      t->left=temp.second;
      return std::make_pair(temp.first,update(t));
    }else{
      auto temp=split(t->right,k-count(t->left)-1);
      t->right=temp.first;
      return std::make_pair(update(t),temp.second);
    }
  }

  void insert(node_ptr& t,node_ptr newnode,int k){
    auto temp=split(t,k);
    t=merge(merge(temp.first,newnode),temp.second);
  }

  void erase(node_ptr& t,int k){
    auto temp=split(t,k+1);
    auto temp2=split(temp.first,k);
    t=merge(temp2.first,temp.second);
  }

  public:

  RBST(T id_,F fn_):id(id_),fn(fn_),root(){};

  int size(){return count(root);}

  void insert(int k,T val){
    insert(root,std::make_shared<Node>(val),k);
  }

  void erase(int k){
    erase(root,k);
  }

  T query(){
    return calc(root);
  }

  T query(int a,int b){
    auto temp=split(root,b);
    auto temp2=split(temp.first,a);
    T ret=calc(temp2.second);

    root=merge(merge(temp2.first,temp2.second),temp.second);

    return ret;
  }
};

//とりあえず1-indexedでつくったBIT
template <class T>
class BinaryIndexedTree {
  private:
  T n;
  std::vector<T> bit;

  public:
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

    //各項がすべて非負のものに対して、sum[1,a]>=keyを満たす最小のaを返す 存在しないなら-1返す
  T lower_bound(T key){
    T len=1,temp=n,now=0,value=0,flag;
    while(temp/2!=0){
      len*=2;
      temp/=2;
    }

    while(len>0){
      while(now+len>n)len/=2;

      if(value+bit[now+len]>=key){
        len/=2;
      }else{
        now+=len;
        value+=bit[now];
        len/=2;
        if(now==n){
          return -1;
        }
      }
    }

    now++;
    return now;
  }
};

// UnionFind
class UnionFind {
  private:
  std::vector<long long> uni;
  long long num;

  public:
  UnionFind(long long s) : uni(s, -1), num(s) {}

  //頂点aが所属するグループ
  long long root(long long a) {
    if (uni[a] < 0) {
      return a;
    }
    return uni[a] = root(uni[a]);
  }

  // a,bをつなげる
  bool connect(long long a, long long b) {
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
  bool isConnect(long long a, long long b) { return root(a) == root(b); }

  // aが属するグループの要素数
  long long size(long long a) { return -uni[root(a)]; }

  //グループ数を返す
  long long groups() {
    std::set<long long> b;
    for (long long i = 0; i < num; i++) {
      b.insert(root(i));
    }
    return b.size();
  }
};