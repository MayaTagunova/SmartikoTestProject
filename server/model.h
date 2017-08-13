#pragma once

#include <jsoncpp/json/json.h>
#include <postgresql/libpq-fe.h>

class Model
{
public:
    Model();

    void getPost(int ID);
    void deletePost(int ID);
    void addPost(Json::Value body);
    void modifyPost(int ID, Json::Value body);

private:
    PGconn *connect();
    void disconnect(PGconn *connection);
};
