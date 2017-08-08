#include <iostream>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include "mqtt/client.h"

const std::string SERVER_ADDRESS { "tcp://localhost:1883" };
const std::string CLIENT_ID { "Smartiko-client" };

const int QOS = 1;

class user_callback : public virtual mqtt::callback
{
    void connection_lost(const std::string& cause) override {
        std::cout << "\nConnection lost" << std::endl;
        if (!cause.empty())
            std::cout << "\tcause: " << cause << std::endl;
    }

public:
};

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <topic>" << std::endl;
        return 1;
    }

    std::cout << "Initialzing..." << std::endl;
    mqtt::client client(SERVER_ADDRESS, CLIENT_ID);

    user_callback cb;
    client.set_callback(cb);

    mqtt::connect_options connOpts;
    connOpts.set_keep_alive_interval(20);
    connOpts.set_clean_session(true);
    std::cout << "...OK" << std::endl;

    try {
        std::cout << "\nConnecting..." << std::endl;
        client.connect(connOpts);
        std::cout << "...OK" << std::endl;

        while (true)
        {
            std::cout << "\nEnter message: " << std::endl;

            std::string message;
            std::getline(std::cin, message);

            std::cout << "\nSending message..." << std::endl;
            auto pubmsg = mqtt::make_message(argv[1], message);
            pubmsg->set_qos(QOS);
            client.publish(pubmsg);
            std::cout << "...OK" << std::endl;
        }

        std::cout << "\nDisconnecting..." << std::endl;
        client.disconnect();
        std::cout << "...OK" << std::endl;
    }
    catch (const mqtt::exception& exc) {
        std::cerr << exc.what() << std::endl;
        return 1;
    }

    std::cout << "\nExiting" << std::endl;
    return 0;
}
