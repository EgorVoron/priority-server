#include <bits/stdc++.h>
#include "crow/crow_all.h"
#include "json/json.hpp"

using nlohmann::json;

struct SomeMiddleware {
    std::string message;
    struct context {
    };

    void before_handle(crow::request & /*req*/, crow::response & /*res*/, context & /*ctx*/) {
        CROW_LOG_DEBUG << " - MESSAGE: " << message;
    }

    void after_handle(crow::request & /*req*/, crow::response & /*res*/, context & /*ctx*/) {}
};

json formatPayload(const crow::json::rvalue &x) {
    std::ostringstream os;
    os << x;
    return json::parse(os.str());
}

long long str2ll(std::string &str) {
    std::string::size_type sz = 0;
    long long id = std::stoll(str, &sz, 0);
    return id;
}

bool correctFile(std::string &path) {
    std::fstream fileStream;
    fileStream.open(path);
    return (!fileStream.fail() && fileStream.peek() != std::ifstream::traits_type::eof());
}

bool checkJson(crow::json::rvalue &j, std::vector<std::string> params) {
    if (!j) return false;
    for (std::string &param : params) {
        if (j.count(param) == 0) return false;
    }
    return true;
}
