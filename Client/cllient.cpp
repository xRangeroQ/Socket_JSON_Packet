// Include Libraries
#include <iostream>
#include <winsock2.h>
#include <nlohmann/json.hpp>

// Main Function
int main()
{
    // WSA Initialize
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        std::cerr << "WSA Initialize Error! Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }
    std::cout << "WSA Initialize Successful!" << std::endl;

    // Create Socket
    SOCKET Client = socket(AF_INET, SOCK_STREAM, 0);
    if (Client == INVALID_SOCKET) {
        std::cerr << "Socket Create Error! Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 2;
    }
    std::cout << "Socket Create Successful!" << std::endl;
    
    // Prepare Connection
    sockaddr_in Connection_Address;
    Connection_Address.sin_family = AF_INET;
    Connection_Address.sin_port = htons(35400);
    Connection_Address.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    // Connect
    if (connect(Client, (sockaddr*)&Connection_Address, sizeof(Connection_Address)) == SOCKET_ERROR)
    {
        if (Client != INVALID_SOCKET) closesocket(Client);
        std::cerr << "Connection Error! Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 3;
    }
    std::cout << "Connection Successful!" << std::endl;
    
    // Send
    nlohmann::json j = {
        {"Id", 0},
        {"Type", "BASIC"}
    };
    
    std::string Packet = j.dump();
    if (send(Client, Packet.c_str(), Packet.length(), 0) == SOCKET_ERROR)
    {
        if (Client != INVALID_SOCKET) closesocket(Client);
        std::cerr << "Send Error! Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 4;
    }
    std::cout << "Send Successful!" << std::endl;

    // Close Program
    if (Client != INVALID_SOCKET) closesocket(Client);
    WSACleanup();
    std::cout << "Resources Cleaned!" << std::endl;

    // Return Function
    return 0;
}
