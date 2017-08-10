#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <cctype>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <mqtt/client.h>
#include "model.h"
#include "controller.h"

const std::string SERVER_ADDRESS("tcp://localhost:1883");
const std::string CLIENT_ID("Smartiko-server");

const int QOS = 1;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <topic>" << std::endl;
        return 1;
    }

    mqtt::connect_options options;
    options.set_keep_alive_interval(20);
    options.set_clean_session(true);
    options.set_automatic_reconnect(true);

    mqtt::client client(SERVER_ADDRESS, CLIENT_ID);
    Model model;
    Controller controller(model);

    try {
        std::cout << "Connecting to the MQTT server..." << std::flush;
        client.connect(options);
        client.subscribe(argv[1], QOS);
        std::cout << "OK" << std::endl;

        while (true) {
            auto message = client.consume_message();

            if (!message) {
                break;
            }

            controller.handleRequest(message->get_payload_str());
        }

        std::cout << "Disconnecting from the MQTT server..." << std::flush;
        client.disconnect();
        std::cout << "OK" << std::endl;
    }
    catch (const mqtt::exception& exception) {
        std::cerr << exception.what() << std::endl;
        return 1;
    }

    return 0;
}
