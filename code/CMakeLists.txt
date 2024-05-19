cmake_minimum_required(VERSION 3.0)

project(code)
link_directories(${PROJECT_SOURCE_DIR}/lib)
message(STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17")
message(STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})

file(GLOB_RECURSE SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_executable(main ${SOURCE_FILES})
# glfw
set(glfw_include "${PROJECT_SOURCE_DIR}/ThirdParty/GLFW/include")
set(glfw_lib "${PROJECT_SOURCE_DIR}/ThirdParty/GLFW/lib")
include_directories(${glfw_include})
link_directories(${glfw_lib})
# glew
set(glew_include "${PROJECT_SOURCE_DIR}/ThirdParty/glew-2.1.0/include")
set(glew_lib "${PROJECT_SOURCE_DIR}/ThirdParty/glew-2.1.0/bin/Release/x64")
message("${glew_lib}")
include_directories(${glew_include})
link_libraries(${glew_lib})
target_link_libraries(main opengl32)
target_link_libraries(main ${glew_lib}/glew32.dll )
target_link_libraries(main glfw3)
# set(glew_lib "")
include_directories(${PROJECT_SOURCE_DIR}/include)

target_link_libraries(main)
# CMakeLists.txt
target_compile_definitions(main PRIVATE EXTERNAL_FILE_PATH="${PROJECT_SOURCE_DIR}/resource/")