cmake_minimum_required(VERSION 3.16)

project(helloworld VERSION 1.0.0 LANGUAGES CXX)

set(PROGRAM_NAME helloworld)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}/CONAN")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES ${PROGRAM_NAME}_en_GB.ts ${PROGRAM_NAME}_en_US.ts ${PROGRAM_NAME}_nb_NO.ts)

set(SOURCE_FILES main.cpp)

add_executable(${PROGRAM_NAME}
    ${SOURCE_FILES}
    ${TS_FILES}
)

qt6_add_translations(${PROGRAM_NAME} TS_FILES ${TS_FILES})
qt6_create_translation(QM_FILES ${SOURCE_FILES} ${TS_FILES})


target_link_libraries(helloworld PRIVATE Qt6::Widgets)