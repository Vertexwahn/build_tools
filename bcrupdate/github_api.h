/*
 *  SPDX-FileCopyrightText: Copyright 2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "base64.h"
#include "semver.hpp"

#include <curl/curl.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "fmt/core.h"
#include "fmt/format.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>
#include <sstream>

namespace bcrupdate {
    class GitHubApi {
    public:
        explicit GitHubApi(std::string_view git_hub_access_token) : GITHUB_ACCESS_TOKEN{git_hub_access_token} {
            // Initialize libcurl
            curl_global_init(CURL_GLOBAL_ALL);
        }

        ~GitHubApi() {
            // Cleanup libcurl
            curl_global_cleanup();
        }

        static size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* buffer) {
            size_t total_size = size * nmemb;
            buffer->append((char*)contents, total_size);
            return total_size;
        }

        std::string fetch_file(std::string_view url) {
            void* curl;
            curl = curl_easy_init();
            curl_easy_setopt(curl, CURLOPT_URL, url.data());
            // example.com is redirected, so we tell libcurl to follow redirection
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1); //Prevent "longjmp causes uninitialized stack frame" bug
            curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "bcrupdate/0.0.1");
            std::stringstream out;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, GitHubApi::write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
            // Perform the request, res will get the return code
            CURLcode res = curl_easy_perform(curl);
            // Check for errors
            if (res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n",
                        curl_easy_strerror(res));
            }
            curl_easy_cleanup(curl);
            return out.str();
        }

        std::string latest_sha_hash(std::string_view owner, std::string_view repo, std::string_view branch_name) {

            std::string url = GITHUB_API_URL + "/repos/" + std::string(owner) + "/" + std::string(repo) + "/branches/" + std::string(branch_name);

            std::string response = get_request(url);

            json jsonResponse = json::parse(response);

            return  jsonResponse["commit"]["sha"];
        }

        std::string create_branch(std::string_view owner, std::string_view repo, std::string_view branch_name,
                                  std::string_view base_commit_sha) {
            std::string url = GITHUB_API_URL + "/repos/" + std::string(owner.data()) + "/" + std::string(repo) + "/git/refs";

            std::string BASE_COMMIT_SHA = std::string(base_commit_sha);
            std::string payload = "{ \"ref\": \"refs/heads/" + std::string(branch_name) + "\", \"sha\": \"" + BASE_COMMIT_SHA + "\" }";

            CURL *curl = curl_easy_init();
            if (curl) {
                // Set headers
                curl_slist* headers = nullptr;

                // Set authorization header
                std::string token = GITHUB_ACCESS_TOKEN;
                std::string auth_header = "Authorization: token " + token;

                headers = curl_slist_append(headers, auth_header.c_str());
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

                // Set User-Agent header
                std::string user_agent = "User-Agent: Bzlbot";
                headers = curl_slist_append(headers, user_agent.c_str());

                // Set options for the POST request
                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
                curl_easy_setopt(curl, CURLOPT_POST, 1L);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, GitHubApi::write_callback);

                // Response buffer
                std::string response;
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

                // Perform the POST request
                CURLcode res = curl_easy_perform(curl);

                std::cout << response << std::endl;

                // Check for errors
                if (res != CURLE_OK) {
                    std::cerr << "Failed to perform request: " << curl_easy_strerror(res) << std::endl;
                } else {
                    std::cout << "Merge request successful." << std::endl;
                }

                // Cleanup
                curl_slist_free_all(headers);
                curl_easy_cleanup(curl);

                return response;
            }

            return std::string("curl init failed");
        }

        std::string merge_upstream(std::string_view owner, std::string_view repo, std::string_view branch_name) {
            const std::string GITHUB_API_URL = "https://api.github.com";

            std::string url = GITHUB_API_URL + "/repos/" + std::string(owner.data()) + "/" + std::string(repo) + "/merge-upstream";
            std::string payload = "{ \"branch\": \"" + std::string(branch_name) + "\" }";

            CURL *curl = curl_easy_init();
            if (curl) {
                // Set headers
                curl_slist* headers = nullptr;

                // Set authorization header
                std::string token = GITHUB_ACCESS_TOKEN;
                std::string auth_header = "Authorization: token " + token;

                headers = curl_slist_append(headers, auth_header.c_str());
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

                // Set User-Agent header
                std::string user_agent = "User-Agent: Bzlbot";
                headers = curl_slist_append(headers, user_agent.c_str());

                // Set options for the POST request
                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
                curl_easy_setopt(curl, CURLOPT_POST, 1L);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, GitHubApi::write_callback);

                // Response buffer
                std::string response;
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

                // Perform the POST request
                CURLcode res = curl_easy_perform(curl);

                std::cout << response << std::endl;

                // Check for errors
                if (res != CURLE_OK) {
                    std::cerr << "Failed to perform request: " << curl_easy_strerror(res) << std::endl;
                } else {
                    std::cout << "Merge request successful." << std::endl;
                }

                // Cleanup
                curl_slist_free_all(headers);
                curl_easy_cleanup(curl);

                return response;
            }

            return std::string("curl init failed");
        }

        std::string content(const std::string& owner, const std::string& repo, const std::string& path) {
            //std::string url = "https://github.com/AcademySoftwareFoundation/openexr/raw/main/MODULE.bazel";
            //std::string url = std::string("https://api.github.com/repos/") + owner + std::string("/") + repo  + std::string("/contents/") + path;
            std::string url = std::string("https://github.com/") + owner + std::string("/") + repo  + std::string("/raw/main/") + path;
            std::string response = fetch_file(url);
            return response;
            /*
            std::string apiUrl = "https://api.github.com/repos/" + owner + "/" + repo + "/contents/" + path;

            std::string response = get_request(apiUrl);
            json jsonResponse = json::parse(response);

            if (jsonResponse.find("content") != jsonResponse.end()) {
                std::string content = jsonResponse["content"];
                return base64_decode(content);  // does not work since MIME is used here
            } else {
                throw std::runtime_error("Failed to fetch file content from GitHub API");
            }
             */
        }

        std::string latest_tag(std::string_view owner, std::string_view repo) {
            try {
                std::string url = fmt::format("https://api.github.com/repos/{}/{}/tags", owner, repo);
                std::string response = get_request(url);
                json json_response = json::parse(response);

                // TODO: try this -> https://chat.openai.com/c/7b7f2a87-757c-41f8-b6cd-38d09c468ce0

                /*
                // Check if the response is an array and non-empty
                if (!json_response.is_array() || json_response.empty()) {
                    throw std::runtime_error("No tags found for the repository.");
                }

                std::vector<semver::version> tags;

                // Extract tag names and store them in the vector
                for (const auto& tag : json_response) {
                    try {
                        tags.push_back(semver::version::parse(tag["name"]));
                    }
                    catch (std::exception& ex) {
                        std::cerr << ex.what() << std::endl;
                    }
                }
                std::sort(std::rbegin(tags), std::rend(tags));

                return tags[0].str();

                 */
                return json_response[0]["name"];
            }
            catch (std::exception& ex) {
                std::cerr << ex.what() << std::endl;
                throw;
            }
        }

        bool has_release_packages(std::string_view owner, std::string_view repo) {
            try {
                std::string url = fmt::format("https://api.github.com/repos/{}/{}/releases", owner, repo);
                std::string response = get_request(url);
                json json_response = json::parse(response);

                if (!json_response.empty())
                    return true;
                else
                    return false;
            }
            catch (std::exception& ex) {
                std::cout << ex.what() << std::endl;
                throw;
            }
        }

        std::string latest_release(std::string_view owner, std::string_view repo) {
            try {
                std::string url = fmt::format("https://api.github.com/repos/{}/{}/releases/latest", owner, repo);
                std::string response = get_request(url);
                json json_response = json::parse(response);

                return json_response["name"];
            }
            catch (std::exception& ex) {
                std::cout << ex.what() << std::endl;
                throw;
            }
        }

    private:
        static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
            using namespace std;
            string data((const char*) ptr, (size_t) size * nmemb);
            *((stringstream*) stream) << data;
            return size * nmemb;
        }

        std::string get_request(std::string_view url) {
            CURL* curl = curl_easy_init();
            if (curl) {
                // URL to GitHub's API endpoint
                //std::string url = "https://api.github.com/repos/bazelbuild/bazel/releases/latest";
                //std::string url = "https://api.github.com/repos/AcademySoftwareFoundation/openexr/releases/latest";
                //std::string url = "https://api.github.com/repos/bazelbuild/bazel/contents";

                // Set URL and callback function
                curl_easy_setopt(curl, CURLOPT_URL, url.data());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, GitHubApi::write_callback);

                // Response data will be stored here
                std::string response_data;

                // Set response data buffer
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

                // Set authorization header
                std::string token = GITHUB_ACCESS_TOKEN;
                std::string auth_header = "Authorization: token " + token;
                struct curl_slist* headers = nullptr;
                headers = curl_slist_append(headers, auth_header.c_str());
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

                // Set User-Agent header
                std::string user_agent = "User-Agent: Bzlbot";
                headers = curl_slist_append(headers, user_agent.c_str());

                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

                // Perform the request
                CURLcode res = curl_easy_perform(curl);

                if (res != CURLE_OK) {
                    std::cerr << "Failed to perform curl request: " << curl_easy_strerror(res) << std::endl;
                }

                // Clean up
                curl_easy_cleanup(curl);

                return response_data;
            } else {
                std::cerr << "Failed to initialize curl." << std::endl;
            }

            return std::string("curl init failed");
        }

    private:
        const std::string GITHUB_API_URL = "https://api.github.com";
        std::string GITHUB_ACCESS_TOKEN;
    };
}
