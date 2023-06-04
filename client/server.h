#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <csignal>
#include <string>

using namespace std;

class SocketHandler{
    struct sockaddr_in address;
    struct sockaddr_in serv_addr;
    int sock;
    char buffer[1024];
    fd_set set;
    struct timeval timeout;

public:
    SocketHandler();
    void sendData(string);
    void createClientSocket();
    void initServerAddress(string, int);
    int connectToServer(string, int);
    void closeSocket();
    int getSock() const;
    char* getBuffer();
    fd_set& getSet();
    timeval& getTimeout();
};