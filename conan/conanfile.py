from conan import ConanFile
from conan.tools.build import can_run, check_min_cppstd
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.files import copy, update_conandata
from conan.tools.scm import Git

import os


class RaymarchConan(ConanFile):
    """ Conan recipe for Raymarch """
    name = "raymarch"
    license = "MIT"
    author = "adnn"
    url = "https://github.com/Adnn/raymarch"
    description = "Toy raymarcher in GLSL."
    topics = ("graphics", "ray-marching", "opengl")

    settings = ("os", "compiler", "build_type", "arch")
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
    }

    generators = "CMakeDeps", "CMakeToolchain"
    revision_mode = "scm"

    requires = (
        "math/ee8b6fb1ed@adnn",

        "glad/0.1.36",
        "glfw/3.4",
    )


    # There exist automatic alternatives.
    # see: https://docs.conan.io/2.0/reference/conanfile/methods/config_options.html?highlight=auto_shared_fpic
    def config_options(self):
        if self.settings.get_safe("os") == "Windows":
            self.options.rm_safe("fPIC")


    # see: https://github.com/conan-io/conan/issues/7530#issuecomment-1420634751
    def configure(self):
        if self.options.get_safe("shared"):
            self.options.rm_safe("fPIC")


    def validate(self):
        if self.settings.compiler.get_safe("cppstd"):
            check_min_cppstd(self, "20")


    # Note: We expect the profile to require it in a specific version
    # but having it here makes it simpler for external users.
    def build_requirements(self):
        self.tool_requires("cmake/[>=3.23]")


    def layout(self):
        # The root of the project is one level above
        self.folders.root = ".."
        cmake_layout(self)


    def export(self):
        git = Git(self, self.recipe_folder)
        # Save the url and commit in conandata.yml
        # Unsafe atm since it is missing the repository argument,
        # so we save the coordinates manually
        #git.coordinates_to_conandata()
        url, commit = git.get_url_and_commit(repository=True)
        update_conandata(self, {"scm": {"url": url, "commit": commit}})


    def source(self):
        # recover the url and commit from conandata.yml, use them to get sources
        git = Git(self)
        git.checkout_from_conandata_coordinates()
        git.run("submodule update --init")


    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        if can_run(self):
            cmake.test()


    def package(self):
        cmake = CMake(self)
        cmake.install()
        copy(self, "LICENSE", src=self.source_folder, dst=os.path.join(self.package_folder, "licenses"))


    def package_info(self):
        pass
