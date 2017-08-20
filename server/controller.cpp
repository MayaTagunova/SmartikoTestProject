#include "controller.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <sstream>

const std::string SECTION = "posts";

Controller::Controller(Model& model)
    : model_(model)
{}

unsigned Controller::handleRequest(std::string request, std::string &response)
{
    unsigned code = HTTP_INTERNAL_SERVER_ERROR;

    Json::Reader reader;

    Json::Value message;
    if (!reader.parse(request.c_str(), message)) {
        return HTTP_BAD_REQUEST;
    }

    if (!message.isMember("uri")) {
        return HTTP_BAD_REQUEST;
    }

    std::vector<std::string> elements;
    std::string uri = message["uri"].asString();
    boost::split(elements, uri, boost::is_any_of("/"));

    elements.erase(
                std::remove_if(
                    elements.begin(),
                    elements.end(),
                    [](std::string element){return element.empty();}),
            elements.end());

    if (elements.size() == 0) {
        return HTTP_NOT_FOUND;
    }

    if (elements[0] != SECTION) {
        return HTTP_NOT_FOUND;
    }

    if (message["method"] == "GET") {
        if (elements.size() < 2) {
            return HTTP_BAD_REQUEST;
        }

        int ID;
        std::istringstream(elements[1]) >> ID;
        code = model_.getPost(ID, response);
    }
    else if (message["method"] == "DELETE") {
        if (elements.size() < 2) {
            return HTTP_BAD_REQUEST;
        }

        int ID;
        std::istringstream(elements[1]) >> ID;
        code = model_.deletePost(ID);
    }
    else if (message["method"] == "POST") {
        Json::Value body;
        if (message.isMember("body")) {
            if (!reader.parse(message["body"].asString(), body)) {
                return HTTP_BAD_REQUEST;
            }
        }

        if (elements.size() > 1) {
            int ID;
            std::istringstream(elements[1]) >> ID;
            code = model_.modifyPost(ID, body);
        }
        else {
            code = model_.addPost(body);
        }
    }
    return code;
}
