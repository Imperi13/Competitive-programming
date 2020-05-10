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

//体の行列クラス
/*必須インターフェース

ゼロ元:0
単位元:1
四則演算: 演算子オーバーロードで+,-,*,/を用意する
等号: 演算子オーバーロードで ==,!=を用意する

show関数は正直デバッグ用なのでインターフェースとかを考えていない
ストリーム演算子を用意すれば普通に表示できる

*/
template<typename Field,std::size_t ROW,std::size_t COLUMN>
class Mat{
  private:

  public:

  std::vector<std::vector<Field>> a;

  explicit Mat(const Field init=0):a(ROW,std::vector<Field>(COLUMN,init)){}
  Mat(const std::vector<std::vector<Field>>& a_):a(a_){}

  constexpr Field& operator()(std::size_t i,std::size_t j){return a[i][j];}
  constexpr static std::size_t get_row(){return ROW;}
  constexpr static std::size_t get_column(){return COLUMN;}
  constexpr bool operator==(const Mat rhs){
    for(std::size_t i=0;i<ROW;i++){
      for(std::size_t j=0;j<COLUMN;j++){
        if(a[i][j]!=rhs.a[i][j])return false;
      }
    }
    return true;
  }
  constexpr bool operator!=(const Mat rhs){return !(*this==rhs);}

  constexpr Mat operator+(const Mat rhs){
    return Mat(*this)+=rhs;
  }

  constexpr Mat operator-(const Mat rhs){
    return Mat(*this)-=rhs;
  }

  template<std::size_t NEWCOLUMN>
  constexpr Mat<Field,ROW,NEWCOLUMN> operator*(const Mat<Field,COLUMN,NEWCOLUMN> rhs){
    Mat<Field,ROW,NEWCOLUMN> tmp;
    for(std::size_t r=0;r<ROW;r++){
      for(std::size_t c=0;c<NEWCOLUMN;c++){
        for(std::size_t i=0;i<COLUMN;i++){
          tmp.a[r][c]+=a[r][i]*rhs.a[i][c];
        }
      }
    }
    return tmp;
  }

  constexpr Mat& operator+=(const Mat<Field,ROW,COLUMN> rhs){
    for(std::size_t i=0;i<ROW;i++){
      for(std::size_t j=0;j<COLUMN;j++){
        a[i][j]+=rhs.a[i][j];
      }
    }
    return *this;
  }

  constexpr Mat& operator-=(const Mat<Field,ROW,COLUMN> rhs){
    for(std::size_t i=0;i<ROW;i++){
      for(std::size_t j=0;j<COLUMN;j++){
        a[i][j]-=rhs.a[i][j];
      }
    }
    return *this;
  }

  constexpr Mat& operator*=(const Mat<Field,COLUMN,COLUMN> rhs){
    std::vector<std::vector<Field>> tmp(ROW,std::vector<Field>(COLUMN,0));
    for(std::size_t r=0;r<ROW;r++){
      for(std::size_t c=0;c<COLUMN;c++){
        for(std::size_t i=0;i<COLUMN;i++){
          tmp[r][c]+=a[r][i]*rhs.a[i][c];
        }
      }
    }
    a=std::move(tmp);
    return *this;
  }

  constexpr static Mat id(){
    static_assert(ROW==COLUMN,"ROW must be equal to COLUMN");
    Mat<Field,ROW,COLUMN> temp;
    for(std::size_t i=0;i<ROW;i++)temp.at(i,i)=1;
    return temp;
  }

  //PA==LUとなる置換行列P,下三角L,上三角Uを求める 任意の正方行列に使える
  constexpr void LUPdecomposition(Mat& P,Mat& L,Mat& U){
    static_assert(ROW==COLUMN,"ROW must be equal to COLUMN");
    std::vector<std::vector<Field>> tmp=a;
    P=Mat::id();L=Mat(0);U=Mat(0);

    for(std::size_t i=0;i<ROW;i++){
      //ピボット選択
      for(std::size_t j=i;j<ROW;j++){
        Field val=0;
        for(std::size_t k=0;k<i;k++)val+=L.a[j][k]*U.a[k][i];
        if(val!=tmp[j][i]){
          std::swap(tmp[i],tmp[j]);
          std::swap(P.a[i],P.a[j]);
          std::swap(L.a[i],L.a[j]);
          break;
        }
      }

      L.a[i][i]=1;

      for(std::size_t j=i;j<ROW;j++){
        U.a[i][j]=tmp[i][j];
        for(std::size_t k=0;k<i;k++)U.a[i][j]-=L.a[i][k]*U.a[k][j];
      }

      if(U.a[i][i]!=0){
        for(std::size_t j=i+1;j<ROW;j++){
          L.a[j][i]=tmp[j][i];
          for(std::size_t k=0;k<i;k++)L.a[j][i]-=L.a[j][k]*U.a[k][i];
          L.a[j][i]/=U.a[i][i];
        }
      }
    }
  }

  constexpr Field det(){
    Mat p,l,u;
    LUPdecomposition(p,l,u);
    Field ans=1;
    for(std::size_t i=0;i<ROW;i++)ans*=u.a[i][i];
    return ans;
  }

  //A*ans=bとなるベクトルansを求める 解なしのときはfalseを返す
  constexpr bool linear_equation(Mat<Field,ROW,1> b,Mat<Field,ROW,1>& ans){
    Mat p,l,u;
    LUPdecomposition(p,l,u);

    b=p*b;
    
    for(std::size_t i=1;i<ROW;i++){
      for(std::size_t j=0;j<i;j++)b.a[i][0]-=l.a[i][j]*b.a[j][0];
    }

    //solve U*ans=b

    for(long long i=ROW-1;i>=0;i--){
      for(std::size_t j=i+1;j<ROW;j++)b.a[i][0]-=u.a[i][j]*b.a[j][0];
      if(b.a[i][0]!=0){
        if(u.a[i][i]==0)return false;
        b.a[i][0]/=u.a[i][i];
      }
    }
    ans=std::move(b);
    return true;
  }

  void show(){
    std::cout<<"-------------------------------------\n";
    for(std::size_t i=0;i<ROW;i++){
      for(std::size_t j=0;j<COLUMN;j++){
        std::cout<<a[i][j]<<" ";
      }
      std::cout<<std::endl;
    }
  }
};

//体の行列クラス Array Version
/*必須インターフェース

ゼロ元:0
単位元:1
四則演算: 演算子オーバーロードで+,-,*,/を用意する
等号: 演算子オーバーロードで ==,!=を用意する

show関数は正直デバッグ用なのでインターフェースとかを考えていない
ストリーム演算子を用意すれば普通に表示できる

*/
template<typename Field,std::size_t ROW,std::size_t COLUMN>
class Mat{
  private:

  public:

  std::array<std::array<Field,COLUMN>,ROW> a;

  explicit Mat(const Field init=0){
    std::fill(a[0].begin(),a[ROW-1].end(),init);
  }

  Mat(const std::array<std::array<Field,COLUMN>,ROW>& a_):a(a_){}

  constexpr Field& operator()(std::size_t i,std::size_t j){return a[i][j];}
  constexpr static std::size_t get_row(){return ROW;}
  constexpr static std::size_t get_column(){return COLUMN;}
  constexpr bool operator==(const Mat rhs){
    for(std::size_t i=0;i<ROW;i++){
      for(std::size_t j=0;j<COLUMN;j++){
        if(a[i][j]!=rhs.a[i][j])return false;
      }
    }
    return true;
  }
  constexpr bool operator!=(const Mat rhs){return !(*this==rhs);}

  constexpr Mat operator+(const Mat rhs){
    return Mat(*this)+=rhs;
  }

  constexpr Mat operator-(const Mat rhs){
    return Mat(*this)-=rhs;
  }

  template<std::size_t NEWCOLUMN>
  constexpr Mat<Field,ROW,NEWCOLUMN> operator*(const Mat<Field,COLUMN,NEWCOLUMN> rhs){
    Mat<Field,ROW,NEWCOLUMN> tmp;
    for(std::size_t r=0;r<ROW;r++){
      for(std::size_t c=0;c<NEWCOLUMN;c++){
        for(std::size_t i=0;i<COLUMN;i++){
          tmp.a[r][c]+=a[r][i]*rhs.a[i][c];
        }
      }
    }
    return tmp;
  }

  constexpr Mat& operator+=(const Mat<Field,ROW,COLUMN> rhs){
    for(std::size_t i=0;i<ROW;i++){
      for(std::size_t j=0;j<COLUMN;j++){
        a[i][j]+=rhs.a[i][j];
      }
    }
    return *this;
  }

  constexpr Mat& operator-=(const Mat<Field,ROW,COLUMN> rhs){
    for(std::size_t i=0;i<ROW;i++){
      for(std::size_t j=0;j<COLUMN;j++){
        a[i][j]-=rhs.a[i][j];
      }
    }
    return *this;
  }

  constexpr Mat& operator*=(const Mat<Field,COLUMN,COLUMN> rhs){
    std::array<std::array<Field,COLUMN>,ROW> tmp;
    for(std::size_t r=0;r<ROW;r++){
      for(std::size_t c=0;c<COLUMN;c++){
        for(std::size_t i=0;i<COLUMN;i++){
          tmp[r][c]+=a[r][i]*rhs.a[i][c];
        }
      }
    }
    a=std::move(tmp);
    return *this;
  }

  constexpr static Mat id(){
    static_assert(ROW==COLUMN,"ROW must be equal to COLUMN");
    Mat<Field,ROW,COLUMN> temp;
    for(std::size_t i=0;i<ROW;i++)temp.at(i,i)=1;
    return temp;
  }

  //PA==LUとなる置換行列P,下三角L,上三角Uを求める 任意の正方行列に使える
  constexpr void LUPdecomposition(Mat& P,Mat& L,Mat& U){
    static_assert(ROW==COLUMN,"ROW must be equal to COLUMN");
    std::array<std::array<Field,COLUMN>,ROW> tmp=a;
    P=Mat::id();L=Mat(0);U=Mat(0);

    for(std::size_t i=0;i<ROW;i++){
      //ピボット選択
      for(std::size_t j=i;j<ROW;j++){
        Field val=0;
        for(std::size_t k=0;k<i;k++)val+=L.a[j][k]*U.a[k][i];
        if(val!=tmp[j][i]){
          std::swap(tmp[i],tmp[j]);
          std::swap(P.a[i],P.a[j]);
          std::swap(L.a[i],L.a[j]);
          break;
        }
      }

      L.a[i][i]=1;

      for(std::size_t j=i;j<ROW;j++){
        U.a[i][j]=tmp[i][j];
        for(std::size_t k=0;k<i;k++)U.a[i][j]-=L.a[i][k]*U.a[k][j];
      }

      if(U.a[i][i]!=0){
        for(std::size_t j=i+1;j<ROW;j++){
          L.a[j][i]=tmp[j][i];
          for(std::size_t k=0;k<i;k++)L.a[j][i]-=L.a[j][k]*U.a[k][i];
          L.a[j][i]/=U.a[i][i];
        }
      }
    }
  }

  constexpr Field det(){
    Mat p,l,u;
    LUPdecomposition(p,l,u);
    Field ans=1;
    for(std::size_t i=0;i<ROW;i++)ans*=u.a[i][i];
    return ans;
  }

  //A*ans=bとなるベクトルansを求める 解なしのときはfalseを返す
  constexpr bool linear_equation(Mat<Field,ROW,1> b,Mat<Field,ROW,1>& ans){
    Mat p,l,u;
    LUPdecomposition(p,l,u);

    b=p*b;
    
    for(std::size_t i=1;i<ROW;i++){
      for(std::size_t j=0;j<i;j++)b.a[i][0]-=l.a[i][j]*b.a[j][0];
    }

    //solve U*ans=b

    for(long long i=ROW-1;i>=0;i--){
      for(std::size_t j=i+1;j<ROW;j++)b.a[i][0]-=u.a[i][j]*b.a[j][0];
      if(b.a[i][0]!=0){
        if(u.a[i][i]==0)return false;
        b.a[i][0]/=u.a[i][i];
      }
    }
    ans=std::move(b);
    return true;
  }

  void show(){
    debug("show");
    std::cout<<"-------------------------------------\n";
    for(std::size_t i=0;i<ROW;i++){
      for(std::size_t j=0;j<COLUMN;j++){
        std::cout<<a[i][j]<<" ";
      }
      std::cout<<std::endl;
    }
  }
};