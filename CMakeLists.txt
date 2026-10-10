cmake_minimum_required(VERSION 3.15)
project(MongoExample LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(mongocxx CONFIG REQUIRED)
find_package(bsoncxx CONFIG REQUIRED)

add_executable(mongo_example main.cpp)

target_link_libraries(mongo_example PRIVATE
    mongo::mongocxx_shared
    mongo::bsoncxx_shared
)