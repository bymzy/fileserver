/* #define CROW_STATIC_DIRECTORY "staticdir" */
/* #define CROW_STATIC_ENDPOINT "/static/<path>" */
#define CROW_DISABLE_STATIC_DIR
#include "crow.h"
#include "cmdline.h"
#include <filesystem>

int main(int argc, char *argv[])
{
    cmdline::parser cmd;
    cmd.add<std::string>("host", 'h', "host name", false, "0.0.0.0");
    cmd.add<short>("port", 'p', "port number", false, 80, cmdline::range(1, 65535));
    cmd.add<std::string>("dirs", 'd', "static files dir", false, "./");
    cmd.add<std::string>("log_level", 'l', "log level: debug, info, warn", false, "info");
    cmd.add<int>("thread_count", 't', "thread cound", false, 2);
    cmd.footer("dir");
    cmd.parse_check(argc, argv);

    std::string host = cmd.get<std::string>("host");
    short port = cmd.get<short>("port");
    std::string log_level = cmd.get<std::string>("log_level");
    int thread_count = cmd.get<int>("thread_count");

    std::string dir= cmd.get<std::string>("dirs");

    // bash> fs dir 
    // for this case just read the replace the dir
    if (cmd.rest().size() > 0) {
        dir = cmd.rest()[0];
    }

    if (dir[dir.length() - 1] != '/') {
        dir += "/";
    }

    //Crow app initialization
    crow::SimpleApp app;

    app.concurrency(thread_count);

    app.loglevel(crow::LogLevel::Info);
    if (log_level == "debug") {
        app.loglevel(crow::LogLevel::Debug);
    } else if (log_level == "warn") {
        app.loglevel(crow::LogLevel::Warning);
    }

    CROW_ROUTE(app, "/")
    ([dir](const crow::request& req, crow::response& res) {
        res.set_static_file_info(dir + "index.html");
        res.end();
    });

    CROW_ROUTE(app, "/<path>")
    ([dir](const crow::request& req, crow::response& res, std::string file_name) {
        file_name = dir + file_name;
        if (!std::filesystem::exists(file_name) || !std::filesystem::is_regular_file(file_name)) {
            file_name = dir + "index.html";
        }
        res.set_static_file_info(file_name);
        res.end();
    });

    app.bindaddr(host).port(port).run();

    return 0;
}

/// You can also use the `/static` directory and endpoint (the directory needs to have the same path as your executable).
/// Any file inside the static directory will have the same path it would in your filesystem.

/// TO change the static directory or endpoint, use the macros above (replace `alternative_directory` and/or `alternative_endpoint` with your own)
