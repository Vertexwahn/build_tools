/*
 *  SPDX-FileCopyrightText: Copyright 2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once

class CurlWrapper {
public:
    CurlWrapper() {
        // Initialize libcurl
        curl_global_init(CURL_GLOBAL_ALL);
    }

    ~CurlWrapper() {
        // Cleanup libcurl
        curl_global_cleanup();
    }

private:
};
