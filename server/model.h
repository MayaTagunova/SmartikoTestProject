#pragma once

#include <jsoncpp/json/json.h>
#include <postgresql/libpq-fe.h>

class Model
{
public:
    Model();

    std::string getPost(int ID);
    std::string deletePost(int ID);
    std::string addPost(Json::Value body);
    std::string modifyPost(int ID, Json::Value body);

private:
    PGconn *connect();
    void disconnect(PGconn *connection);


};
