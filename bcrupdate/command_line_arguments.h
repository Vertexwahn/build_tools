/*
 *  SPDX-FileCopyrightText: Copyright 2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "boost/program_options.hpp"
using namespace boost::program_options;

#include <iostream>
#include <fstream>
#include <filesystem>

namespace bcrupdate {
    class CommandLineArguments {
    public:
        CommandLineArguments() {
            desc.add_options()
                ("module,m", boost::program_options::value<std::string>()->required(),  "Define the module that should be updated")
                ("git_hub_access_token", boost::program_options::value<std::string>()->required(),  "GitHub Access Token")
                ("bazel_central_registry_directory,d", boost::program_options::value<std::string>()->required(),  "Directory where BCR should be cloned via Git")
                ("use_proxy", boost::program_options::value<bool>()->default_value(false), "Use proxy")
                ("proxy_url", boost::program_options::value<std::string>(), "Proxy URL")
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
                std::cout << "bcrupdate 0.0.1-dirty" << std::endl;
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

        bool has_module_param() const {
            return vm.count("module");
        }

        std::string git_hub_access_token() const {
            if(vm.count("git_hub_access_token")) {
                return vm["git_hub_access_token"].as<std::string>();
            }

            return "InvalidToken";
        }

        std::string bazel_central_registry_directory() const {
            if(vm.count("bazel_central_registry_directory")) {
                return vm["bazel_central_registry_directory"].as<std::string>();
            }

            return "InvalidDirectory";
        }

        std::string module() const {
            if(vm.count("module")) {
                return vm["module"].as<std::string>();
            }

            return "";
        }

        bool use_proxy() const {
            return vm["use_proxy"].as<bool>();
        }

        bool parse_command_line_options(int argc, char **argv) {
            try {
                store(parse_command_line(argc, argv, desc), vm);

                if(handle_standard_commands()) {
                    return true;
                }

                notify(vm);
            } catch (const error &ex) {
                std::cout << desc << std::endl;
                //LOG(ERROR) << ex.what();
                throw std::runtime_error(ex.what());
            }

            return false;
        }

    public:
        boost::program_options::variables_map vm;
        boost::program_options::options_description desc{"Options"};
    };
}
