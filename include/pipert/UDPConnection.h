#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <vector>

#include "pipert/MeasurementProfile.h"
#include "pipert/UserMeasurementProfile.h"
#include "pipert/MeasurementProfileBase.h"
#include "pipert/MeasurementEvent.h"
using namespace std;
using std::vector;

#define PORT 8080
#define MAXLINE 1024
#define UNUSED(x) (void)(x)

namespace pipert {

class UDPConnection {
 private:




 public:
  UDPConnection() {}

  void send(std::vector<string> messages) {

    int sockfd;
    //   char buffer[MAXLINE];
    string message = "";
    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      perror("socket creation failed");
      exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    //  int n;
    // socklen_t len;
    int measurementsSize = messages.size();
    for (int i = 0; i < measurementsSize; i++) {
      message = messages.at(i);
      sendto(sockfd, (const char *)message.c_str(), message.length(),
             MSG_CONFIRM, (struct sockaddr *)&servaddr, sizeof(servaddr));
    }

    close(sockfd);
  }

}; 
}  // namespace pipert
