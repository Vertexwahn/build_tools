/*
 *  SPDX-FileCopyrightText: Copyright 2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "git_client.h"

namespace bcrupdate {
    void checkout_progress(
            const char *path,
            size_t cur,
            size_t tot,
            void *payload) {
        std::cout << "Cloning: " << path << " " << cur << "/" << tot << " bytes" << std::endl;
    }
}
