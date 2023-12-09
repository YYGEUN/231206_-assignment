#include "chatting.h"

// 다른 IP

using namespace std;
string n;
string IP;
int main(){
  int a = 0;
  cout << "PORT : " ;
  cin >> a;
  cout << "NAME : " ;
  cin >> n;
  cout << "IP : " ;
  cin >> IP;
  string name = n;
  CHAT_Client chat3(name,IP,a);
}
