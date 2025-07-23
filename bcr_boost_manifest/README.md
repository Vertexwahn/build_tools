# Readme

## Description

This repositiory contains scripts to maintain Boost C++ libraries in the Bazel Central registry.

## How to run?

Via Bazel:

```shell
# Windows
bazel run //bcr_boost_manifest:generate_bcr_modules -- --boost_version=1.88.0 --boost_library_name=crc --bcr_workspace_directory=C:/dev/bazel-central-registry
# Ubuntu
bazel run //bcr_boost_manifest:generate_bcr_modules -- --boost_version=1.88.0 --boost_library_name=uuid --bcr_workspace_directory=/home/vertexwahn/dev/bazel-central-registry
```

Plain python:

```shell
python3 generate_bcr_modules.py --boost_version=1.88.0 --boost_library_name=leaf --bcr_workspace_directory=/home/vertexwahn/dev/bazel-central-registry
```

Generate markdown:

```shell
bazel run //:boost_markdown_generator
```

## Further references

- [A manifest for Boost libraries in the Bazel Central Registry](https://medium.com/p/cc289c9ad12e)
