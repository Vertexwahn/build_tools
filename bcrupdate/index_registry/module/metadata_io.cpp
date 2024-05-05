/*
 *  SPDX-FileCopyrightText: Copyright 2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "bcrupdate/index_registry/module/metadata.h"
#include "bcrupdate/index_registry/module/metadata_io.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <iostream>

namespace bcrupdate {
    namespace internal {
        json createMetadataJson(const ModuleMetadata& metadata) {
            // Convert the C++ data structure to JSON
            json jsonData;
            jsonData["homepage"] = metadata.homepage;

            for (const auto& maintainer : metadata.maintainers) {
                jsonData["maintainers"].push_back({
                                                          {"email", maintainer.email},
                                                          {"github", maintainer.github},
                                                          {"name", maintainer.name}
                                                  });
            }

            jsonData["repository"] = metadata.repository;
            jsonData["versions"] = metadata.versions;
            jsonData["yanked_versions"] = metadata.yanked_versions;

            return jsonData;
        }
    }

    void print_metadata(std::ostream& out, ModuleMetadata& metadata) {
        json jsonData = internal::createMetadataJson(metadata);

        // Print the JSON data
        out << jsonData.dump(4) << std::endl;
    }

    void store_metadata(const std::string_view& filename, const ModuleMetadata& metadata) {

    }
}
