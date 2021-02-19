#include "UDPConnection.h"

namespace pipert {

    UDPConnection::UDPConnection(std::uint16_t port_, char *ip_address_) :port_(port_),ip_address_(ip_address_){}
    UDPConnection::~UDPConnection() {}
    void UDPConnection::openCoccection() {
        opened_=true;
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("socket creation failed");
            opened_=false;
    }
        memset(&servaddr_, 0, sizeof(servaddr_));

        /// Filling server information
        servaddr_.sin_family = AF_INET;
        servaddr_.sin_port = htons(port_);
        servaddr_.sin_addr.s_addr =inet_addr(ip_address_);
    }

    void UDPConnection::send(std::uint8_t * buffer_) {
        if(opened_) {
            ///send the buffer to the UDP connection
            sendto(sockfd, buffer_, sizeof(*buffer_) / sizeof(buffer_[0]), MSG_CONFIRM,
                   (struct sockaddr *)&servaddr_, sizeof(servaddr_));
        }
    }

    void UDPConnection::send(std::uint8_t * buffer_,std::uint8_t size_) {
        if(opened_) {
            ///send the buffer to the UDP connection
            sendto(sockfd, buffer_, size_, MSG_CONFIRM,
                   (struct sockaddr *)&servaddr_, sizeof(servaddr_));
        }
    }

    void UDPConnection::closeConnection() {
        /// close connection
        close(sockfd);
        opened_=false;
    }
}  // namespace pipert
