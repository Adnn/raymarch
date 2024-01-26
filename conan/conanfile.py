from conans import ConanFile


class RaymarchConan(ConanFile):
    """ Conan recipe for Raymarch """

    name = "raymarch"
    license = "MIT License"
    url = "https://github.com/Adnn/raymarch"
    description = "Toy raymarcher in GLSL."
    #topics = ("", "", ...)
    settings = ("os", "compiler", "build_type", "arch")
    options = {
        "build_tests": [True, False],
        "shared": [True, False],
        "visibility": ["default", "hidden"],
    }
    default_options = {
        "build_tests": False,
        "shared": False,
        "visibility": "hidden",
    }

    requires = (
        "math/4c3fcbd2f5@adnn/develop",

        "glad/0.1.36",
        "glfw/3.3.8",
    )

    # Note: It seems conventionnal to add CMake build requirement
    # directly to the build profile.
    #build_requires = ()

    build_policy = "missing"
    generators = "CMakeDeps", "CMakeToolchain"


    python_requires="shred_conan_base/0.0.5@adnn/stable"
    python_requires_extend="shred_conan_base.ShredBaseConanFile"
