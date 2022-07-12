
uint8_t uuid[16] = { 0x36, 0x27, 0x5f, 0x8e, 0xd5, 0x39, 0x11, 0xea, 0x87, 0xd0, 0x02, 0x42, 0xac, 0x13, 0x00, 0x03 };
 
#include <fcntl.h>
// convert from shorts to BYTEs and back again
#define short_get_high_byte(x) ((0xFF00 & x) >> 8)
#define short_get_low_byte(x)  (0x00FF & x)

bool SetSocketBlockingEnabled(int fd, bool bBlocking)
{
    if (fd < 0) return false;
#ifdef WIN32
    unsigned long mode = bBlocking ? 0 : 1;
    return (ioctlsocket(fd, FIONBIO, &mode) == 0) ? true : false;
#else
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return false;
    flags = bBlocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;

#endif
}


UdpClient::UdpClient(std::string host, unsigned short server_port, unsigned short local_port)
    : m_ipAddress(host)
    , m_serverport(server_port)
{
    int slen;
    slen = sizeof(si_other);
    memset(&si_other, '\0', slen);
    si_other.sin_family = PF_INET;
    //si_other.sin_addr.s_addr = inet_addr(m_sSrcIP.c_str());
    si_other.sin_addr.s_addr = inet_addr("255.255.255.255");
    si_other.sin_port = htons(49150);
    SetupSocket();
    
    service_thread = std::thread(&UdpClient::run_service, this);
}

UdpClient::~UdpClient()
{
    for (auto s: mapSockets)
    {
        closesocket(s.second);
    }

    service_thread.join();

}

bool UdpClient::SetupSocket()
{
    bool bOk = true;
    mapSockets.emplace(49150, SOCKET());
    
#ifdef _WIN32
    //Initialise winsock
    WSADATA wsa;



    std::cout << "\nInhitialising Winsock..." << std::endl;;


    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        //ns::debug::write("Failed. Error Code : %d", WSAGetLastError());
        //exit(EXIT_FAILURE);
    }

    //ns::debug::write("Initialised.\n");
#endif
    //Create a socket

    if ((mapSockets[49150] = socket(PF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        //ns::debug::write("Could not create socket : %d", WSAGetLastError());
    }

    //ns::debug::write("Socket created.\n");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;// inet_addr(m_sSrcIP.c_str());;
    server.sin_port = htons(49150);


    //Bind
    if (bind(mapSockets[49150], (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        //ns::debug::write("Bind failed with error code : %d", WSAGetLastError());
        //exit(EXIT_FAILURE);
    }

    //ns::debug::write("Bind done");
    bool enabled = true;

    if (setsockopt(mapSockets[49150], SOL_SOCKET, SO_BROADCAST, (char*)&enabled, sizeof(BOOL)) < 0) {
        //ns::debug::write("Can't enable Broadcasting");
        //exit(EXIT_FAILURE);
    }

    bool enable = true;
    
    if (setsockopt(mapSockets[49150],
        SOL_SOCKET,
        SO_BROADCAST,
        (char*)&enable, // char* for win32
        sizeof(int)) == -1) {
        //printf("Failed to bind to socket %s", artnet_net_last_error());
        //artnet_net_close(sock);
        //return ARTNET_ENET;
    }

#ifdef WIN32
    if (setsockopt(mapSockets[49150], SOL_SOCKET, SO_REUSEADDR, (char*)&enable,
        sizeof(enable)) < 0) {
    }

    u_long _true = 1;

    if (SOCKET_ERROR == ioctlsocket(mapSockets[49150], FIONBIO, &_true)) {

    }

#endif

    SetSocketBlockingEnabled(mapSockets[49150], false);
    return bOk;
}

bool UdpClient::HasMessages()
{
    return !incomingMessages.empty();
}

void UdpClient::Send(const char* data, int32_t size, std::string& s_adr, bool b_broadcast, int port)
{
    int slen;
    slen = sizeof(si_other);
    memset(&si_other, '\0', slen);
    si_other.sin_family = PF_INET;
    //si_other.sin_addr.s_addr = inet_addr(m_sSrcIP.c_str());
    si_other.sin_addr.s_addr = inet_addr(s_adr.c_str());
    si_other.sin_port = htons(port);
    if (port == 49150)
    {
        if (sendto(mapSockets[49150], data, size, 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR) {
            printf("sendto() failed with error code : %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        auto res = mapSockets.find(port);
        if (res == mapSockets.end())
        {
            mapSockets.emplace(port, socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP));

            sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = INADDR_ANY;
            addr.sin_port = htons(port);
            memset(addr.sin_zero, 0, sizeof addr.sin_zero);
            bind(mapSockets[port], (struct sockaddr*)&addr, sizeof addr);
        }

        if (sendto(mapSockets[port], data, size, 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR) {
            printf("sendto() failed with error code : %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }


    }
}
void UdpClient::Send(const std::vector<uint8_t>& message, std::string& s_adr, bool b_broadcast, int port)
{ 
    Send((const char*)message.data(), message.size(), s_adr, b_broadcast, port);
    
   /* try {
        boost::asio::ip::multicast::outbound_interface option(boost::asio::ip::address_v4::from_string(m_ipAddress.c_str()));
        socket.set_option(option);

        socket.set_option(boost::asio::socket_base::broadcast(b_broadcast));

        boost::asio::ip::udp::endpoint sendpoint(boost::asio::ip::address::from_string(s_adr.c_str()), port);

        socket.send_to(boost::asio::buffer(message), sendpoint);
    }
    catch (const boost::system::system_error& ex)
    {
        std::cout << "Failed to send from socket ... " << std::endl;
        std::cout << ex.what() << std::endl;
    }*/
}

std::vector<uint8_t> UdpClient::PopMessage()
{
    if (incomingMessages.empty())
        throw std::logic_error("No messages to pop ... ");
    return incomingMessages.pop();
}

void UdpClient::run_service()
{
    while (mapSockets.size() != 0)
    {
        start_receive();
        Sleep(10);
    }
}

void UdpClient::start_receive()
{
    static int slen = sizeof(si_other);
    int recv_len = 0;

    BYTE buf[NetworkBufferSize];

    if ((recv_len = recvfrom(mapSockets[49150], (char*)buf, NetworkBufferSize, 0, (struct sockaddr*)&si_other, &slen)) != SOCKET_ERROR) {
        ("recvfrom() failed with error code : %d", WSAGetLastError());
    }
    if (recv_len > 0)
    {
        std::vector<uint8_t> message;
        for (int i = 0; i < recv_len; i++)
        {
            message.push_back(buf[i]);
        }
        incomingMessages.push(message);
    }
    /*
    try {
        size_t availBytes = recvsocket.available();
        if (availBytes > 0)
        {
            uint8_t buffer[100000];
            std::size_t bytes_transferred = recvsocket.receive_from(boost::asio::buffer(buffer, 10000), remote_endpoint);
            if (bytes_transferred > 1)
            {
                std::vector<uint8_t> message;
                for (int i = 0; i < bytes_transferred; i++)
                {
                    message.push_back(buffer[i]);
                }
                incomingMessages.push(message);
            }
        }
    }
    catch (const boost::system::system_error& ex)
    {
        std::cout << "Failed to receive from socket ... " << std::endl;
        std::cout << ex.what() << std::endl;
    }
    */
}

void UdpClient::handle_receive(const std::error_code& error, std::size_t bytes_transferred)
{
    /*/
    if (!error)
    {
        std::vector<uint8_t> message(recv_buffer.data(), recv_buffer.data() + bytes_transferred);
        incomingMessages.push(message);
    }
    else
    {
        std::cout << "UdpClient::handle_receive: " << error.message() << std::endl;
    }

    start_receive();
    */
}
