from conans import ConanFile, CMake, tools


class Phonon4Qt5Conan(ConanFile):
    name = "phonon4qt5"
    version = "4.10.3"
    license = "GPLv2"
    url = "https://api.kde.org/phonon/html/index.html"
    description = "Phonon provides a Qt-style interface for audio and video"
    settings = "os", "compiler", "build_type", "arch"

    requires = (
        #"pulse audio"!!!
        "extra-cmake-modules/[>=5.60.0]@kde/testing", # CMakeLists.txt requires 5.49.0
        "qt/[>=5.11.0]@bincrafters/stable"
    )

    generators = "cmake"

    scm = {
        "type": "git",
        "url": "auto",
        "revision": "auto"
    }

    def build(self):
        cmake = CMake(self)
        cmake.definitions["PHONON_BUILD_PHONON4QT5"] = "on"
        cmake.configure()
        cmake.build()
        cmake.install()

    def package_info(self):
        self.cpp_info.resdirs = ["share"]
