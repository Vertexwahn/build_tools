import os
import argparse
from boost_downloader import download_and_extract_boost_library
from boost_dependencies import BOOST_1_88_0_DEPS
from boost_dependencies import generate_boost_levels_markdown


def generate_module_dependency_content(name, version):
    return f"""bazel_dep(name = "{name}", version = "{version}")\n"""


def generate_boost_module_header_content(
    boost_library_name, boost_library_version, boost_compatibility_level
):
    return f"""module(
    name = "boost.{boost_library_name}",
    version = "{boost_library_version}",
    bazel_compatibility = [">=7.6.0"],
    compatibility_level = {boost_compatibility_level},
)\n"""


def generate_boost_library_source_json_content(
    boost_library_name,
    boost_version,
    integrity_hash,
    build_bazel_hash,
    module_bazel_hash,
    patch_strip=0,
):
    return f"""{{
    "integrity": "{integrity_hash}",
    "strip_prefix": "{boost_library_name}-boost-{boost_version}",
    "url": "https://github.com/boostorg/{boost_library_name}/archive/refs/tags/boost-{boost_version}.tar.gz",
    "patch_strip": {patch_strip},
    "overlay": {{
        "BUILD.bazel": "{build_bazel_hash}",
        "MODULE.bazel": "{module_bazel_hash}"
    }}
}}"""


def generate_boost_build_bazel_content(boost_library_name, dependencies):
    deps_str = ""
    if dependencies:
        deps_lines = [f'        "@{dep}",' for dep in dependencies]
        deps_str = f"""    deps = [
{chr(10).join(deps_lines)}
    ],"""

    return f"""load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "boost.{boost_library_name}",
    hdrs = glob([
        "include/**/*.hpp",
    ]),
    features = ["parse_headers"],
    includes = ["include"],
    visibility = ["//visibility:public"],
{deps_str}
)\n"""


def generate_boost_presubmit_yml_content(boost_library_name):
    return f"""matrix:
  platform:
    - debian10
    - debian11
    - macos
    - macos_arm64
    - ubuntu2004
    - ubuntu2204
    - ubuntu2404
    - windows
  bazel: [7.x, 8.x, rolling]
tasks:
  verify_targets:
    name: Verify build targets
    platform: ${{{{ platform }}}}
    bazel: ${{{{ bazel }}}}
    build_flags:
      - '--process_headers_in_dependencies'
    build_targets:
      - '@boost.{boost_library_name}//:boost.{boost_library_name}'
"""


def save_content_as_file(content, filename, mode="w"):
    dir_name = os.path.dirname(filename)
    if dir_name and not os.path.exists(dir_name):
        os.makedirs(dir_name, exist_ok=True)
    with open(filename, mode) as f:
        f.write(content)


def create_overlay_module_symlink(
    bcr_workspace_directory, boost_library_name, boost_library_version
):
    overlay_dir = f"{bcr_workspace_directory}/modules/boost.{boost_library_name}/{boost_library_version}/overlay"
    src = "../MODULE.bazel"
    dst = os.path.join(overlay_dir, "MODULE.bazel")
    # Only create symlink if it doesn't already exist
    if not os.path.exists(dst):
        os.symlink(src, dst)
        print(f"Created symlink: {dst} -> {src}")
    else:
        print(f"Symlink already exists: {dst}")


def copy_existing_boost_files(
    boost_library_name,
    boost_library_copy_existing_files,
    bcr_workspace_directory,
    bcr_boost_library_version,
):
    import shutil

    if boost_library_name in boost_library_copy_existing_files:
        files_to_copy = boost_library_copy_existing_files[boost_library_name]
        for dst_path, src_paths in files_to_copy.items():
            for src_path in src_paths:
                full_src_path = os.path.join(bcr_workspace_directory, src_path)
                full_dst_path = os.path.join(
                    bcr_workspace_directory,
                    f"modules/boost.{boost_library_name}/{bcr_boost_library_version}",
                    dst_path,
                )
                if os.path.exists(full_src_path):
                    dst_dir = os.path.dirname(full_dst_path)
                    if not os.path.exists(dst_dir):
                        os.makedirs(dst_dir, exist_ok=True)
                    shutil.copy2(full_src_path, full_dst_path)
                    print(f"Copied {full_src_path} to {full_dst_path}")
                else:
                    print(f"Warning: Source file {full_src_path} does not exist")


def main():
    # TODO: Make command line paramters out of this
    parser = argparse.ArgumentParser(
        description="Generate BCR modules for Boost libraries."
    )
    parser.add_argument(
        "--boost_version", required=True, help="Boost library version, e.g., 1.88.0"
    )
    parser.add_argument(
        "--boost_library_name", required=True, help="Boost library name, e.g., crc"
    )
    parser.add_argument(
        "--bcr_workspace_directory",
        required=True,
        help="Path to Bazel Central Registry workspace directory",
    )
    args = parser.parse_args()

    BOOST_LIBRARY_VERSION = args.boost_version
    print(f"Using Boost library version: {BOOST_LIBRARY_VERSION}")
    BOOST_LIBRARY_NAME = args.boost_library_name
    print(f"Using Boost library name: {BOOST_LIBRARY_NAME}")
    BCR_WORKSPACE_DIRECTORY = args.bcr_workspace_directory

    RULES_CC_VERSION = "0.1.3"

    # Derive BOOST_COMPATIBILITY_LEVEL from BOOST_LIBRARY_VERSION, e.g., 1.88.0 -> 108800
    version_parts = BOOST_LIBRARY_VERSION.split(".")
    BOOST_COMPATIBILITY_LEVEL = (
        f"1{int(version_parts[0]):01d}{int(version_parts[1]):02d}00"
    )

    BCR_BOOST_LIBRARY_VERSION = BOOST_LIBRARY_VERSION + ".bcr.1"

    boost_deps = {
        **BOOST_1_88_0_DEPS["level 0"],
        **BOOST_1_88_0_DEPS["level 1"],
        **BOOST_1_88_0_DEPS["level 2"],
        **BOOST_1_88_0_DEPS["level 3"],
        **BOOST_1_88_0_DEPS["level 4"],
        **BOOST_1_88_0_DEPS["level 5"],
        **BOOST_1_88_0_DEPS["level 6"],
        **BOOST_1_88_0_DEPS["level 7"],
        **BOOST_1_88_0_DEPS["level 8"],
        **BOOST_1_88_0_DEPS["level 9"],
        **BOOST_1_88_0_DEPS["level 10"],
        **BOOST_1_88_0_DEPS["level 11"],
        **BOOST_1_88_0_DEPS["level 12"],
        **BOOST_1_88_0_DEPS["level 13"],
        **BOOST_1_88_0_DEPS["level 14"],
        **BOOST_1_88_0_DEPS["level 15"],
        **BOOST_1_88_0_DEPS["level 16"],
        **BOOST_1_88_0_DEPS["level 17"],
        **BOOST_1_88_0_DEPS["level 18"],
        **BOOST_1_88_0_DEPS["level 19"],
        **BOOST_1_88_0_DEPS["level 20"],
        **BOOST_1_88_0_DEPS["level 21"],
    }

    # TODO: Generate this list automatically based on the latest shit
    boost_library_copy_existing_files = {
        "core": {
            "overlay/BUILD.bazel": ["modules/boost.core/1.87.0/overlay/BUILD.bazel"],
        },
        "uuid": {
            "overlay/BUILD.bazel": ["modules/boost.uuid/1.87.0/overlay/BUILD.bazel"],
        },
        "system": {
            "overlay/BUILD.bazel": ["modules/boost.system/1.87.0/overlay/BUILD.bazel"],
        },
        "timer": {
            "overlay/BUILD.bazel": ["modules/boost.timer/1.87.0/overlay/BUILD.bazel"],
        },
        "align": {
            "overlay/BUILD.bazel": ["modules/boost.align/1.87.0/overlay/BUILD.bazel"],
        },
        "bind": {
            "overlay/BUILD.bazel": ["modules/boost.bind/1.87.0/overlay/BUILD.bazel"],
        },
        "charconv": {
            # "overlay/BUILD.bazel": ["modules/boost.bind/1.87.0/overlay/BUILD.bazel"],
        },
        "circular_buffer": {
            "overlay/BUILD.bazel": [
                "modules/boost.circular_buffer/1.87.0/overlay/BUILD.bazel"
            ],
        },
        "detail": {
            "overlay/BUILD.bazel": ["modules/boost.detail/1.87.0/overlay/BUILD.bazel"],
            "overlay/test/BUILD.bazel": [
                "modules/boost.detail/1.87.0/overlay/test/BUILD.bazel"
            ],
            "overlay/test/MODULE.bazel": [
                "modules/boost.detail/1.87.0/overlay/test/MODULE.bazel"
            ],
        },
        "tuple": {
            "overlay/BUILD.bazel": ["modules/boost.tuple/1.87.0/overlay/BUILD.bazel"],
        },
        "unordered": {
            "overlay/BUILD.bazel": [
                "modules/boost.unordered/1.87.0/overlay/BUILD.bazel"
            ],
        },
        "lexical_cast": {
            "overlay/BUILD.bazel": [
                "modules/boost.lexical_cast/1.87.0/overlay/BUILD.bazel"
            ],
        },
        "utility": {
            "overlay/BUILD.bazel": ["modules/boost.utility/1.87.0/overlay/BUILD.bazel"],
        },
        "smart_ptr": {
            "overlay/BUILD.bazel": [
                "modules/boost.smart_ptr/1.87.0/overlay/BUILD.bazel"
            ],
        },
        "stacktrace": {
            "overlay/BUILD.bazel": [
                "modules/boost.stacktrace/1.87.0/overlay/BUILD.bazel"
            ],
        },
        "optional": {
            "overlay/BUILD.bazel": [
                "modules/boost.optional/1.87.0/overlay/BUILD.bazel"
            ],
        },
        "ratio": {
            "overlay/BUILD.bazel": ["modules/boost.ratio/1.87.0/overlay/BUILD.bazel"],
        },
    }

    boost_dev_deps = {
        "endian": [],
        "winapi": [],
        "timer": [],
    }

    BOOST_LIBRARY_DIRECT_DEPS = boost_deps.get(BOOST_LIBRARY_NAME)
 
    header = generate_boost_module_header_content(
        BOOST_LIBRARY_NAME, BCR_BOOST_LIBRARY_VERSION, BOOST_COMPATIBILITY_LEVEL
    )

    deps = ""
    for dep in BOOST_LIBRARY_DIRECT_DEPS:
        deps += generate_module_dependency_content(dep, BCR_BOOST_LIBRARY_VERSION)
    deps += generate_module_dependency_content(
        "boost.pin_version", BCR_BOOST_LIBRARY_VERSION
    )
    deps += generate_module_dependency_content("rules_cc", RULES_CC_VERSION)

    module_content = header + "\n" + deps
    print(module_content)
    save_content_as_file(
        module_content,
        f"{BCR_WORKSPACE_DIRECTORY}/modules/boost.{BOOST_LIBRARY_NAME}/{BCR_BOOST_LIBRARY_VERSION}/MODULE.bazel",
    )

    # TODO: Generte correct sha256 hashes for the source.json
    source_json_content = generate_boost_library_source_json_content(
        BOOST_LIBRARY_NAME,
        BOOST_LIBRARY_VERSION,
        "sha256-TLoGSRQk/a7XII9H9SSHJbL0to9/osiUQRzFrJe869E=",
        "sha256-TLoGSRQk/a7XII9H9SSHJbL0to9/osiUQRzFrJe869E=",
        "sha256-TLoGSRQk/a7XII9H9SSHJbL0to9/osiUQRzFrJe869E=",
    )
    print(source_json_content)
    save_content_as_file(
        source_json_content,
        f"{BCR_WORKSPACE_DIRECTORY}/modules/boost.{BOOST_LIBRARY_NAME}/{BCR_BOOST_LIBRARY_VERSION}/source.json",
    )

    overlay_build_file_content = generate_boost_build_bazel_content(
        BOOST_LIBRARY_NAME, BOOST_LIBRARY_DIRECT_DEPS
    )
    print(overlay_build_file_content)
    save_content_as_file(
        overlay_build_file_content,
        f"{BCR_WORKSPACE_DIRECTORY}/modules/boost.{BOOST_LIBRARY_NAME}/{BCR_BOOST_LIBRARY_VERSION}/overlay/BUILD.bazel",
    )

    presubmit_content = generate_boost_presubmit_yml_content(BOOST_LIBRARY_NAME)
    print(presubmit_content)
    save_content_as_file(
        presubmit_content,
        f"{BCR_WORKSPACE_DIRECTORY}/modules/boost.{BOOST_LIBRARY_NAME}/{BCR_BOOST_LIBRARY_VERSION}/presubmit.yml",
    )

    print(
        f"buildifier modules/boost.{BOOST_LIBRARY_NAME}/{BCR_BOOST_LIBRARY_VERSION}/overlay/BUILD.bazel"
    )
    print(
        f"buildifier modules/boost.{BOOST_LIBRARY_NAME}/{BCR_BOOST_LIBRARY_VERSION}/overlay/MODULE.bazel"
    )
    print(f"bazel run //tools:update_integrity -- boost.{BOOST_LIBRARY_NAME}")
    print(f"git checkout -b boost.{BOOST_LIBRARY_NAME}@{BCR_BOOST_LIBRARY_VERSION}")
    print(f"git add .")
    print(f'git commit -m "boost.{BOOST_LIBRARY_NAME}@{BCR_BOOST_LIBRARY_VERSION}"')
    print(
        f"git push --set-upstream origin boost.{BOOST_LIBRARY_NAME}@{BCR_BOOST_LIBRARY_VERSION}"
    )

    copy_existing_boost_files(
        BOOST_LIBRARY_NAME,
        boost_library_copy_existing_files,
        BCR_WORKSPACE_DIRECTORY,
        BCR_BOOST_LIBRARY_VERSION,
    )

    create_overlay_module_symlink(
        BCR_WORKSPACE_DIRECTORY, BOOST_LIBRARY_NAME, BCR_BOOST_LIBRARY_VERSION
    )

    # Generate boost levels markdown and print/save it
    #levels_markdown = generate_boost_levels_markdown(BOOST_1_88_0_DEPS)
    #print(levels_markdown)
    # save_content_as_file(
    #    levels_markdown,
    #    f"{BCR_WORKSPACE_DIRECTORY}/boost_levels.md",
    # )


"""
    extracted_dir = download_and_extract_boost_library(BOOST_LIBRARY_NAME, BOOST_LIBRARY_VERSION, 
                                                       extract_to=f"{BOOST_LIBRARY_NAME}")
    
    # Add MODULE.bazel file to the extracted archive
    module_bazel_path = os.path.join(extracted_dir, "MODULE.bazel")
    save_content_as_file(module_content, module_bazel_path)
    print(f"Created MODULE.bazel in extracted archive: {module_bazel_path}")
    
    # Add BUILD.bazel file to the extracted archive
    build_bazel_path = os.path.join(extracted_dir, "BUILD.bazel")
    save_content_as_file(overlay_build_file_content, build_bazel_path)
    print(f"Created BUILD.bazel in extracted archive: {build_bazel_path}")
"""

if __name__ == "__main__":
    main()
