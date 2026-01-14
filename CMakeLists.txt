cmake_minimum_required(VERSION 3.5)
project(Mental)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# FetchContent_Declare(
# 	lexy
# 	URL https://lexy.foonathan.net/download/lexy-src.zip
# )

# FetchContent_MakeAvailable(lexy)

FetchContent_Declare(
	rapidcsv
	GIT_REPOSITORY "https://github.com/d99kris/rapidcsv.git"
	GIT_TAG        "v8.90"
)

FetchContent_MakeAvailable(rapidcsv)

option(USE_CLEANED_DICT "Enable to use cleaned dictionary file (cleaned-csvdict.csv)" OFF)

add_executable(mental)
target_sources(mental PRIVATE markov.cpp main.cpp)
target_compile_options(mental PRIVATE -O3)
target_compile_definitions(mental PRIVATE SHOULD_USE_CLEANED_DICT)
# target_link_libraries(mental PRIVATE foonathan::lexy rapidcsv)
target_link_libraries(mental PRIVATE rapidcsv)

add_executable(mental-uncleaned)
target_sources(mental-uncleaned PRIVATE markov.cpp main.cpp)
target_compile_options(mental-uncleaned PRIVATE -O3)
# target_link_libraries(mental PRIVATE foonathan::lexy rapidcsv)
target_link_libraries(mental-uncleaned PRIVATE rapidcsv)


# add_executable(cleancsv)
# target_sources(cleancsv PRIVATE markov.cpp cleancsv.cpp)
# target_compile_options(cleancsv PRIVATE -O3)
# target_link_libraries(cleancsv PRIVATE rapidcsv)