#include "UDPConnection.h"

namespace pipert {

    UDPConnection::UDPConnection(std::uint16_t port_, char *ip_address_) :port_(port_),ip_address_(ip_address_){}
    UDPConnection::~UDPConnection() {closeConnection();}
    void UDPConnection::openCoccection() {
        opened_=true;
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("socket creation failed");
            opened_=false;
    }
        memset(&servaddr, 0, sizeof(servaddr));

        /// Filling server information
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(PORT);
        servaddr.sin_addr.s_addr =inet_addr(ip_address_);
    }

    void UDPConnection::send(std::uint8_t * buffer) {
        if(opened_) {
            ///send the buffer to the UDP connection
            sendto(sockfd, buffer, sizeof(buffer) / sizeof(buffer[0]), MSG_CONFIRM,
                   (struct sockaddr *)&servaddr, sizeof(servaddr));
        }
    }

    void UDPConnection::closeConnection() {
        /// close connection
        close(sockfd);
        opened_=false;
    }
}  // namespace pipert
#endif