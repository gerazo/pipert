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
      template <class  T>
      string Serialize(MeasurementProfileBase* profileBase) {
    if (profileBase->IsOptionalUserProfile) {


      return SerializeUserMeasurementProfile<T>(profileBase);
    }

      else
    {
        return SerializeGeneralMeasurementProfile(profileBase);

    }

    return "";
  }

  template <class  T>
  string SerializeUserMeasurementProfile(pipert::MeasurementProfileBase* profileBase) {

    pipert::UserMeasurementProfile<T>* measurement_profile_ = (pipert::UserMeasurementProfile<T>*) (profileBase);
    string serializedMessage = "start,1,";
    serializedMessage.append(measurement_profile_->MesurementProfileName);
    serializedMessage.append(",");
    for(pipert::MeasurementEvent e : measurement_profile_->MeasurementsEventsLog){
      serializedMessage.append("EventStart");
      serializedMessage.append(",");
      serializedMessage.append(std::to_string(e.EventTime));
      serializedMessage.append(",");
      serializedMessage.append(std::to_string(e.ProcessStatus));
      serializedMessage.append(",");
      serializedMessage.append(std::to_string(*e.ThreadID));
      serializedMessage.append(",");
      serializedMessage.append(e.ChannelName);
      serializedMessage.append(",");
      serializedMessage.append("EventEnd");
      serializedMessage.append(",");

    }
    serializedMessage.append("ResultStart,");
    for(auto elem : measurement_profile_->getResult())
    {

      serializedMessage.append(elem.first );
      serializedMessage.append(",");
      serializedMessage.append(elem.second);
      serializedMessage.append(",");
    }
    serializedMessage.append("ResultStart,");
    serializedMessage.append("end");
    return serializedMessage;
  }




  string SerializeGeneralMeasurementProfile(
      pipert::MeasurementProfileBase* profileBase) {
    pipert::MeasurementProfile* measurement_profile_ = (pipert::MeasurementProfile*) (profileBase);
    string serializedMessage = "start,0,";
    serializedMessage.append(measurement_profile_->MesurementProfileName);
    serializedMessage.append(",");
    for(pipert::MeasurementEvent e : measurement_profile_->MeasurementsEventsLog){
      serializedMessage.append("EventStart");
      serializedMessage.append(",");
      serializedMessage.append(std::to_string(e.EventTime));
      serializedMessage.append(",");
      serializedMessage.append(std::to_string(e.ProcessStatus));
      serializedMessage.append(",");
      serializedMessage.append(std::to_string(*e.ThreadID));
      serializedMessage.append(",");
      serializedMessage.append(e.ChannelName);
      serializedMessage.append(",");
      serializedMessage.append("EventEnd");
      serializedMessage.append(",");

    }
    serializedMessage.append("end");
    return serializedMessage;
  }

 public:
  UDPConnection() {}
  template <class  T>
  void send(std::vector<pipert::MeasurementProfileBase*> measurements) {
    UNUSED(measurements);
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
    int measurementsSize = measurements.size();
    for (int i = 0; i < measurementsSize; i++) {
      message = Serialize<T>(measurements.at(i));
      sendto(sockfd, (const char *)message.c_str(), message.length(),
             MSG_CONFIRM, (struct sockaddr *)&servaddr, sizeof(servaddr));
    }

    close(sockfd);
  }

}; 
}  // namespace pipert
