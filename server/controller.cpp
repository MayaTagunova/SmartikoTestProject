#include "controller.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <sstream>

const std::string SECTION = "posts";

Controller::Controller(Model& model)
    : model_(model)
{}

std::string Controller::handleRequest(std::string request)
{
    std::ostringstream response;
    Json::Reader reader;

    Json::Value message;
    if (!reader.parse(request.c_str(), message)) {
        response << "JSON parsing error: "
                 << reader.getFormattedErrorMessages()
                 << std::endl;
        return response.str();
    }

    if (!message.isMember("uri")) {
        response << "Invalid query: command not specified" << std::endl;
        response << "Usage: " << "/posts/[ID]" << std::endl;
        return response.str();
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
        response << "Invalid query: command not specified" << std::endl;
        response << "Usage: " << "/posts/[ID]" << std::endl;
        return response.str();
    }

    if (elements[0] != SECTION) {
        response << "Invalid query: command not recognized" << std::endl;
        response << "Usage: " << "/posts/[ID]" << std::endl;
        return response.str();
    }

    if (message["method"] == "GET") {
        if (elements.size() < 2) {
            response << "Invalid query: ID not specified" << std::endl;
            response << "Usage: " << "GET /posts/<ID>" << std::endl;
            return response.str();
        }

        int ID;
        std::istringstream (elements[1]) >> ID;
        response << model_.getPost(ID);
    }
    else if (message["method"] == "DELETE") {
        if (elements.size() < 2) {
            response << "Invalid query: ID not specified" << std::endl;
            response << "Usage: " << "DELETE /posts/<ID>" << std::endl;
            return response.str();
        }

        int ID;
        std::istringstream (elements[1]) >> ID;
        model_.deletePost(ID);
    }
    else if (message["method"] == "POST") {
        Json::Value body;
        if (message.isMember("body")) {
            if (!reader.parse(message["body"].asString(), body)) {
                response << "Body was invalid JSON object" << std::endl;
                response << "Usage: " << "POST /posts/[ID] {\"title\":<title>, \"content\":<content>}" << std::endl;
                return response.str();
            }
        }

        if (elements.size() > 1) {
            int ID;
            std::istringstream (elements[1]) >> ID;
            model_.modifyPost(ID, body);
        }
        else {
            model_.addPost(body);
        }
    }
    return response.str();
}
