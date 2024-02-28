#include <chrono>
#include <thread>
#include <iomanip>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

class Client
{
public:

    Client(const char* name, unsigned int port, unsigned int time)
        : name_(name), port_(port), time_(time), address_("127.0.0.1")
    {
        start();
    }

private:

    void start()
    {
        int client_socket = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in addr;

        if (client_socket < 0)
        {
            std::cerr << "Error creating socket!\n";
            return;
        }

        addr.sin_family = AF_INET;
        addr.sin_port = htons(port_);
        inet_aton(address_, &addr.sin_addr);

        if(connect(client_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            std::cerr << "Error connecting to server!";
            return;
        }

        while(true)
        {
            std::string message = create_message();

            send(client_socket, message.c_str(), message.size(), 0);

            std::this_thread::sleep_for(static_cast<std::chrono::seconds>(time_));
        }
    }

    std::string create_message()
    {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm *tm_now = std::localtime(&now_time);

        std::ostringstream oss;

        oss << "[" << std::put_time(tm_now, "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0') << std::setw(3)
              << std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000
              << "] " << name_;

        return oss.str();
    }

private:

    const char* name_;
    const char* address_;
    unsigned int port_;
    unsigned int time_;
};

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        std::cerr << "Wrong argument's number!\n";
        return EXIT_FAILURE;
    }    

    Client client(argv[1], std::atoi(argv[2]), std::atoi(argv[3]));

    return EXIT_SUCCESS;
}
