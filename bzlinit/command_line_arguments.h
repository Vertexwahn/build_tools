#pragma once

#include <iostream>

#include "boost/program_options.hpp"

using namespace boost::program_options;

class CommandLineArguments {
public:
    CommandLineArguments() {
        desc.add_options()
                ("workspace_path,w",  value<std::string>()->default_value(""), "Workspace path")
                ("version,v", "print version string")
                ("help,h", "Help screen");
    }

    bool handle_help() const {
        if (vm.count("help")) {
            std::cout << desc << std::endl;
            return true;
        }

        return false;
    }

    bool handle_version() const {
        if(vm.count("version")) {
            std::cout << "bzlinit 0.0.1-dirty" << std::endl;
            return true;
        }

        return false;
    }

    bool handle_standard_commands() const {
        if(handle_help()) {
            return true;
        }

        if(handle_version()) {
            return true;
        }

        return false;
    }

    std::string workspace_path() {
        return vm["workspace_path"].as<std::string>();
    }

    void parse_command_line_options(int argc, char **argv) {
        try {
            store(parse_command_line(argc, argv, desc), vm);
            notify(vm);
        } catch (const error &ex) {
            //LOG(ERROR) << ex.what();
            throw std::runtime_error(ex.what());
        }
    }

public:
    boost::program_options::variables_map vm;
    boost::program_options::options_description desc{"Options"};
};

