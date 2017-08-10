#pragma once
#include "model.h"

class Controller
{
public:
    Controller(Model& model);

    void handleRequest(std::string request);

private:
    Model& model_;
};
