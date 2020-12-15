#include <bits/stdc++.h>
#include "crow/crow_all.h"
#include "json/json.hpp"
#include "PriorityQueue.h"
#include "utils.h"

using json = nlohmann::json;

PriorityQueue priorityQueue;

bool checkJson(crow::json::rvalue &j, vector<string> params) {
    if (!j) return false;
    for (string &param : params) {
        if (j.count(param) == 0) return false;
    }
    return true;
}

int main() {
    crow::App<SomeMiddleware> app;

    CROW_ROUTE(app, "/add").methods("POST"_method)([](const crow::request &req) {
        auto inputJson = crow::json::load(req.body);
        if (!checkJson(inputJson, {"uid", "priority", "payload"})) return crow::response(400);
        try {
            long long uid = inputJson["uid"].i();
            long long priority = inputJson["priority"].i();
            json payload = formatPayload(inputJson["payload"]);
            long long id = priorityQueue.insert(uid, priority, payload);
            std::ostringstream os;
            os << id;
            return crow::response{201, os.str()};
        } catch (...) { return crow::response(500); }
    });

    CROW_ROUTE(app, "/delete").methods("DELETE"_method)([](const crow::request &req) {
        string body = req.body;
        if (body.empty()) return crow::response(400);
        long long id = str2ll(body);
        try {
            if (priorityQueue.exists(id)) {
                priorityQueue.erase(id);
                return crow::response(204);
            }
            return crow::response(404);
        } catch (...) { return crow::response(500); }
    });

    CROW_ROUTE(app, "/get").methods("GET"_method)([](const crow::request &req) {
        string strId = req.url_params.get("id");
        if (strId.empty()) return crow::response(400);
        try {
            long long id = str2ll(strId);
            if (priorityQueue.exists(id)) {
                string outputStr = priorityQueue.get(id).toString();
                return crow::response{outputStr};
            }
            return crow::response(404);
        } catch (...) { return crow::response(500); }
    });

    CROW_ROUTE(app, "/getMax").methods("GET"_method)([] {
        try {
            if (priorityQueue.isEmpty()) return crow::response(404);
            return crow::response(priorityQueue.getMax().toString());
        } catch (...) { return crow::response(500); }
    });

    CROW_ROUTE(app, "/getUserNodes").methods("GET"_method)([](const crow::request &req) {
        string strUid = req.url_params.get("id");
        if (strUid.empty()) return crow::response(400);
        try {
            long long uid = str2ll(strUid);
            if (priorityQueue.userExists(uid)) {
                set<long long> outputSet = priorityQueue.getUserNodes(uid);
                std::ostringstream os;
                for (long long i : outputSet) os << i << "; ";
                string osStr = os.str();
                return crow::response{osStr.substr(0, osStr.size()-2)};
            }
            return crow::response(404);
        } catch (...) { return crow::response(500); }
    });

    CROW_ROUTE(app, "/changeUserNodes").methods("POST"_method)([](const crow::request &req) {
        auto inputJson = crow::json::load(req.body);
        if (!checkJson(inputJson, {"uid", "priority"})) return crow::response(400);
        try {
            long long uid = inputJson["uid"].i();
            if (priorityQueue.userExists(uid)) {
                long long priority = inputJson["priority"].i();
                vector<long long> outputVector = priorityQueue.changeUserNodes(uid, priority);
                std::ostringstream os;
                for (long long i : outputVector) os << i << "; ";
                string osStr = os.str();
                return crow::response{osStr.substr(0, osStr.size()-2)};
            }
            return crow::response(404);
        } catch (...) { return crow::response(500); }
    });

    app.port(8080).run();
}