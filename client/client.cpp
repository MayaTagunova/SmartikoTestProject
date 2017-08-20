#include <iostream>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <mqtt/client.h>
#include <json/json.h>

const std::string SERVER_ADDRESS { "tcp://localhost:1883" };
const std::string CLIENT_ID { "Smartiko-client" };

const int QOS = 1;

int main(int argc, char* argv[])
{
    mqtt::client client(SERVER_ADDRESS, CLIENT_ID);

    mqtt::connect_options options;
    options.set_keep_alive_interval(20);
    options.set_clean_session(true);
    options.set_automatic_reconnect(true);

    try {
        std::cout << "Connecting..." << std::endl;
        client.connect(options);
        std::cout << "...OK" << std::endl;

        client.subscribe("server_queue", QOS);

        while (true) {
            std::cout << "Enter method (GET/DELETE/POST): " << std::endl;

            std::string method;
            std::getline(std::cin, method);

            if ((method != "GET") &&
                (method != "DELETE") &&
                (method != "POST")) {
                std::cerr << "Invalid method!" << std::endl;
                continue;
            }

            std::cout << "Enter URI: " << std::endl;

            std::string uri;
            std::getline(std::cin, uri);

            std::string body;

            if (method == "POST") {
                std::cout << "Enter body: " << std::endl;
                std::getline(std::cin, body);
            }

            Json::Value message;
            message["method"] = method;
            message["uri"] = uri;
            if (!body.empty()) {
                message["body"] = body;
            }

            std::cout << "Sending message..." << std::endl;
            auto mqtt_message = mqtt::make_message("client_queue", message.toStyledString());
            mqtt_message->set_qos(QOS);
            client.publish(mqtt_message);
            std::cout << "...OK" << std::endl;

            auto response = client.consume_message();

            if (response) {
                std::cout << response->get_payload_str() << std::endl;
            }
        }

        std::cout << "Disconnecting..." << std::endl;
        client.disconnect();
        std::cout << "...OK" << std::endl;
    }
    catch (const mqtt::exception& exc) {
        std::cerr << exc.what() << std::endl;
        return 1;
    }

    std::cout << "Exiting" << std::endl;
    return 0;
}
