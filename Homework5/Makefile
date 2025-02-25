.PHONY: all compile run clean evaluate plot

MAKEFLAGS += --no-print-directory

BUILD_FOLDER := build
SRC_FOLDER   := src
N_PROCESSES  := 4  # Default number of MPI processes

# Define the MPI process counts and rounds for performance evaluation
PROCESS_COUNTS := 2 4 6 8
ROUND_COUNTS := 1 2 3

# Define executables
TARGETS := centralized ring symmetric

# Default target
all: compile

# Ensure the build folder exists
${BUILD_FOLDER}:
	@mkdir -p ${BUILD_FOLDER}

# Compile each program separately, using files inside src/
centralized: ${BUILD_FOLDER}
	@mpicc -o ${BUILD_FOLDER}/centralized ${SRC_FOLDER}/centralized.c

ring: ${BUILD_FOLDER}
	@mpicc -o ${BUILD_FOLDER}/ring ${SRC_FOLDER}/ring.c

symmetric: ${BUILD_FOLDER}
	@mpicc -o ${BUILD_FOLDER}/symmetric ${SRC_FOLDER}/symmetric.c

# Compile all programs
compile: ${TARGETS}

# Run all programs with trace output (Normal execution)
run: compile
	@echo "Running centralized.c..."
	@mpiexec -n ${N_PROCESSES} ${BUILD_FOLDER}/centralized 5
	@echo "Running ring.c..."
	@mpiexec -n ${N_PROCESSES} ${BUILD_FOLDER}/ring 5
	@echo "\nRunning symmetric.c..."
	@mpiexec -n ${N_PROCESSES} ${BUILD_FOLDER}/symmetric 5

# Run performance evaluation experiments (No trace output)
evaluate: compile
	@echo "Running performance evaluation..."
	@echo "Program, Processes, Rounds, Execution Time (s)" > performance_results.csv
	@for np in ${PROCESS_COUNTS}; do \
		for rounds in ${ROUND_COUNTS}; do \
			for program in ${TARGETS}; do \
				echo "Evaluating $$program with $$np processes and $$rounds rounds..."; \
				time_taken=$$(mpiexec -n $$np ${BUILD_FOLDER}/$$program $$rounds --silent | tail -n 1); \
				echo "$$program, $$np, $$rounds, $$time_taken" >> performance_results.csv; \
			done \
		done \
	done
	@echo "Performance evaluation completed! Results saved in performance_results.csv."

# Clean build folder
clean:
	@rm -rf ${BUILD_FOLDER} performance_results.csv





