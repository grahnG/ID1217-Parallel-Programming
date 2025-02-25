.PHONY: all compile run clean evaluate benchmark

MAKEFLAGS += --no-print-directory

CC = mpicc
EXEC = mpiexec

BUILD_FOLDER := build
SRC_FOLDER   := src
RESULT       := result
N_PROCESSES  := 4  # Default number of MPI processes

# Define the MPI process counts and rounds for performance evaluation
PROCESS_COUNTS := 2 4 6 8
ROUND_COUNTS := 1 2 3

# Define executables
TARGETS := centralized ring symmetric

# Default target
all: compile

# Ensure build and result folders exist
$(BUILD_FOLDER):
	@mkdir -p $(BUILD_FOLDER)

$(RESULT):
	@mkdir -p $(RESULT)

# Compile each program separately, using files inside src/
centralized: $(BUILD_FOLDER)
	@$(CC) -o $(BUILD_FOLDER)/centralized $(SRC_FOLDER)/centralized.c

ring: $(BUILD_FOLDER)
	@$(CC) -o $(BUILD_FOLDER)/ring $(SRC_FOLDER)/ring.c

symmetric: $(BUILD_FOLDER)
	@$(CC) -o $(BUILD_FOLDER)/symmetric $(SRC_FOLDER)/symmetric.c

# Compile all programs
compile: $(TARGETS)

# Run all programs
run: compile
	@echo "Running centralized..."
	@$(EXEC) -n $(N_PROCESSES) $(BUILD_FOLDER)/centralized 5
	@echo "Running ring..."
	@$(EXEC) -n $(N_PROCESSES) $(BUILD_FOLDER)/ring 5
	@echo "Running symmetric..."
	@$(EXEC) -n $(N_PROCESSES) $(BUILD_FOLDER)/symmetric 5

# Benchmark target
benchmark-%: % $(RESULT)
	@rm -f $(RESULT)/$<-benchmark-result.md
	@echo "| Processes | 1 run | 2 runs | 3 runs |" >> $(RESULT)/$<-benchmark-result.md
	@echo "|-----------|-------|-------|-------|" >> $(RESULT)/$<-benchmark-result.md
	@for np in $(PROCESS_COUNTS); do \
		printf "| $$np " >> $(RESULT)/$<-benchmark-result.md; \
		for rounds in $(ROUND_COUNTS); do \
			time_taken=`$(EXEC) -np $$np $(BUILD_FOLDER)/$< $$rounds --silent | tail -n 1`; \
			printf "| $$time_taken " >> $(RESULT)/$<-benchmark-result.md; \
		done; \
		echo "|" >> $(RESULT)/$<-benchmark-result.md; \
	done
	@echo "Benchmark for $< completed. Results saved in $(RESULT)/$<-benchmark-result.md."

# Evaluate all programs
evaluate: compile $(RESULT)
	@for target in $(TARGETS); do \
		$(MAKE) benchmark-$$target; \
	done
	@echo "All benchmarks completed!"

# Run benchmarks for all programs
benchmark: benchmark-centralized benchmark-symmetric benchmark-ring

# Clean build and results
clean:
	rm -rf $(BUILD_FOLDER) $(RESULT) performance_results.csv








