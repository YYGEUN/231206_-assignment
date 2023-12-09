#include "chatting.h"

using namespace std;
string n;
int main(){
  int a = 0;
  cout << "PORT : " ;
  cin >> a;
  cout << "NAME : " ;
  cin >> n;
  string name = n;
  CHAT_Client chat2(name,a);
}
