#include "model.h"
#include <iostream>
#include <arpa/inet.h>
#include <cstring>

Model::Model()
{}

void Model::getPost(int ID)
{
    PGconn *connection = connect();
    if (connection == nullptr)
    {
        return;
    }

    ID = htonl(ID);
    const char *values[] = {reinterpret_cast<const char *>(&ID)};
    const int lengths[] = {sizeof(ID)};
    const int formats[] = {1};

    PGresult *result = PQexecParams(connection,
                                    "SELECT * FROM posts WHERE id = $1",
                                    1,
                                    0,
                                    values,
                                    lengths,
                                    formats,
                                    0);

    if (PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        std::cerr << "SELECT failed: " << PQerrorMessage(connection) << std::endl;
    }

    if (PQntuples(result) == 0)
    {
        std::cout << "Post not found" << std::endl;
    }
    else
    {
        std::cout << "title: " << PQgetvalue(result, 0, 2) << std::endl;
        std::cout << "content: " << PQgetvalue(result, 0, 1) << std::endl;
    }

    PQclear(result);
    disconnect(connection);
}

void Model::deletePost(int ID)
{
    PGconn *connection = connect();
    if (connection == nullptr)
    {
        return;
    }

    ID = htonl(ID);
    const char *values[] = {reinterpret_cast<const char *>(&ID)};
    const int lengths[] = {sizeof(ID)};
    const int formats[] = {1};

    PGresult *result = PQexecParams(connection,
                                    "DELETE FROM posts WHERE id = $1",
                                    1,
                                    0,
                                    values,
                                    lengths,
                                    formats,
                                    0);

    if (PQresultStatus(result) != PGRES_COMMAND_OK)
    {
        std::cerr << "DELETE failed: " << PQerrorMessage(connection) << std::endl;
    }

    PQclear(result);
    disconnect(connection);
}

void Model::addPost(Json::Value body)
{
    PGconn *connection = connect();
    if (connection == nullptr)
    {
        return;
    }

    Json::Value content = body.get("content", "This is an empty and useless post");
    Json::Value title = body.get("title", "No subject");

    const char *values[] = {title.asCString(), content.asCString()};

    PGresult *result = PQexecParams(connection,
                                    "INSERT INTO posts (title, content) VALUES ($1, $2)",
                                    2,
                                    0,
                                    values,
                                    nullptr,
                                    nullptr,
                                    0);

    if (PQresultStatus(result) != PGRES_COMMAND_OK)
    {
        std::cerr << "INSERT failed: " << PQerrorMessage(connection) << std::endl;
    }

    PQclear(result);
    disconnect(connection);
}

void Model::modifyPost(int ID, Json::Value body)
{
    PGconn *connection = connect();
    if (connection == nullptr)
    {
        return;
    }

    const char *title = body.isMember("title") ? body["title"].asCString() : nullptr;
    const char *content = body.isMember("content") ? body["content"].asCString() : nullptr;

    ID = htonl(ID);
    const char *values[] = {title,
                            content,
                            reinterpret_cast<const char *>(&ID)};
    const int lengths[] = {0, 0,
                         sizeof(ID)};
    const int formats[] = {0, 0, 1};

    PGresult *result = PQexecParams(connection,
                                    "UPDATE posts SET title = COALESCE($1, title), content = COALESCE($2, content) WHERE id = $3",
                                    3,
                                    0,
                                    values,
                                    lengths,
                                    formats,
                                    0);

    if (PQresultStatus(result) != PGRES_COMMAND_OK)
    {
        std::cerr << "INSERT failed: " << PQerrorMessage(connection) << std::endl;
    }

    PQclear(result);
    disconnect(connection);
}

PGconn *Model::connect()
{
    PGconn *connection = PQconnectdb("dbname=Blog host=localhost user=postgres");
    if (PQstatus(connection) == CONNECTION_BAD) {
        std::cerr << "Connection failed!" << std::endl;
        return nullptr;
    }

    PGresult *result = PQexec(connection, "BEGIN");

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        std::cerr << "BEGIN failed: " << PQerrorMessage(connection) << std::endl;
        PQclear(result);
        PQfinish(connection);
        return nullptr;
    }

    PQclear(result);
    return connection;
}

void Model::disconnect(PGconn *connection)
{
    PQexec(connection, "END");
    PQfinish(connection);
}
