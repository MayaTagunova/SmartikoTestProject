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

int main(int argc, char *argv[])
{
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
        client.subscribe("client_queue", QOS);
        std::cout << "OK" << std::endl;

        while (true) {
            auto message = client.consume_message();

            if (!message) {
                break;
            }

            std::string body;
            unsigned code = controller.handleRequest(message->get_payload_str(), body);

            Json::Value response;
            response["code"] = std::to_string(code);
            response["body"] = body;
            std::cout << "Sending response..." << std::endl;
            auto mqtt_message = mqtt::make_message("server_queue", response.toStyledString());
            mqtt_message->set_qos(QOS);
            client.publish(mqtt_message);
            std::cout << "...OK" << std::endl;
        }

        std::cout << "Disconnecting..." << std::flush;
        client.disconnect();
        std::cout << "OK" << std::endl;
    }
    catch (const mqtt::exception& exception) {
        std::cerr << exception.what() << std::endl;
        return 1;
    }

    return 0;
}
