cmake_minimum_required(VERSION 3.22.1)
project("MyProject" VERSION 1.0.0)

if("${TARGET}" STREQUAL "")
	set(TARGET "app")
endif()

set(SOURCE_FOLDER "src")
set(SOURCE_FILE_EXTENSION "c")

set(BUILD_DATE "Unknown")

if(UNIX)
	execute_process(
		COMMAND date "+%Y %b %d %H:%M:%S"
		OUTPUT_VARIABLE BUILD_DATE
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
endif()

add_definitions(
	-DPROJECT_BUILD_DATE="${BUILD_DATE}"
	-DPROJECT_NAME="${PROJECT_NAME}"
	-DPROJECT_VERSION="${PROJECT_VERSION}"
	-DPROJECT_SYSTEM="${CMAKE_SYSTEM_NAME}"
)

include_directories(include)

# Explicitly list source files instead of GLOB to avoid including multiple main functions
set(SOURCES
	src/main.c
	src/MatrixSumA.c
	src/MatrixSumB.c
	src/MatrixSumC.c
)

add_executable(${TARGET} ${SOURCES})

target_link_libraries(${TARGET}
)
