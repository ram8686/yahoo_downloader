cmake_minimum_required(VERSION 3.16)
project(YahooDownloader)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

add_executable(yahoo_downloader
    main.cpp
    downloader.cpp
    downloader_worker.cpp
    config_manager.cpp
    fetch_nse_tickers.cpp
    utils/csv_writer.cpp
    utils/file_utils.cpp
)

target_include_directories(yahoo_downloader PRIVATE
    .
    utils
)

target_link_libraries(yahoo_downloader PRIVATE
    CURL::libcurl
    nlohmann_json::nlohmann_json
    Qt6::Core
    Qt6::Widgets
)