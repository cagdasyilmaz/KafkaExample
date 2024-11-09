cmake_minimum_required(VERSION 3.28)
project(KafkaExample)

set(CMAKE_CXX_STANDARD 20)

#set(CMAKE_CXX_COMPILER "/usr/bin/g++-13")
#set(CMAKE_CXX_COMPILER "/usr/bin/clang++-18")
#set(CMAKE_CXX_COMPILER "/opt/intel/oneapi/2024.2/bin/icpx")

add_compile_options(-D_GLIBCXX_USE_CXX11_ABI=1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

# cmake -DCMAKE_BUILD_TYPE = DEBUG | RELEASE | RELWITHDEBINFO | MINSIZEREL ...
set(CMAKE_BUILD_TYPE RELEASE)
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Find librdkafka
find_package(PkgConfig REQUIRED)
pkg_check_modules(RDKAFKA REQUIRED IMPORTED_TARGET rdkafka)

# Find required protobuf package

find_package(protobuf CONFIG REQUIRED)
find_package(Threads REQUIRED)

find_package(glog 0.7.1 REQUIRED)

include_directories(/include)

add_executable(KafkaProducer KafkaProducer.cpp
                             src/Logger.cpp
                             src/Vocabulary.pb.cc
                             src/JSONDatabaseVocabulary.cpp
                             src/KafkaProducerWrapper.cpp)
target_link_libraries(KafkaProducer PRIVATE Threads::Threads PkgConfig::RDKAFKA protobuf::libprotobuf glog::glog)

add_executable(KafkaConsumer KafkaConsumer.cpp
                             src/Logger.cpp
                             src/KafkaConsumerWrapper.cpp
                             src/Vocabulary.pb.cc)
target_link_libraries(KafkaConsumer PRIVATE Threads::Threads PkgConfig::RDKAFKA protobuf::libprotobuf glog::glog)

add_executable(KafkaAdminClient KafkaAdminClient.cpp)
target_link_libraries(KafkaAdminClient PRIVATE Threads::Threads PkgConfig::RDKAFKA protobuf::libprotobuf)

#add_executable(KafkaManualCommitConsumer KafkaManualCommitConsumer.cpp)
#target_link_libraries(KafkaManualCommitConsumer PRIVATE Threads::Threads PkgConfig::RDKAFKA)
