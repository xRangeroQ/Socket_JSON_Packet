// Include Libraries
#include <chrono>
#include <atomic>
#include <thread>
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <nlohmann/json.hpp>

// Global Variable(s)
std::thread Event_Thread;
std::atomic<bool> Status = true;

// Handle Event Function
void Start_Event(SOCKET &Server)
{
    Event_Thread = std::thread([&Server]() {
        while (Status)
        {
            if (GetAsyncKeyState(0x1B))
            {
                Status.store(false);
                closesocket(Server);
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
    });
    
    if (Event_Thread.joinable())
        Event_Thread.detach();
}

// Handle Client Function
void Handle_Client(SOCKET Client, sockaddr_in Client_Address, int Client_Address_Size)
{
    char Buffer[4096];
    int recv_len = recv(Client, Buffer, sizeof(Buffer), 0);
    if (recv_len > 0)
    {
        Buffer[recv_len] = '\0';
        nlohmann::json j = nlohmann::json::parse(Buffer);
        std::cout << j["Id"] << " - " << j["Type"] << std::endl;
    }
    if (Client != INVALID_SOCKET) closesocket(Client);
}

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
    SOCKET Server = socket(AF_INET, SOCK_STREAM, 0);
    if (Server == INVALID_SOCKET)
    {
        std::cerr << "Socket Create Error! Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 2;
    }
    std::cout << "Socket Create Successful!" << std::endl;

    // Start Event
    Start_Event(Server);
    
    // Prepare Bind
    sockaddr_in Server_Address;
    Server_Address.sin_family = AF_INET;
    Server_Address.sin_port = htons(35400);
    Server_Address.sin_addr.s_addr = INADDR_ANY;

    // Bind
    if (bind(Server, (sockaddr*)&Server_Address, sizeof(Server_Address)) == SOCKET_ERROR)
    {
        if (Server != INVALID_SOCKET) closesocket(Server);
        std::cout << "Socket Bind Error! Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 3;
    }
    std::cout << "Socket Bind Successful!" << std::endl;

    // Listen
    if (listen(Server, 15) == SOCKET_ERROR)
    {
        if (Server != INVALID_SOCKET) closesocket(Server);
        std::cout << "Socket Listen Error! Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 4;
    }

    // Create Client Variables
    SOCKET Client;
    sockaddr_in Client_Address;
    int Client_Address_Len = sizeof(Client_Address);

    // Wait Connections
    while (Status.load())
    {
        Client = accept(Server, (sockaddr*)&Client_Address, &Client_Address_Len);
        if (Client == INVALID_SOCKET) continue;
        std::cout << inet_ntoa(Client_Address.sin_addr) << " - " << ntohs(Client_Address.sin_port) << std::endl;
        std::thread t(Handle_Client, Client, Client_Address, Client_Address_Len);
        t.detach();
    }

    // Wait 1 Second
    Sleep(1000);

    // Close Program
    if (Event_Thread.joinable()) Event_Thread.join();
    if (Client != INVALID_SOCKET) closesocket(Client);
    if (Server != INVALID_SOCKET) closesocket(Server);
    WSACleanup();
    std::cout << "Resources Cleaned!" << std::endl;

    // Return Function
    return 0;
}
