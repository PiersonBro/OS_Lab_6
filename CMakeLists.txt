cmake_minimum_required(VERSION 3.0)

project(os-sim)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 11)
include_directories(AFTER PRIVATE headers)
file(GLOB lab-6
	"source/*.cpp"
)

add_executable(main ${lab-6})
