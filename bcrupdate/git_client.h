/*
 *  SPDX-FileCopyrightText: Copyright 2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
//#include <git2.h>
#include "git2.h" // on macOS
#include <string_view>

namespace bcrupdate {
    void checkout_progress(
            const char *path,
            size_t cur,
            size_t tot,
            void *payload);

    class GitClient {
    public:
        GitClient(bool use_proxy) : use_proxy_{use_proxy} {
            // Initialize libgit2
            git_libgit2_init();
        }

        /*
        int clone(std::string_view url, std::string_view local_path, std::string_view branch_name) {

        }
*/
        int clone(std::string_view url, std::string_view local_path) {
            // Clone options
            git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
            clone_opts.checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
            clone_opts.checkout_opts.progress_cb = checkout_progress;

            if(use_proxy_) {
                clone_opts.fetch_opts.proxy_opts.type = GIT_PROXY_SPECIFIED;
                clone_opts.fetch_opts.proxy_opts.url = "http://proxy.muc:8080";
            }

            // Initialize the repository object
            git_repository *repo = nullptr;

            // Clone the repository
            int error = git_clone(&repo, url.data(), local_path.data(), &clone_opts);

            if (error != 0) {
                const git_error *err = giterr_last();
                std::cerr << "Error cloning repository: " << err->message << std::endl;
                giterr_clear();
                git_libgit2_shutdown();
                return 1;
            }

            // Get the repository's root tree
            git_reference *head = nullptr;
            error = git_repository_head(&head, repo);
            if (error != 0) {
                std::cerr << "Error getting repository head: " << giterr_last()->message << std::endl;
                git_repository_free(repo);
                git_libgit2_shutdown();
                return 1;
            }

            git_commit *commit = nullptr;
            error = git_commit_lookup(&commit, repo, git_reference_target(head));
            if (error != 0) {
                std::cerr << "Error looking up commit: " << giterr_last()->message << std::endl;
                git_reference_free(head);
                git_repository_free(repo);
                git_libgit2_shutdown();
                return 1;
            }

            git_tree *tree = nullptr;
            error = git_commit_tree(&tree, commit);
            if (error != 0) {
                std::cerr << "Error getting commit tree: " << giterr_last()->message << std::endl;
                git_commit_free(commit);
                git_reference_free(head);
                git_repository_free(repo);
                git_libgit2_shutdown();
                return 1;
            }

            // List tree entries (files)
            size_t entry_count = git_tree_entrycount(tree);
            std::cout << "List of files in the repository:" << std::endl;
            for (size_t i = 0; i < entry_count; ++i) {
                const git_tree_entry *entry = git_tree_entry_byindex(tree, i);
                const char *filename = git_tree_entry_name(entry);
                std::cout << filename << std::endl;
            }

            // Free resources
            git_tree_free(tree);
            git_commit_free(commit);
            git_reference_free(head);
            git_repository_free(repo);

            return 0;
        }

        ~GitClient() {
            git_libgit2_shutdown();
        }

    private:
        bool use_proxy_ = false;
    };
}
