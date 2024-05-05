/*
 *  SPDX-FileCopyrightText: Copyright 2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "string.h"
#include <string>

namespace bcrupdate {
    void replace(std::string& str, const std::string& pattern, const std::string& replacement) {
        size_t pos = 0;
        while ((pos = str.find(pattern, pos)) != std::string::npos) {
            str.replace(pos, pattern.length(), replacement);
            pos += replacement.length();
        }
    }
};
