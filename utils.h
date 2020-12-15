#include "crow/crow_all.h"
#include "json/json.hpp"

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

long long str2ll(string str) {
    std::string::size_type sz = 0;
    long long id = std::stoll(str, &sz, 0);
    return id;
}