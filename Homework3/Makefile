.PHONY: all compile run clean

MAKEFLAGS += --no-print-directory

BUILD_FOLDER := build
TARGET       := app
ARGUMENTS    :=

all: compile

compile: ${TARGET}

run: ${TARGET}
	@./${BUILD_FOLDER}/${TARGET} ${ARGUMENTS}

clean:
	@rm -rf ${BUILD_FOLDER}

${BUILD_FOLDER}:
	@cmake -B ${BUILD_FOLDER} -DTARGET=${TARGET}

${TARGET}: ${BUILD_FOLDER}
	@cmake --build ${BUILD_FOLDER}
