/*
 *  SPDX-FileCopyrightText: Copyright 2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <string>

namespace bcrupdate {
    void replace(std::string& str, const std::string& pattern, const std::string& replacement);
};
