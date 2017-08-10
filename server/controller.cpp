#include "controller.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <sstream>

const std::string SECTION = "posts";

Controller::Controller(Model& model)
    : model_(model)
{}

void Controller::handleRequest(std::string request)
{
    Json::Reader reader;

    Json::Value message;
    if (!reader.parse(request.c_str(), message))
    {
        std::cerr  << "JSON parsing error: "
                   << reader.getFormattedErrorMessages();
        return;
    }

    if (!message.isMember("uri"))
    {
        std::cerr << "Invalid query: no command!" << std::endl;
        return;
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

    if (elements.size() == 0)
    {
        std::cerr << "Invalid query: no command!" << std::endl;
        return;
    }

    if (elements[0] != SECTION)
    {
        std::cerr << "Invalid query: command not recognized!" << std::endl;
        return;
    }

    if (message["method"] == "GET")
    {
        if (elements.size() < 2)
        {
            std::cerr << "Invalid query: no ID!" << std::endl;
            return;
        }

        int ID;
        std::istringstream (elements[1]) >> ID;
        model_.getPost(ID);
    }
    else if (message["method"] == "DELETE")
    {
        if (elements.size() < 2)
        {
            std::cerr << "Invalid query: no ID!" << std::endl;
            return;
        }

        int ID;
        std::istringstream (elements[1]) >> ID;
        model_.deletePost(ID);
    }
    else if (message["method"] == "POST")
    {
        Json::Value body;
        if (message.isMember("body"))
        {
            if (!reader.parse(message["body"].asString(), body))
            {
                std::cerr << "Body was invalid JSON object" << std::endl;
                return;
            }
        }

        if (elements.size() > 1)
        {
            int ID;
            std::istringstream (elements[1]) >> ID;
            std::cout << "MODIFY" << std::endl;
            model_.modifyPost(ID, body);
        }
        else
        {
            model_.addPost(body);
            std::cout << "ADD" << std::endl;
        }
    }
}
