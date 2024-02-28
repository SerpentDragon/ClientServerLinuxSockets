#include <mutex>
#include <chrono>
#include <thread>
#include <fstream>
#include <string.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

class Server
{
public:

    Server(unsigned int port) : port_(port)
    {
        start();
    }

private:

    void start()
    {
        int client_socket;
        struct sockaddr_in addr;

        int server_socket = socket(AF_INET, SOCK_STREAM, 0);
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port_);

        bind(server_socket, (struct sockaddr*)&addr, sizeof(addr));

        listen(server_socket, 100);

        while(true)
        {
            client_socket = accept(server_socket, nullptr, nullptr);

            if (client_socket < 0)
            {
                std::cerr << "Error accepting client!\n";
                continue;   
            }

            std::thread(&Server::handle_connection, this, client_socket).detach();
        }
    }

    void handle_connection(int client_socket)
    {
        char message[100];

        while(true)
        {
            memset(message, 0, 100);

            ssize_t size = recv(client_socket, message, sizeof(message) - 1, 0);
            if (size <= 0)
                break;

            write_log(message);
        }
        
    }

    void write_log(char* message)
    {
        std::lock_guard lock(mtx_);

        std::fstream file("log.txt", std::ios_base::app);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file!\n";
            return;
        }

        file << message << std::endl;
        file.close();
    }

private:

    unsigned int port_;
    std::mutex mtx_;
};

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Wrong number of arguments!\n";
        return EXIT_FAILURE;
    }

    Server server(std::atoi(argv[1]));

    return EXIT_SUCCESS;
}
