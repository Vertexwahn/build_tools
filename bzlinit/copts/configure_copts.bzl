BZLINIT_MSVC_FLAGS = [
    "/std:c++20",
]

BZLINIT_GCC_FLAGS = [
    "-std=c++23",
]

BZLINIT_CLANG_FLAGS = [
    "-std=c++20",
]

BZLINIT_DEFAULT_COPTS = select({
    "//bzlinit:msvc_compiler": BZLINIT_MSVC_FLAGS,
    "//bzlinit:gcc_compiler": BZLINIT_GCC_FLAGS,
    "//bzlinit:clang_compiler": BZLINIT_CLANG_FLAGS,
    "//conditions:default": BZLINIT_GCC_FLAGS,
})
