/*
 *  SPDX-FileCopyrightText: Copyright 2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <string>
#include <vector>

namespace bcrupdate {
    struct Maintainer {
        std::string email;
        std::string github;
        std::string name;
    };

    struct ModuleMetadata {
        std::string homepage;
        std::vector<Maintainer> maintainers;
        std::vector<std::string> repository;
        std::vector<std::string> versions;
        std::vector<std::string> yanked_versions;
    };
}
