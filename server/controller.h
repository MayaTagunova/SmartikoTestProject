#pragma once
#include "model.h"

class Controller
{
public:
    Controller(Model& model);

    std::string handleRequest(std::string request);

private:
    Model& model_;
};
