#include <iostream>
#include <fstream>
#include <filesystem>
#include <cassert>

#include "command_line_arguments.h"

void create_dot_bazelversion_file(std::string_view workspace_directory_path) {
    // Construct the full path to the .bazelversion file
    std::string file_path = std::string(workspace_directory_path) + "/.bazelversion";

    // Check if the file already exists
    if (std::filesystem::exists(file_path)) {
        // File already exists, no need to create it
        std::cout << ".bazelversion file already exists in the specified workspace directory. Nothing to do." << std::endl;
        return;
    }

    // File doesn't exist, create it
    std::ofstream out_file(file_path);
    if (!out_file.is_open()) {
        std::cerr << "Error creating .bazelversion file in the specified workspace directory." << std::endl;
        return;
    }

    // Write the default Bazel version to the file
    out_file << "7.0.2" << std::endl;  // You can replace this version with your desired Bazel version

    std::cout << ".bazelversion file created successfully in the specified workspace directory." << std::endl;
}

void create_module_dot_bazel_file(std::string_view workspace_directory_path, std::string_view module_name) {
    // Construct the full path to the MODULE.bazel file
    std::string file_path = std::string(workspace_directory_path) + "/MODULE.bazel";

    // Check if the file already exists
    if (std::filesystem::exists(file_path)) {
        // File already exists, no need to create it
        std::cout << "MODULE.bazel file already exists in the specified workspace directory. Nothing to do." << std::endl;
        return;
    }

    // File doesn't exist, create it
    std::ofstream out_file(file_path);
    if (!out_file.is_open()) {
        std::cerr << "Error creating MODULE.bazel file in the specified workspace directory." << std::endl;
        return;
    }

    // Write content to the MODULE.bazel file
    out_file << "module(name = \"" << module_name << "\")" << std::endl;

    std::cout << module_name << "MODULE.bazel file created successfully in the specified workspace directory." << std::endl;
}

void create_build_dot_bazel_file(std::string_view workspace_directory_path) {
    // Construct the full path to the MODULE.bazel file
    std::string file_path = std::string(workspace_directory_path) + "/BUILD.bazel";

    // Check if the file already exists
    if (std::filesystem::exists(file_path)) {
        // File already exists, no need to create it
        std::cout << "BUILD.bazel file already exists in the specified workspace directory. Nothing to do." << std::endl;
        return;
    }

    // File doesn't exist, create it
    std::ofstream out_file(file_path);
    if (!out_file.is_open()) {
        std::cerr << "Error creating BUILD.bazel file in the specified workspace directory." << std::endl;
        return;
    }

    // Write content to the MODULE.bazel file
    out_file << "# A build file" << std::endl;

    std::cout << "BUILD.bazel file created successfully in the specified workspace directory." << std::endl;
}

void write_hello_world_program(const std::string& filename) {
    std::ofstream outfile(filename);

    if (outfile.is_open()) {
        outfile << "#include <iostream>\n\n";
        outfile << "int main() {\n";
        outfile << "    std::cout << \"Hello, World!\" << std::endl;\n";
        outfile << "    return 0;\n";
        outfile << "}\n";
        std::cout << "Hello World program written to " << filename << std::endl;
    } else {
        std::cerr << "Error opening file: " << filename << std::endl;
    }
}

void extend_build_file(const std::string& build_filename, const std::string& target_name, const std::string& source_filename) {
    // Open the BUILD file to append the cc_binary target
    std::ofstream build_file(build_filename, std::ios::app);

    if (build_file.is_open()) {
        build_file << "cc_binary(\n";
        build_file << "    name = \"" << target_name << "\",\n";
        build_file << "    srcs = [\"" << source_filename << "\"],\n";
        build_file << ")\n";
        std::cout << "cc_binary target added to " << build_filename << std::endl;
    } else {
        std::cerr << "Error opening file: " << build_filename << std::endl;
    }
}

void create_or_extend_gitignore(std::string_view workspace_directory_path) {
    std::string gitignore_filename = std::string(workspace_directory_path) + "/.gitignore";

    // Check if the .gitignore file already exists
    if (std::filesystem::exists(gitignore_filename)) {
        std::ifstream existing_file(gitignore_filename);
        std::string file_content;

        if (existing_file.is_open()) {
            // Read existing content
            file_content.assign((std::istreambuf_iterator<char>(existing_file)), std::istreambuf_iterator<char>());
            existing_file.close();
        } else {
            std::cerr << "Error opening existing .gitignore file." << std::endl;
            return;
        }

        // Append the pattern to the existing file content
        std::ofstream out_file(gitignore_filename, std::ios::app);
        if (out_file.is_open()) {
            if (file_content.find("# Ignore Bazel files and directories") == std::string::npos) {
                out_file << "\n# Ignore Bazel files and directories\n";
            }
            
            // Check if the pattern already exists in the file
            if (file_content.find("bazel*") == std::string::npos) {
                 out_file << "bazel*\n";
            }

            if (file_content.find("MODULE.bazel.lock") == std::string::npos) {
                 out_file << "MODULE.bazel.lock\n";
            }

            std::cout << "Pattern 'bazel*' added to existing .gitignore file." << std::endl;
        } else {
            std::cerr << "Error opening .gitignore file for appending." << std::endl;
        }
    } else {
        // Create a new .gitignore file with the pattern
        std::ofstream out_file(gitignore_filename);
        if (out_file.is_open()) {
            out_file << "# Ignore Bazel files and directories\n";
            out_file << "bazel*\n";
            out_file << "MODULE.bazel.lock" << std::endl;
            std::cout << ".gitignore file created with the pattern 'bazel*'." << std::endl;
        } else {
            std::cerr << "Error creating .gitignore file." << std::endl;
        }
    }
}

// todo: std::string_view workspace_directory_path -> use std::filesystem::path?
void create_empty_bazelrc_file(std::string_view workspace_directory_path) {
    std::string bazelrc_filename = std::string(workspace_directory_path) + "/.bazelrc";

    // Check if the .bazelrc file already exists
    if (std::filesystem::exists(bazelrc_filename)) {
        std::cout << ".bazelrc file already exists in the specified workspace directory. Nothing to do." << std::endl;
        return;
    }

    // Create a new empty .bazelrc file
    std::ofstream out_file(bazelrc_filename);
    if (out_file.is_open()) {
        std::cout << ".bazelrc file created successfully in the specified workspace directory." << std::endl;
    } else {
        std::cerr << "Error creating .bazelrc file in the specified workspace directory." << std::endl;
    }
}

void create_empty_workspace_bazel_file(std::string_view workspace_directory_path) {
    std::filesystem::path dir_path(workspace_directory_path);
    std::filesystem::path file_path = dir_path / "WORKSPACE.bazel";

    // Create an empty file
    std::ofstream ofs(file_path);
    if (!ofs) {
        throw std::runtime_error("Failed to create the file: " + file_path.string());
    }
    // File is automatically closed when ofs goes out of scope
}

bool directory_exists(const std::filesystem::path& dir_path) {
   return std::filesystem::exists(dir_path) && std::filesystem::is_directory(dir_path);
}

int main(int argc, char **argv) {
    CommandLineArguments cla;
    cla.parse_command_line_options(argc, argv);

    if(cla.handle_standard_commands()) {
        return 0;
    }

    if(cla.workspace_path().empty()) {
        std::cout << "Please provide a workspace directory!" << std::endl;
        return 1;
    }

    if(!directory_exists(cla.workspace_path())) {
        std::cout << "Provided workspace path (" << cla.workspace_path() << ")" << " does not exist" << std::endl;
        return 2;
    }

    std::string workspace_dir = cla.workspace_path();

    bool empty_workspace = std::filesystem::is_empty(workspace_dir);

    create_dot_bazelversion_file(workspace_dir);

    std::string_view workspace_directory_path = workspace_dir;  // Replace with the desired workspace directory path
    std::string_view module_name = "my_module";  // Replace with the desired module name
    create_module_dot_bazel_file(workspace_directory_path, module_name);

    create_build_dot_bazel_file(workspace_directory_path);

    if(empty_workspace) {
        write_hello_world_program(workspace_dir + "/main.cpp");
        extend_build_file(workspace_dir + "/BUILD.bazel", "hello_world", "main.cpp");
    }

    create_or_extend_gitignore(workspace_dir);
    create_empty_bazelrc_file(workspace_dir);
    create_empty_workspace_bazel_file(workspace_dir);

    return 0;
}
