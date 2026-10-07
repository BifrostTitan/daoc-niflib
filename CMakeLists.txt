cmake_minimum_required(VERSION 3.20)

project(niflib VERSION 0.1.0 LANGUAGES CXX)

add_library(niflib STATIC
    src/nif_binary_reader.cpp
    src/nif_header.cpp
    src/nif_records.cpp
    src/ni_geometry.cpp
    src/ni_lights.cpp
    src/ni_particles.cpp
    src/ni_particle_controller.cpp
    src/ni_string.cpp
    src/ni_textures.cpp
    src/ni_skinning.cpp
    src/ni_morph.cpp
    src/ni_collision.cpp
    src/ni_file.cpp
    src/ni_extra_data.cpp
    src/ni_animation.cpp
    src/ni_properties.cpp
    src/nif_object_factory.cpp
    src/ni_scene.cpp
    src/ni_scene_variants.cpp
)

add_library(Niflib::niflib ALIAS niflib)

target_compile_features(niflib PUBLIC cxx_std_17)
target_include_directories(niflib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if(MSVC)
    target_compile_options(niflib PRIVATE /W4 /permissive-)
else()
    target_compile_options(niflib PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(GNUInstallDirs)
install(TARGETS niflib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

option(NIFLIB_BUILD_TESTS "Build Niflib++ tests" OFF)
if(NIFLIB_BUILD_TESTS)
    enable_testing()
    add_executable(niflib_tests tests/nif_records_tests.cpp)
    target_link_libraries(niflib_tests PRIVATE Niflib::niflib)
    add_test(NAME niflib_tests COMMAND niflib_tests)
endif()