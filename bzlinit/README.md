# bzlinit

bzlinit is a simple tool to setup a basic Bazel project.

## Usage

### Add basic Bazel related files

```shell
bazel run //bzlinit:bzlinit -- --workspace_path=${HOME}/dev/Piper/third_party/libdeflate-1.20
```

### Print version

```shell
bazel run //bzlinit:bzlinit -- --version
```

### Show help

```shell
bazel run //bzlinit:bzlinit -- --help
```

### Random ideas for the future

#### Config file

Add `Config.yaml` file

```yaml
version: latest # can be also some specific version e.g. 7.0.0
create_dot_bazelversion: true # Determines if a .bazelversion file is created with the defined version
favourite_lang: cpp # Define language for generated hello world program
logging: verbose # warning, error
```

#### Improve automatic bazelization

Idea:

```shell
cd some_project
bzlinit # This command tries to bazelize the given project
# Example
git clone https://github.com/markuspi/SnakeIO.git
bzlinit
bazel run //:main
```
