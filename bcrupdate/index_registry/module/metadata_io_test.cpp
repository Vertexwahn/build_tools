/*
 *  SPDX-FileCopyrightText: Copyright 2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "gmock/gmock.h"

#include "bcrupdate/index_registry/module/metadata_io.h"

using namespace bcrupdate;

TEST(metadata_io, print_metadata) {
    ModuleMetadata metadata = {
            "https://curl.haxx.se",
            {
                    {"julian.amann@tum.de", "Vertexwahn", "Julian Amann"}
            },
            {"github:curl/curl"},
            {
                    "7.69.1",
            },
            {}  // empty yanked_versions
    };

    std::stringstream ss;
    print_metadata(ss, metadata);

    EXPECT_THAT(ss.str(), testing::HasSubstr("7.69.1"));
}
