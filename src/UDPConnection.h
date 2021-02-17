#ifndef _UDPConnection_H_
#define _UDPConnection_H_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdint>


namespace pipert {

/// this class is responsible for UDP connection management the purpose of it to enable profiler
/// to send the data through UDP connection
class UDPConnection {

private:

    /// save the state of the connection weather its opened or not
    bool opened_;
    ///  file descriptor for the new socket
    int sockfd;
    ///server information struct
    struct sockaddr_in servaddr_;
    ///UDP Connection Port number
    std::uint16_t  port_;
    ///UDP Connection address
    char* const ip_address_;



public:
    /// construct new UDP connection instance
    ///
    /// \param port_ UDP connection port
    /// \param ip_address_ UDP connection ip address
    UDPConnection(std::uint16_t port_,char* ip_address_);

    ///destructor of UDPConnection
    ~UDPConnection();

    /// open UDP connection
    void openCoccection();
    ///send the data throughout UDP connection
    ///
    /// \param message hold the byte buffer that needs to be sent
    void send(std::uint8_t* buffer);
    ///close UDP connection
    void closeConnection();
};

}

#endif //end of UDPConnection_H
