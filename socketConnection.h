/*
 * socketConnection.h
 *
 *  Created on: 7 Dec 2017
 *      Author: bhzhang
 */

#ifndef SOCKETCONNECTION_H_
#define SOCKETCONNECTION_H_

#include <boost/shared_ptr.hpp>
#include <string>

#include "CoSimDataStruct.h"

class socketConnection
{
public:

		socketConnection();

    ~socketConnection();


    /// blocking function to accept an incoming connection
    int acceptSerial(int port);

    int connectSerial(std::string host, unsigned short port);

    int recvData(CoSimSocketData& req);

    int sendData(const CoSimSocketData& data);

    /*
    int readSerial(PolyVRRequestType& t);

    int writeSerial(const CoSimDataStruct& t);
    */


private:
    int sockfd, listensockfd;
};

typedef boost::shared_ptr<socketConnection> socketConnection_ptr;



#endif /* SOCKETCONNECTION_H_ */
