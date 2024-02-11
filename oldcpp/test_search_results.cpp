#include "search_results.hpp"
#include <vector>
#include <cassert>

int main()
{
  std::vector<int> test(10);
  for (int i=0;i<10;i++) {
    test[i]=i*10;
  }



{
  search_results<int> sr(test.size());
  assert(sr.empty());
  assert(sr.size()==0);
  sr.push_interval(std::make_pair(&*test.begin(),&*test.begin()+2));
  assert(!sr.empty());
  assert(sr.size()==2);
  auto it=sr.begin();
  assert(*it==0);
  assert(*++it==10);
  assert(++it==sr.end());
  --it;
  sr.push_interval(std::make_pair(&*test.begin()+1,&*test.begin()+2));
  assert(sr.size()==2);
  sr.push_interval(std::make_pair(&*test.begin()+1,&*test.begin()+4));
  assert(sr.size()==4);
  ++it;
  assert(*it==20);
  assert(*++it==30);
  sr.push_interval(std::make_pair(&*test.begin()+7,&*test.begin()+10));
  assert(sr.size()==7);
  assert(*++it==70);
  assert(*++it==80);
  assert(*++it==90);
  assert(++it==sr.end());
  for (auto & val : sr) {
    val=-val;
  }
}
  assert(test[0]==0);
  assert(test[1]==-10);
  assert(test[2]==-20);
  assert(test[3]==-30);
  assert(test[4]==40);
  assert(test[5]==50);
  assert(test[6]==60);
  assert(test[7]==-70);
  assert(test[8]==-80);
  assert(test[9]==-90);
}
