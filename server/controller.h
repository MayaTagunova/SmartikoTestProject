#pragma once
#include "model.h"

class Controller
{
public:
    Controller(Model& model);

    unsigned handleRequest(std::string request, std::string &response);

private:
    Model& model_;
};
