/*
 *  SPDX-FileCopyrightText: Copyright 2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <vector>
#include <string>

namespace bcrupdate {
    void create_directory(std::string_view directory_path);
    std::vector<std::string> get_files_in_directory(const std::string& directory);
}
