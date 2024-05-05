/*
 *  SPDX-FileCopyrightText: Copyright 2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "bcrupdate/index_registry/module/metadata.h"

#include <string_view>

namespace bcrupdate {
    void print_metadata(std::ostream& out, ModuleMetadata& metadata);
    void store_metadata(const std::string_view& filename, const ModuleMetadata& metadata);
}
