#include <iostream>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sstream>
#include <mqtt/client.h>
#include <json/json.h>
#include <microhttpd.h>

#define PORT 8888

const std::string SERVER_ADDRESS { "tcp://localhost:1883" };
const std::string CLIENT_ID { "Smartiko-client" };

const int QOS = 1;

static void request_completed(void *cls,
                              struct MHD_Connection *connection,
                              void **con_cls,
                              enum MHD_RequestTerminationCode toe)
{
    if (*con_cls == nullptr) {
        return;
    }

    std::string *body = static_cast<std::string*>(*con_cls);
    delete body;
    *con_cls = nullptr;
}

int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url,
                         const char *method, const char *version,
                         const char *upload_data,
                         size_t *upload_data_size, void **con_cls)
{
    if (*con_cls == nullptr) {
        std::string *request_body = new std::string();
        *con_cls = (void *)request_body;
        if (0 == strcasecmp(method, MHD_HTTP_METHOD_POST)) {
            return MHD_YES;
        }
    }

    unsigned code = MHD_HTTP_INTERNAL_SERVER_ERROR;
    std::string response_body;

    if ((0 == strcasecmp(method, MHD_HTTP_METHOD_GET)) ||
        (0 == strcasecmp(method, MHD_HTTP_METHOD_DELETE)) ||
        (0 == strcasecmp(method, MHD_HTTP_METHOD_POST))) {

        Json::Value message;
        message["method"] = method;
        message["uri"] = url;
        std::string *request_body = static_cast<std::string*>(*con_cls);
        if (*upload_data_size != 0) {
            *request_body += std::string(upload_data, *upload_data_size);
            *upload_data_size = 0;
            return MHD_YES;
        }
        else {
            if (!request_body->empty()) {
                message["body"] = *request_body;
            }
            request_body->clear();
        }

        auto mqtt_message = mqtt::make_message("client_queue", message.toStyledString());
        mqtt_message->set_qos(QOS);

        mqtt::client *client = (mqtt::client *)cls;
        client->publish(mqtt_message);
        std::cout << "...OK" << std::endl;

        std::string page = client->consume_message()->get_payload_str();
        Json::Reader reader;
        Json::Value value;

        if (!reader.parse(page.c_str(), value)) {
            return MHD_NO;
        }

        response_body = value["body"].asString();
        if (value.isMember("code")) {
            std::istringstream(value["code"].asString()) >> code;
        }
    }

    struct MHD_Response *response = MHD_create_response_from_buffer(response_body.length(), (void*)response_body.c_str(), MHD_RESPMEM_MUST_COPY);
    if (response == 0) {
        return MHD_NO;
    }
    MHD_add_response_header(response,
                            MHD_HTTP_HEADER_CONTENT_TYPE,
                            "application/json");
    int result = MHD_queue_response(connection, code, response);
    MHD_destroy_response(response);

    return result;
}

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

        struct MHD_Daemon *daemon;

        daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, nullptr, nullptr,
                                  &answer_to_connection, (void *)&client, MHD_OPTION_NOTIFY_COMPLETED, &request_completed, nullptr, MHD_OPTION_END);
        if (daemon == nullptr) {
            return 1;
        }

        (void)getchar();

        MHD_stop_daemon(daemon);

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
