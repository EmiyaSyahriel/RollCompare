# CMakeList.txt : CMake project for RollingComparison, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
# AviSynth+ FilterSDK
include_directories("C:\\Program Files (x86)\\AviSynth+\\FilterSDK\\include")

file(GLOB_RECURSE filter_cpp *.cpp)
file(GLOB_RECURSE filter_h *.h)

# Add source to this project's executable.
add_library (RollCompare SHARED ${filter_cpp} ${filter_h} "avslink.h")

add_custom_command(
	TARGET RollCompare
	POST_BUILD 
	COMMAND dotnet script ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/PostBuild.cs "RollCompare" "dll"
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../
	COMMENT "Moving DLLs into bin folder"
)