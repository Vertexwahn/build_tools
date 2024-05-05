/*
 *  SPDX-FileCopyrightText: Copyright 2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "filesystem.h"

#include "gmock/gmock.h"

#include <filesystem>
#include <fstream>

using namespace bcrupdate;

TEST(FileHandlingTest, GetFilesInDirectory) {
    // Create a temporary directory with some files for testing
    std::filesystem::path temp_dir = std::filesystem::temp_directory_path() / "test_directory";
    std::filesystem::create_directory(temp_dir);
    std::ofstream(temp_dir / "file1.txt").close();
    std::ofstream(temp_dir / "file2.txt").close();
    std::ofstream(temp_dir / "file3.txt").close();

    // Call the function to get files in the temporary directory
    std::vector<std::string> files = get_files_in_directory(temp_dir.string());

    // Check if the function returns the correct number of files
    ASSERT_EQ(files.size(), 3);

    // Check if the files retrieved are correct
    EXPECT_EQ(files[0], "file1.txt");
    EXPECT_EQ(files[1], "file2.txt");
    EXPECT_EQ(files[2], "file3.txt");

    // Clean up: remove the temporary directory
    std::filesystem::remove_all(temp_dir);
}
