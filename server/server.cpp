#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <cctype>
#include <thread>
#include <chrono>
#include "mqtt/client.h"

const std::string SERVER_ADDRESS("tcp://localhost:1883");
const std::string CLIENT_ID("Smartiko-server");

const int QOS = 1;

class callback : public virtual mqtt::callback
{
    mqtt::client& client_;

    void connected(const std::string& cause) override {
        std::cout << "\nConnected: " << cause << std::endl;
    }

    void connection_lost(const std::string& cause) override {
        std::cout << "\nConnection lost";
        if (!cause.empty())
            std::cout << ": " << cause << std::endl;
        std::cout << std::endl;
    }

    void message_arrived(mqtt::const_message_ptr msg) override {
        std::cout << msg->get_topic() << ": " << msg->get_payload_str() << std::endl;
    }

    void delivery_complete(mqtt::delivery_token_ptr token) override {}

public:
    callback(mqtt::client& client) : client_(client) {}
};

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <topic>" << std::endl;
        return 1;
    }

    mqtt::connect_options connOpts;
    connOpts.set_keep_alive_interval(20);
    connOpts.set_clean_session(false);
    connOpts.set_automatic_reconnect(true);

    mqtt::client client(SERVER_ADDRESS, CLIENT_ID);

    callback cb(client);
    client.set_callback(cb);

    try {
        std::cout << "Connecting to the MQTT server..." << std::flush;
        client.connect(connOpts);
        client.subscribe(argv[1], QOS);
        std::cout << "OK" << std::endl;
    }
    catch (const mqtt::exception& exc) {
        std::cerr << "\nERROR: Unable to connect to MQTT server: '"
            << SERVER_ADDRESS << "'" << std::endl;
        return 1;
    }

    while (true);

    try {
        std::cout << "\nDisconnecting from the MQTT server..." << std::flush;
        client.disconnect();
        std::cout << "OK" << std::endl;
    }
    catch (const mqtt::exception& exc) {
        std::cerr << exc.what() << std::endl;
        return 1;
    }

    return 0;
}
