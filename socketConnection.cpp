#include "socketConnection.h"

#include <boost/shared_ptr.hpp>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <iostream>
socketConnection::socketConnection(): sockfd(-1),listensockfd(-1) { }


socketConnection::~socketConnection()
{
    if(listensockfd >= 0)
    	  close(listensockfd);

    if(sockfd >= 0)
        close(sockfd);
}

int socketConnection::connectSerial(std::string host, unsigned short port)
{
    struct sockaddr_in  serv_addr;
    struct hostent      *server;


    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
    		printf("create socket error: %s errno :%d\n", strerror(errno), errno);
        return -1;
    }

    server = gethostbyname(host.c_str());
    if (server == NULL)
    {
    		printf("host address failed to resolve: %s errno :%d\n", strerror(errno), errno);
        return -1;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);

    serv_addr.sin_port = htons(port);
    //connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)
    if(connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
    		printf("connect socket error: %s errno :%d\n", strerror(errno), errno);
        return -1;
    }
    return 0;
}

/// blocking function to accept an incoming connection
int socketConnection::acceptSerial(int port)
{
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    listensockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listensockfd < 0)
    {
    		printf("create accept socket error: %s errno :%d\n", strerror(errno), errno);
        return -1;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(listensockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
    {
    		printf("bind port: %d failed: %s errno :%d\n", port, strerror(errno), errno);
        return -1;
    }

    listen(listensockfd,5);

    clilen = sizeof(cli_addr);
    sockfd = accept(listensockfd, (struct sockaddr *) &cli_addr, &clilen);

    if (sockfd < 0)
    {
    		printf("accept socket failed: %s errno :%d\n", strerror(errno), errno);
        return -1;
    }
    return 0;
}

int socketConnection::recvData(CoSimSocketData& req){
		int n;
		n = recv(sockfd, &req, sizeof(req), 0);
    if (n < 0) {
        std::cout << "Receive Data Error" << std::endl;
        return -1;
    }
    return 0;
}

int socketConnection::sendData(const CoSimSocketData& data){
		int n;
		n = send(sockfd, &data, sizeof(data), 0);
		if (n < 0) {
				std::cout << "Send Data Error" << std::endl;
				return -1;
		}
		return 0;
}


/*
int socketConnection::readSerial(CoSimDataStruct& t)
{
    int n;

    // read header
    //------------------
    n = read(sockfd, m_inbound_header, header_length);
    if (n < header_length)
    {
        std::cout << "Error during header read" << std::endl;
        return -1;
    }

    // determine the length of the serialized data.
    std::istringstream is(std::string(m_inbound_header, header_length));
    std::size_t inbound_data_size = 0;
    if (!(is >> std::hex >> inbound_data_size))
    {
        // header doesn't seem to be valid. Inform the caller.
        return -1;
    }

    // resize inbound data array if required
    if(m_inbound_data_storage_size < inbound_data_size)
    {
        delete[] m_inbound_data;
        m_inbound_data_storage_size = inbound_data_size;
        m_inbound_data = new char[m_inbound_data_storage_size];
    }

    // read data
    //------------------
    n = read(sockfd, m_inbound_data, inbound_data_size);
    if (n < 0)
    {
        std::cout << "Error during data read" << std::endl;
        return -1;
    }


    // extract the data structure from the data just received.
    try
    {
        std::string archive_data(m_inbound_data, inbound_data_size);

        std::istringstream archive_stream(archive_data);
        boost::archive::text_iarchive archive(archive_stream, boost::archive::no_header);
        archive >> t;
    }
    catch (std::exception& e)
    {
        // unable to decode data.
        std::cout << "Error during de-serialization" << std::endl;
        std::cout << e.what() << std::endl;

        return -1;
    }

    return 0;
}

int socketConnection::writeSerial(const CoSimDataStruct& t)
{
    // serialize the data first so we know how large it is.
    std::ostringstream archive_stream;
    boost::archive::text_oarchive archive(archive_stream, boost::archive::no_header);
    archive << t;
    m_outbound_data = archive_stream.str();

    // format the header
    std::ostringstream header_stream;
    header_stream << std::setw(header_length)
    << std::hex << m_outbound_data.size();
    if (!header_stream || header_stream.str().size() != header_length)
    {
        // something went wrong
        return -1;
    }
    m_outbound_header = header_stream.str();

    // resize array if required
    if((m_outbound_header.size() + m_outbound_data.size()) > m_outboud_data_storage_size)          // termination is ignored '\0'
    {
        delete[] m_outbound_data_buf;
        m_outboud_data_storage_size = m_outbound_header.size() + m_outbound_data.size();
        m_outbound_data_buf = new char[m_outboud_data_storage_size];
    }
    // Write the serialized data to the socket. We use "gather-write" to send
    // both the header and the data in a single write operation.
    std::copy(m_outbound_header.begin(), m_outbound_header.end(), m_outbound_data_buf);         // '\0\ is ignored by iterator
    std::copy(m_outbound_data.begin(), m_outbound_data.end(), &m_outbound_data_buf[m_outbound_header.size()]);

    int n = write( sockfd,m_outbound_data_buf,(m_outbound_header.size() + m_outbound_data.size()) );
    if(n < 0)
    {
        std::cout << "Error during write" << std::endl;
        return -1;
    }

    return 0;
}
*/
