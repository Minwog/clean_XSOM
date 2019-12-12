#include <iostream>
#include <random>
using namespace std;

void random_number(random_device& rd){
  mt19937 mt(rd());
  uniform_int_distribution<int> dist(0,99);
 for (int i = 0; i< 16; ++i){
    cout<<dist(mt)<<" ";
 }
 cout<<endl;
}

int main(){
 random_device rd;
 random_number(rd);
 random_number(rd);
}
