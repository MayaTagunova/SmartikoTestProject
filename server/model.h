#pragma once

#include <jsoncpp/json/json.h>
#include <postgresql/libpq-fe.h>

#define HTTP_OK 200
#define HTTP_CREATED 201
#define HTTP_BAD_REQUEST 400
#define HTTP_NOT_FOUND 404
#define HTTP_INTERNAL_SERVER_ERROR 500

class Model
{
public:
    Model();

    unsigned getPost(int ID, std::string &response);
    unsigned deletePost(int ID);
    unsigned addPost(Json::Value body);
    unsigned modifyPost(int ID, Json::Value body);

private:
    PGconn *connect();
    void disconnect(PGconn *connection);


};
