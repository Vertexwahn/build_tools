/*
 *  SPDX-FileCopyrightText: Copyright 2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "filesystem.h"

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace bcrupdate {
    void create_directory(std::string_view directory_path) {
        try {
            if (std::filesystem::exists(directory_path)) {
                throw std::runtime_error("Directory already exists: " + std::string(directory_path));
            }

            if (!std::filesystem::create_directory(directory_path)) {
                throw std::runtime_error("Failed to create directory: " + std::string(directory_path));
            }

            std::cout << "Directory created: " << directory_path << std::endl;
        } catch (const std::exception& ex) {
            std::cerr << "Exception caught: " << ex.what() << std::endl;
            throw; // rethrow the exception
        }
    }

    std::vector<std::string> get_files_in_directory(const std::string& directory) {
        std::vector<std::string> files;
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().filename().string());
            }
        }
        return files;
    }
}
