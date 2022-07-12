#pragma once

#include <string>
#include <vector>
#include <map>
#include <thread>
#include <array>
#include "LockedQueue.h"
#define USE_RAW_UDP
static const int NetworkBufferSize = 4096;

class IClient
{
public:
	virtual ~IClient() {};
	virtual bool SetupSocket() = 0;
	virtual bool HasMessages() = 0;
	virtual void Send(const std::vector<uint8_t>& message, std::string& s_adr, bool b_broadcast, int port = 49150) = 0;
	virtual void Send(const char* data, int32_t size, std::string& s_adr, bool b_broadcast, int port = 49150)=0;
	virtual std::vector<uint8_t> PopMessage() = 0;
};


#ifndef USE_RAW_UDP
#include <boost/asio.hpp>
using boost::asio::ip::udp;
class UdpClient : public IClient
{

public:
	UdpClient(std::string host, unsigned short server_port, unsigned short local_port = 0);
	virtual ~UdpClient() override;
	virtual bool SetupSocket() override;
	virtual bool HasMessages() override;
	virtual void Send(const std::vector<uint8_t>& message, std::string& s_adr, bool b_broadcast, int port = 49150);
	virtual std::vector<uint8_t> PopMessage() override;
protected:
	void run_service();
	void start_receive();

	void handle_receive(const std::error_code& error, std::size_t bytes_transferred);

private:
	std::string m_ipAddress;
	unsigned short m_serverport;
	boost::asio::io_context io_context;
	udp::socket socket, recvsocket;
	udp::endpoint server_endpoint;
	udp::endpoint remote_endpoint;
	std::array<char, NetworkBufferSize> recv_buffer;
	std::thread service_thread;
	LockedQueue<std::vector<uint8_t>> incomingMessages;
};
#else 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#ifdef _WIN32
#include <WinSock2.h>

#else
#include <sys/socket.h> 
#define SOCKET int
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#include <errno.h>
#define WSAGetLastError() errno
#include <linux/if_packet.h> 

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

#endif

class UdpClient : public IClient
{

public:
	UdpClient(std::string host, unsigned short server_port, unsigned short local_port = 0);
	virtual ~UdpClient() override;
	virtual bool SetupSocket() override;
	virtual bool HasMessages() override;
	virtual void Send(const std::vector<uint8_t>& message, std::string& s_adr, bool b_broadcast, int port = 49150);
	virtual void Send(const char* data, int32_t size, std::string& s_adr, bool b_broadcast, int port = 49150);
	virtual std::vector<uint8_t> PopMessage() override;
protected:
	void run_service();
	void start_receive();

	void handle_receive(const std::error_code& error, std::size_t bytes_transferred);

private:
	std::string m_ipAddress;
	unsigned short m_serverport;
	
	
	std::map<int, SOCKET> mapSockets;
	sockaddr_in server, si_other;
	//e131_addr_t _dest;
	std::array<char, NetworkBufferSize> recv_buffer;
	std::thread service_thread;
	LockedQueue<std::vector<uint8_t>> incomingMessages;
};
#endif