#include <bits/stdc++.h>
#include "crow/crow_all.h"
#include "json/json.hpp"
#include "PriorityQueue.h"

using json = nlohmann::json;

PriorityQueue priorityQueue = PriorityQueue("queue.bson");

// initialise random generator
std::random_device rd;
std::mt19937_64 eng(rd());
std::uniform_int_distribution<uint32_t> distr;

int main() {
    crow::App<SomeMiddleware> app;

    CROW_ROUTE(app, "/add").methods("POST"_method)([](const crow::request &req) {
        auto inputJson = crow::json::load(req.body);
        if (!validJson(inputJson, {"uid", "priority", "payload"})) return crow::response(400);
        try {
            uint32_t uid = inputJson["uid"].u();
            uint32_t priority = inputJson["priority"].u();
            json payload = formatPayload(inputJson["payload"]);
            uint32_t id = priorityQueue.insert(uid, priority, payload, distr(eng));
            std::ostringstream os;
            os << id;
            return crow::response{201, os.str()};
        } catch (exception &e) {
            std::cout << e.what() << '\n';
            return crow::response(500);
        }
        catch (...) {
            std::cout << "Unknown error\n";
            return crow::response(500);
        }
    });

    CROW_ROUTE(app, "/delete").methods("DELETE"_method)([](const crow::request &req) {
        string body = req.body;
        if (body.empty()) return crow::response(400);
        try {
            uint32_t id = str2ul(body);
            if (priorityQueue.exists(id)) {
                priorityQueue.erase(id);
                return crow::response(204);
            }
            return crow::response(404);
        } catch (exception &e) {
            std::cout << e.what() << '\n';
            return crow::response(500);
        }
        catch (...) {
            std::cout << "Unknown error\n";
            return crow::response(500);
        }
    });

    CROW_ROUTE(app, "/get").methods("GET"_method)([](const crow::request &req) {
        string strId = req.url_params.get("id");
        if (strId.empty()) return crow::response(400);
        try {
            uint32_t id = str2ul(strId);
            if (priorityQueue.exists(id)) {
                string outputStr = priorityQueue.get(id).toString();
                return crow::response{outputStr};
            }
            return crow::response(404);
        } catch (exception &e) {
            std::cout << e.what() << '\n';
            return crow::response(500);
        }
        catch (...) {
            std::cout << "Unknown error\n";
            return crow::response(500);
        }
    });

    CROW_ROUTE(app, "/getMax").methods("GET"_method)([] {
        try {
            priorityQueue.print();
            if (priorityQueue.isEmpty()) return crow::response(404);
            return crow::response(priorityQueue.getMax().toString());
        } catch (exception &e) {
            std::cout << e.what() << '\n';
            return crow::response(500);
        }
        catch (...) {
            std::cout << "Unknown error\n";
            return crow::response(500);
        }
    });

    CROW_ROUTE(app, "/getUserNodes").methods("GET"_method)([](const crow::request &req) {
        string strUid = req.url_params.get("uid");
        if (strUid.empty()) return crow::response(400);
        try {
            uint32_t uid = str2ul(strUid);
            if (priorityQueue.userExists(uid)) {
                set<uint32_t> outputSet = priorityQueue.getUserNodes(uid);
                std::ostringstream os;
                for (auto i : outputSet) os << i << "; ";
                string osStr = os.str();
                return crow::response{osStr.substr(0, osStr.size() - 2)};
            }
            return crow::response(404);
        } catch (exception &e) {
            std::cout << e.what() << '\n';
            return crow::response(500);
        }
        catch (...) {
            std::cout << "Unknown error\n";
            return crow::response(500);
        }
    });

    CROW_ROUTE(app, "/changeUserNodes").methods("POST"_method)([](const crow::request &req) {
        auto inputJson = crow::json::load(req.body);
        if (!validJson(inputJson, {"uid", "priority"})) return crow::response(400);
        try {
            uint32_t uid = inputJson["uid"].u();
            if (priorityQueue.userExists(uid)) {
                uint32_t priority = inputJson["priority"].u();
                vector<uint32_t> outputVector = priorityQueue.changeUserNodes(uid, priority);
                std::ostringstream os;
                for (auto i : outputVector) os << i << "; ";
                string osStr = os.str();
                return crow::response{osStr.substr(0, osStr.size() - 2)};
            }
            return crow::response(404);
        } catch (exception &e) {
            std::cout << e.what() << '\n';
            return crow::response(500);
        }
        catch (...) {
            std::cout << "Unknown error\n";
            return crow::response(500);
        }
    });

    app.port(8080).run();
}