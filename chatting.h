#include <iostream>
#include <unistd.h>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/procfs.h>
#include <sys/sem.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h> 
#include <ctime>
#include <iomanip>
// #include <Windows.h>
#include <fstream>

using namespace std;

#define MAX_LISTEN_COUNT 5
#define MAX_CLIENT 2

class CHAT_Server{
  struct sockaddr_in client_addr,server_addr;	
  socklen_t client_addr_size;
  pthread_t pt[MAX_CLIENT+1];
  string IP,msg;
  int status,PORT,client_socket[MAX_CLIENT],server_socket,client_number;

public:

  CHAT_Server(string ip,int port)
  :IP(ip),PORT(port),client_number(0)
  { 
    server_socket = socket(AF_INET,SOCK_STREAM,0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if(bind(server_socket,(const struct sockaddr*)&server_addr,sizeof(server_addr))<0)
    {
      perror("바인딩 실패");
      return;
    }
    printf("바인딩 ----\n");

    listen(server_socket,5);
    printf("리슨 ----\n");

    pthread_create(&pt[MAX_CLIENT], nullptr, &CHAT_Server::sendThread, this);

    while(1)
    {
      client_addr_size = sizeof(client_addr_size);
      client_socket[client_number] = accept(server_socket,(struct sockaddr*)&client_addr,&client_addr_size);
      if(client_socket[client_number] < 0){
        printf("\n클라이언트소켓 accept Fail\n");
        continue;
      }
      cout << "--------accept client --------" << endl;
      pthread_create(&pt[client_number], nullptr, &CHAT_Server::recvThread, (void*)&client_socket[client_number]);

      printf("현재 연결된 클라이언트 수 : %d\n",++client_number);
      if(client_number == MAX_CLIENT){
        cout << "최대 연결 됨" << endl;
        break;
      }
    }

    for(int i=1;i<=client_number;i++){
      pthread_join(pt[i],(void**)&status);
    }
  }

  static void* sendThread(void* arg) {
      CHAT_Server* chat = static_cast<CHAT_Server*>(arg);
      return nullptr;
  }

  static void* recvThread(void* arg) {
      // CHAT_Server* chat = static_cast<CHAT_Server*>(arg);
      int *socket = (int*)arg;
      while (1) {
          string receivedMsg = recvmsg(*socket);
          if(receivedMsg.size() > 0){
            time_t currentTime;
            time(&currentTime);
            tm* localTime = localtime(&currentTime);
            cout << *socket << "Received message: " << receivedMsg << "(" << put_time(localTime,"%Y-%m-%d %H:%M:%S") 
            << ")" << endl;
          }
      }

      return nullptr;
  }

  void sendmsg(int socket, const string &msg) {
      // 문자열의 길이를 먼저 전송
      size_t length = msg.size();
      send(socket, &length, sizeof(length), 0);

      // 실제 문자열을 전송
      send(socket, msg.c_str(), length, 0);
  }

  static string recvmsg(int socket) {
      // 문자열의 길이를 먼저 수신
      size_t length;
      recv(socket, &length, sizeof(length), 0);
      if(length < 0 ) return NULL;

      // 실제 문자열을 수신
      char buffer[length + 1];  // +1은 문자열 끝에 널 문자('\0')를 추가하기 위함
      recv(socket, buffer, length, 0);
      buffer[length] = '\0';  // 문자열 끝에 널 문자('\0') 추가

      return string(buffer);
  }

  void Window(){
      while(1){
        cout << "▶ " << "클라 입력 : ";
        cin >> msg;
        if(msg == "end") break;
      }
    }
};

class CHAT_Client{
  struct sockaddr_in client_addr,server_addr;	
  socklen_t client_addr_size;
  pthread_t pt[2];
  string IP,msg;
  int client_socket,server_socket,PORT;
public:

  CHAT_Client(string ip,int port)
  :IP(ip),PORT(port),server_socket(0)
  { 
    client_socket = socket(AF_INET,SOCK_STREAM,0);
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    client_addr.sin_port = htons(PORT);

    connect(client_socket,(const struct sockaddr*)&client_addr,sizeof(client_addr));

    Window();
  }

  void sendmsg(int socket, const string &msg) {
      // 문자열의 길이를 먼저 전송
      size_t length = msg.size();
      send(socket, &length, sizeof(length), 0);
      if(length <= 0) return;
      // 실제 문자열을 전송
      send(socket, msg.c_str(), length, 0);
      ofstream ou("user.txt", ios::app);
      savemsg(&ou,msg);
  }

  void roadmsg(){
    string logfilename = "user.txt";
    ifstream iu(logfilename);
    string logmsg;
    while (getline(iu, logmsg)) {
      cout << logmsg << endl;
    }

    iu.close();
  }
  void savemsg(ofstream* ou,const string &msg)
  {
    if (ou->is_open()) {
      *ou << msg << endl;
    }
  }
  void Window(){
    while(1){
      cout << "▶ 클라 입력 : ";
      cin >> msg;
      if(msg == "end") break;
      if(msg == "print"){
        roadmsg();
        continue;
      }
      size_t length = msg.size();
      sendmsg(client_socket, msg);
    }
  }
};
