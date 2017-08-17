# SmartikoTestProject
## Prerequisites
* postgresql-libpq
* Eclipse Paho MQTT C client library (paho.mqtt.c)
* Eclipse Paho MQTT C++ client library (paho.mqtt.cpp)
* JSONcpp
* Cmake
## Build instructions
```
mkdir build
cd build
cmake ..
make
```
## Usage
Create a database using psql:
```
createdb -h localhost -p 5432 -U postgres Blog
psql -U postgres Blog
Blog=# CREATE SEQUENCE post_ids;
CREATE SEQUENCE
Blog=# CREATE TABLE posts ( id INTEGER PRIMARY KEY DEFAULT NEXTVAL('post_ids'), title VARCHAR(128), content VARCHAR(2048));
CREATE TABLE
```
Run MQTT server using port #1883

From SmartikoTestProject build directory, run in separate terminals:

```
./SmartikoTestProject-server <topic>
```
```
./SmartikoTestProject-client <topic>
```
Valid commands for the client are:

GET /posts/\<id\>

DELETE /posts/\<id\>

POST /posts/[id] {"title":"\<title\>", "content":"\<content\>"}
