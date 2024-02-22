cmake_minimum_required(VERSION 3.10)

project(DojoDuel VERSION 1.0)

add_executable(DojoDuel keyboard.cpp keyboard.hpp main.cpp mouse.cpp mouse.hpp types.hpp)

set(includeDir "include")
set(libDir "lib")

target_include_directories(DojoDuel PRIVATE ${includeDir})

target_link_directories(DojoDuel PRIVATE ${libDir})

target_compile_definitions(DojoDuel PUBLIC SFML_STATIC WIN32 NDEBUG _CONSOLE)

set_target_properties(DojoDuel PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")

target_link_libraries(DojoDuel PRIVATE 
freeglut.lib
sfml-audio-s.lib
sfml-system-s.lib
vorbis.lib
vorbisenc.lib
vorbisfile.lib
flac.lib
openal32.lib
ogg.lib)