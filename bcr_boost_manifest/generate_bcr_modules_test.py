import pytest
from build_tools.bcr_boost_manifest.generate_bcr_modules import generate_module_dependency_content

def test_generate_module_dependency_content_basic():
    result = generate_module_dependency_content("foo", "1.2.3")
    assert result == 'bazel_dep(name = "foo", version = "1.2.3")\n'

def test_generate_module_dependency_content_empty_name():
    result = generate_module_dependency_content("", "1.2.3")
    assert result == 'bazel_dep(name = "", version = "1.2.3")\n'

def test_generate_module_dependency_content_empty_version():
    result = generate_module_dependency_content("foo", "")
    assert result == 'bazel_dep(name = "foo", version = "")\n'

def test_generate_module_dependency_content_special_chars():
    result = generate_module_dependency_content("foo-bar_123", "v1.2.3-beta")
    assert result == 'bazel_dep(name = "foo-bar_123", version = "v1.2.3-beta")\n'
