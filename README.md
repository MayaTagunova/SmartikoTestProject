# SmartikoTestProject
## Prerequisites
* PostgreSQL libpq
* Eclipse Paho MQTT C client library (paho.mqtt.c)
* Eclipse Paho MQTT C++ client library (paho.mqtt.cpp)
* JSONcpp
* Cmake
* GNU libmicrohttpd
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
In another terminal, run:
```
telnet localhost 8888
```

Valid commands examples:

```
GET /posts/1 HTTP/1.0
```
```
DELETE /posts/1 HTTP/1.0
```
```
POST /posts HTTP/1.0
Content-Type: application/json
Content-Length: 48

{"title":"A new post", "content":"Some content"}
```
```
POST /posts/2 HTTP/1.0
Content-Type: application/json
Content-Length: 29

{"content":"Updated content"}
```
