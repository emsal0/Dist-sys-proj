#include "connections.hpp"
using json = nlohmann::json;
// Checks if it's a valid message
json recv_spec_msg(zmq::message_t request, zmq::socket_t socket, std::map<std::string, zmq::socket_t *> *curr) { 
    socket.recv(&request);
    try {
        std::string rpl = std::string(static_cast<char *>(request.data()),request.size());
        json j = json(rpl);
        store_connection(j);
        return j;
        std::string ip = j["ip"];
        (*curr)[ip] = &socket;
    } catch (std::exception& e) {
        json j = {
            {"error", e.what()}
        };
        return j;
    }
}

// store connections in a redis
void store_connection(json j) {
    redis3m::connection::ptr_t conn = redis3m::connection::create();
    std::string ip = j["ip"];
    std::string whole_thing = j.dump();

    conn->run(redis3m::command("SET") << ip << whole_thing);
}

// delete a connection on disconnect
void delete_connection(json j) {
    redis3m::connection::ptr_t conn = redis3m::connection::create();
    std::string ip = j["ip"];
    redis3m::reply r = conn->run(redis3m::command("DEL") << ip);
}
