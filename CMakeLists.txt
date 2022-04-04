cmake_minimum_required(VERSION 3.10)
# set the project name
project(libAdvatek)

set(BOOST_ROOT "${PROJECT_SOURCE_DIR}/External/boost")
set(Boost_INCLUDE_DIR  "${PROJECT_SOURCE_DIR}/External/boost")
set(Boost_LIBRARY_DIR "${PROJECT_SOURCE_DIR}/External/boost/libs")


add_library(libAdvatek advatek_assistor.cpp udpclient.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_ROOT})
