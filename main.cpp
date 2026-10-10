#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/uri.hpp>

int main() {
    std::string stage = "initializing driver";

    try {
        // Keep this alive for the entire program.
        mongocxx::instance instance{};

        const char* envUri = std::getenv("MONGODB_URI");

        if (envUri == nullptr || envUri[0] == '\0') {
            std::cerr << "MONGODB_URI is not set.\n";
            return 1;
        }

        std::string connectionString{envUri};

        if (connectionString.rfind("mongodb+srv://", 0) != 0 &&
            connectionString.rfind("mongodb://", 0) != 0) {
            std::cerr
                << "MONGODB_URI must begin with mongodb+srv:// "
                << "or mongodb://\n";
            return 1;
        }

        stage = "parsing connection string";
        mongocxx::uri uri{connectionString};
        std::cout << "Connection string parsed.\n";

        stage = "creating MongoDB client";
        mongocxx::client client{uri};

        stage = "checking connection";
        auto ping = bsoncxx::from_json(R"({"ping": 1})");
        client["admin"].run_command(ping.view());
        std::cout << "Connected to MongoDB successfully.\n";

        auto students = client["learning_db"]["students"];

        // {} means display all students.
        auto filter = bsoncxx::from_json("{}");

        // Display students alphabetically by name.
        auto sort = bsoncxx::from_json(R"({"name": 1})");

        mongocxx::options::find options{};
        options.sort(sort.view());

        stage = "reading learning_db.students";

        std::size_t count = 0;

        for (auto&& document : students.find(filter.view(), options)) {
            std::cout << "\nStudent " << ++count << ":\n";
            std::cout << bsoncxx::to_json(document) << '\n';
        }

        std::cout << "\nFound " << count << " student(s).\n";

        if (count == 0) {
            std::cout
                << "Check that the URI points to the same cluster "
                << "you use in Compass.\n";
        }

        return 0;
    } catch (const std::exception& error) {
        std::cerr << "\nFailed while " << stage << ".\n";
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}