# bcrupdate

bcrupdate is tiny fragile tool to perform simple version updates of modules in the [Bazel Central Registry](https://registry.bazel.build/).

bcrupdate assumes that you have forked the [Bazel Central Registry](https://registry.bazel.build/) in your GitHub account and can provide access to it
via a GitHub access token.
When an update for a specific module is performed the forked BCR repository gets first synchronized with the official upstream BCR repository.
Afterwards, a branch in your forked BCR repository is created.
This branch naming here follows the pattern `<module-name>-<version>`.
Then the branch is checked out locally on your file system.
The local repository is then edited.
Templates that can be found in `bcrupdate/modulse/<module-name>` are applied to the branch.
Afterward, the changes are pushed to GitHub.

## How to run?

### Prerequisites

Make sure to setup an GitHub token.
E.g. add `GIT_HUB_ACCESS_TOKEN` to your `.bashrc` or `.zshrc` file:

```shell
echo "export GIT_HUB_ACCESS_TOKEN=\"$github_token\"" >> ~/.zshrc # .zshrc
```

Check if token is available:

```shell
echo ${GIT_HUB_ACCESS_TOKEN}
```

### Check for suggested updates

```shell
bazel run //bcrupdate:bcrupdate.cli -- \
--bazel_central_registry_directory={HOME}/Desktop/test/bcr \
--git_hub_access_token=${GIT_HUB_ACCESS_TOKEN} \
--module=update
```

With proxy:

```shell
bazel run //bcrupdate:bcrupdate.cli -- \
--bazel_central_registry_directory={HOME}/Desktop/test/bcr \
--git_hub_access_token=${GIT_HUB_ACCESS_TOKEN} \
--module=update \
--use_proxy=true
```

### Update a module

```shell
bazel run //bcrupdate:bcrupdate.cli -- \
--bazel_central_registry_directory=${HOME}/Desktop/test/bcr \
--git_hub_access_token=${GIT_HUB_ACCESS_TOKEN} \
--module=onetbb
```

Show version:

```shell
bazel run //bcrupdate:bcrupdate.cli -- --version
```

Show help:

```shell
bazel run //bcrupdate:bcrupdate.cli -- --help
```

## References

- https://conan.io/center
- https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2656r2.html

## Todos

- [ ] update of de_vertexahn would be nice that would check if there is a new version of a module available
- [ ] Add libcurl to Bazel Central Registry
- [ ] Add gitlib2 to Bazel Central Registry
- [ ] Do some statistics
  - [ ] Make a list of all modules that are c++ related (have a public cc_library
  - [ ] Count all modules in a Bazel Registry
- [ ] BCR self-maintenance
  - [ ] Auto update of `.bazelversion` in https://github.com/bazelbuild/bazel-central-registry
- [ ] Auto updates of container structure tests
- [ ] Self-maintenance
  - [ ] Make sure bcrupdate uses always the newest rolling release (https://bazel.build/release/rolling)
  - [ ] Make sure bcrupdate uses the newest version of third-party libs
