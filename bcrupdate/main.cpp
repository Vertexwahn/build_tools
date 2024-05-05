/*
 *  SPDX-FileCopyrightText: Copyright 2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "command_line_arguments.h"
#include "git_client.h"
#include "github_api.h"
#include "string.h"
#include "filesystem.h"

#include <nlohmann/json.hpp>

#include <git2.h>

#include <curl/curl.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>

using namespace bcrupdate;

void update_integrity(const std::string& directory, const std::string& module) {
    std::string command = "cd " + directory + " && bazel run //tools:update_integrity -- " + module;
    int exitCode = std::system(command.c_str());

    if (exitCode != 0) {
        std::cerr << "Error: Failed to run 'bazel info' command." << std::endl;
    }
}

void git_switch_branch(const std::string& directory, const std::string& branch) {
    std::string command = "cd " + directory + " && git switch " + branch;
    int exitCode = std::system(command.c_str());

    if (exitCode != 0) {
        std::cerr << "Error: Failed to run 'bazel info' command." << std::endl;
    }
}

void commit_changes(const std::string& directory, const std::string& commit_message) {
    std::string command = "cd " + directory + " && git add . && git commit -m " + "\"" + commit_message + "\"" + " && git push";
    int exitCode = std::system(command.c_str());

    if (exitCode != 0) {
        std::cerr << "Error: Failed to run 'bazel info' command." << std::endl;
    }
}

void create_file_from_template(
        const std::string& template_filename,
        const std::string& out_filename,
        const std::string& version) {
    std::ifstream infile(template_filename);
    if (!infile) {
        std::cerr << "Error: Unable to open file " << template_filename << std::endl;
        return;
    }

    std::string content((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    infile.close();

    replace(content, "@VERSION@", version);

    // Write modified content to the new file
    std::string new_filename = out_filename;
    std::ofstream outfile(new_filename);
    if (!outfile) {
        std::cerr << "Error: Unable to create file " << new_filename << std::endl;
        return;
    }

    outfile << content;
    outfile.close();
}

void create_module_dot_file(
        const std::string& directory,
        const std::string& module,
        const std::string& version) {
    std::string template_path = "bcrupdate/modules/" + module + "/MODULE.bazel";

    std::string new_filename = directory + "/" + "MODULE.bazel";

    create_file_from_template(template_path, new_filename, version);
}

void create_presubmit_yml_file(const std::string& directory,
                               const std::string& module) {
    std::string template_path = "bcrupdate/modules/" + module + "/presubmit.yml";
    std::ifstream infile(template_path);
    if (!infile) {
        std::cerr << "Error: Unable to open file " << template_path << std::endl;
        return;
    }

    std::string content((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    infile.close();

    // Write modified content to the new file
    std::string new_filename = directory + "/" + "presubmit.yml";
    std::ofstream outfile(new_filename);
    if (!outfile) {
        std::cerr << "Error: Unable to create file " << new_filename << std::endl;
        return;
    }

    outfile << content;
    outfile.close();
}

void create_module_source_json_file(
    const std::string& directory,
    const std::string& module,
    const std::string& version) {
    std::string template_path = "bcrupdate/modules/" + module + "/source.json";

    std::string new_filename = directory + "/" + "source.json";

    create_file_from_template(template_path, new_filename, version);
}

void update_module_hosted_on_github(GitHubApi &api,
                                    GitClient &git_client,
                                    std::string module,
                                    const std::string& github_module_repo,
                                    const std::string& github_module_repo_owner,
                                    const std::string& strip_prefix,
                                    const std::string& path_to_bcr,
                                    const std::string& version) {
    std::cout << module << " update" << std::endl;

    std::string GIT_HUB_USER = "Vertexwahn";

    // now make an own branch for module update
    std::string branch = module + "-" + version;
    std::cout << "Create branch: " << branch << std::endl;

    std::string base_commit_sha = api.latest_sha_hash(GIT_HUB_USER, "bazel-central-registry", "main");

    api.create_branch(GIT_HUB_USER, "bazel-central-registry", branch, base_commit_sha);

    // Clone repo and branch

    git_client.clone(
            fmt::format("https://github.com/{}/bazel-central-registry.git", GIT_HUB_USER),
            path_to_bcr);
    git_switch_branch(path_to_bcr, branch);

    // No edit everything
    std::string path = path_to_bcr + std::string("/modules/") + module + std::string("/") + version;
    create_directory(path);

    create_module_dot_file(path, module, version);
    create_presubmit_yml_file(path, module);
    create_module_source_json_file(path, module, version);

    create_directory(path + "/patches");

    auto patch_files =  get_files_in_directory(fmt::format("bcrupdate/modules/{}/patches", module));

    for(std::string& patch_filename : patch_files) {
        create_file_from_template(
                fmt::format("bcrupdate/modules/{}/patches/{}", module, patch_filename),
                fmt::format("{}/patches/{}", path, patch_filename),
                version
        );
    }

    update_integrity(path_to_bcr, module);

    commit_changes(path_to_bcr, std::string("Add ") + module + std::string(" ") + version);
}

struct ModuleDescription {
    std::string module;
    std::string owner;
    std::string repo;
    std::string version_strip_prefix; // need format in version X.Y.Z - sometimes "v" or something else need to be stripped away..
    std::string last_known_version;
    std::vector<std::string> tags;
};

// TODO: Get this info from BCR
// TODO: This is very specific to GitHub (does not work for GitLab)
std::vector<ModuleDescription> repos {
    //{"cpplint", "cpplint", "cpplint", "", "1.6.1", {"C++", "lint", "tool"}},
    //{"jsoncpp", "open-source-parsers", "jsoncpp", "", "1.9.5", {"C++", "library"}},
    //{"module", "owner", "repo", "version_strip_prefix", "1.5.5", {"C++", "library"}},
    //{"msgpack-c", "msgpack", "msgpack-c", "", "6.1.0", {"C++", "library"}},
    //{"tinyobjloader", "tinyobjloader", "tinyobjloader", "", "2.0.0-rc1", {"C++", "library"}},
    {"abseil-cpp", "abseil", "abseil-cpp", "", "Abseil LTS branch, Jan 2024, Patch 1", {"C++", "library"}},
    {"brotli", "google", "brotli", "v", "1.1.0", {"C++", "library", "compression"}},
    {"catch2", "catchorg", "Catch2", "v", "3.5.2", {"C++", "library"}},
    {"cpptoml", "skystrife", "cpptoml", "v", "0.1.1", {"C++", "library"}},
    {"doctest", "doctest", "doctest", "v", "2.4.11", {"C++", "library"}},
    //{"embree", "embree", "embree", "Embree v", "4.3.0", {"C++", "library"}},
    {"fmt", "fmtlib", "fmt", "", "10.2.1", {"C++", "library"}},
    {"foonathan_memory", "foonathan", "memory", "Version ", "0.7-3", {"C++", "library"}},
    {"gflags", "gflags", "gflags", "gflags ", "2.2.2", {"C++", "library"}},
    {"glm", "g-truc", "glm", "GLM ", "0.9.9.8", {"C++", "library"}},
    {"glog", "google", "glog", "google-glog ", "0.6.0", {"C++", "library"}},
    {"google_benchmark", "google", "benchmark", "v", "1.8.3", {"C++", "library"}},
    {"googletest", "google", "googletest", "v", "1.14.0", {"C++", "library"}},
    {"imath", "AcademySoftwareFoundation", "Imath", "v", "3.1.10", {"C++", "library"}},
    {"libdeflate", "ebiggers", "libdeflate", "v", "1.20.bcr.1", {"C++", "library"}},
    {"libwebp", "webmproject", "libwebp", "v", "1.3.2", {"C++", "library"}},
    {"libyaml", "yaml", "libyaml", "v", "0.2.5", {"C++", "library"}},
    {"nanoflann", "jlblancoc", "nanoflann", "Release of v", "1.5.4", {"C++", "library"}},
    {"nlohmann_json", "nlohmann", "json", "JSON for Modern C++ version ", "3.11.3", {"C++", "library"}},
    {"onetbb", "oneapi-src", "oneTBB", "oneTBB ", "2021.11.0", {"C++", "library"}},
    {"onnx", "onnx", "onnx", "v", "1.15.0", {"C++", "library", "ai"}},
    {"openexr", "AcademySoftwareFoundation", "openexr", "v", "3.2.2", {"C++", "library"}},
    {"pcg", "imneme", "pcg-cpp", "v", "0.98.1", {"C++", "library"}},
    {"pigz", "madler", "pigz", "v", "2.8", {"C++", "library", "compression"}},
    {"prometheus-cpp", "jupp0r", "prometheus-cpp", "v", "1.2.4", {"C++", "library", "logging"}},
    {"pugixml", "zeux", "pugixml", "v", "1.14", {"C++", "library"}},
    {"rapidjson", "Tencent", "rapidjson", "v", "1.1.0", {"C++", "library"}},
    {"s2geometry", "google", "s2geometry", "v", "0.10.0", {"C++", "library"}},
    {"spdlog", "gabime", "spdlog", "Version ", "1.13.0", {"C++", "library"}},
    {"tinyformat", "c42f", "tinyformat", "Release ", "2.3.0", {"C++", "library"}},
    {"yaml-cpp", "jbeder", "yaml-cpp", "yaml-cpp-", "0.8.0", {"C++", "library"}},
    {"zlib", "madler", "zlib", "zlib ", "1.3.1", {"C++", "library", "compression"}},
    {"zstd", "facebook", "zstd", "Zstandard v", "1.5.5", {"C++", "library", "compression"}},
};

void update_module_description(const std::string& module_name, GitHubApi& api) {
    // find last version from BCR
    auto content = api.content("bazelbuild", "bazel-central-registry", std::string("modules/") + module_name + std::string("/metadata.json"));
    json json_response = json::parse(content);
    auto versions =  json_response["versions"].get<std::vector<std::string>>();
    //std::cout << versions.back() << std::endl;
    const auto find_by_name = [&module_name](const ModuleDescription& p ){ return p.module == module_name; };
    const auto find_it = std::find_if( std::begin(repos), std::end(repos), find_by_name );
    if(find_it == std::end(repos)) {
        std::cout << "Module " << module_name << " not found" << std::endl;
        return;
    }
    find_it->last_known_version = versions.back();
}

int main(int argc, char **argv) {
    CommandLineArguments cla;
    if(cla.parse_command_line_options(argc, argv)) {
        return 0; // return on standard command (e.g. --help, --version)
    };

    bool use_proxy = cla.use_proxy();
    std::string path_to_bcr = cla.bazel_central_registry_directory();
    std::string git_hub_access_token = cla.git_hub_access_token();

    // Sync https://github.com/Vertexwahn/bazel-central-registry main branch
    // with bazelbuild/bazel-central-registry
    GitHubApi api{git_hub_access_token};
    api.merge_upstream("Vertexwahn", "bazel-central-registry", "main");

    if(cla.module() == std::string("update")) {
        // Check if there is a new version of fmt
        std::cout << "Check for updates..." << std::endl;

        std::cout << "Known modules: " << repos.size() << std::endl;

        // Update module description
        update_module_description("catch2", api);
        update_module_description("embree", api);
        update_module_description("glm", api);
        update_module_description("glog", api);
        update_module_description("imath", api);
        update_module_description("libdeflate", api);
        update_module_description("onetbb", api);
        update_module_description("openexr", api);
        update_module_description("tinyformat", api);

        // Update module descriptions should be done this way - but the mechanism has still problems with .bcrX versions
        //for(ModuleDescription desc : repos) {
        //    update_module_description(desc.module, api);
        //}

        for(ModuleDescription desc : repos) {
            std::string version{};

            bool has_release_packages = api.has_release_packages(desc.owner, desc.repo);

            if(has_release_packages) {
                version = api.latest_release(desc.owner, desc.repo);
            }
            else {
                version = api.latest_tag(desc.owner, desc.repo);
            }

            if(!desc.version_strip_prefix.empty()) {
                replace(version, desc.version_strip_prefix, "");
            }

            if(desc.last_known_version != version) {
                std::cout << "New version of " << desc.module << " available (" << version << ")" << std::endl;
            }
        }
    }
    else if(cla.module() == std::string("libpng")) {
        std::string module = cla.module();
        std::cout << module << " update" << std::endl;

        std::string GIT_HUB_USER = "Vertexwahn";

        // Find latest release of module
        std::string version = "1.6.43"; //api.latest_release(github_module_repo_owner, github_module_repo);
        //replace(version, strip_prefix, "");
        std::cout << "Latest release: " << version << std::endl;

        // now make an own branch for module update
        std::string branch = module + "-" + version;
        std::cout << "Create branch: " << branch << std::endl;

        std::string base_commit_sha = api.latest_sha_hash(GIT_HUB_USER, "bazel-central-registry", "main");

        api.create_branch(GIT_HUB_USER, "bazel-central-registry", branch, base_commit_sha);

        // Clone repo and branch
        GitClient git_client{use_proxy};
        git_client.clone(
                fmt::format("https://github.com/{}/bazel-central-registry.git", GIT_HUB_USER),
                path_to_bcr);
        git_switch_branch(path_to_bcr, branch);

        // No edit everything
        std::string path = path_to_bcr + std::string("/modules/") + module + std::string("/") + version;
        create_directory(path);

        create_module_dot_file(path, module, version);
        create_presubmit_yml_file(path, module);
        create_module_source_json_file(path, module, version);

        create_directory(path + "/patches");

        auto patch_files = get_files_in_directory(fmt::format("bcrupdate/modules/{}/patches", module));

        for(std::string& patch_filename : patch_files) {
            create_file_from_template(
                    fmt::format("bcrupdate/modules/{}/patches/{}", module, patch_filename),
                    fmt::format("{}/patches/{}", path, patch_filename),
                    version
            );
        }

        update_integrity(path_to_bcr, module);

        commit_changes(path_to_bcr, std::string("Add ") + module + std::string(" ") + version);
    }
    else {
        const std::string name_to_find = cla.module();
        const auto find_by_name = [&name_to_find]( const ModuleDescription& p ){ return p.module == name_to_find; };

        const auto find_it = std::find_if( std::begin(repos), std::end(repos), find_by_name);

        if(find_it == std::end(repos)) {
            std::cout << "Could not find module " << cla.module() << std::endl;
            return -1;
        }

        std::string module = find_it->module;
        std::string github_module_repo = find_it->repo;
        std::string github_module_repo_owner = find_it->owner;
        std::string strip_prefix = find_it->version_strip_prefix;

        GitClient git_client{use_proxy};

        // Find latest release of module
        std::string version = api.latest_release(github_module_repo_owner, github_module_repo);
        replace(version, strip_prefix, "");
        std::cout << "Latest release: " << version << std::endl;

        update_module_hosted_on_github(api, git_client, module, github_module_repo, github_module_repo_owner,
                                       strip_prefix, path_to_bcr, version);
    }

    std::cout << "Goodbye from bcrupdate!" << std::endl;

    return 0;
}
