#include <iostream>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include "mqtt/client.h"
#include "jsoncpp/json/json.h"

const std::string SERVER_ADDRESS { "tcp://localhost:1883" };
const std::string CLIENT_ID { "Smartiko-client" };

const int QOS = 1;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <topic>" << std::endl;
        return 1;
    }

    std::cout << "Initialzing..." << std::endl;
    mqtt::client client(SERVER_ADDRESS, CLIENT_ID);

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
            std::cout << "\nEnter method (GET/DELETE/POST): " << std::endl;

            std::string method;
            std::getline(std::cin, method);

            std::cout << "\nEnter URI: " << std::endl;

            std::string uri;
            std::getline(std::cin, uri);

            std::string body;

            if (method == "POST")
            {
                std::cout << "\nEnter body: " << std::endl;
                std::getline(std::cin, body);
            }

            if ((method != "GET") &&
                (method != "DELETE") &&
                (method != "POST"))
            {
                std::cerr << "\nInvalid method!" << std::endl;
                continue;
            }

            Json::Value message;
            message["method"] = method;
            message["uri"] = uri;
            if (!body.empty())
            {
                message["body"] = body;
            }

            std::cout << "\nSending message..." << std::endl;
            auto pubmsg = mqtt::make_message(argv[1], message.toStyledString());
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
