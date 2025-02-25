import matplotlib.pyplot as plt

# Data for symmetric.c
processes = [2, 4, 6, 8]
times_1_run = [2e-05, 1.6e-05, 5.8e-05, 5.2e-05]    # 1 run
times_2_runs = [2.9e-05, 5.8e-05, 0.0001, 0.000123] # 2 runs
times_3_runs = [1.8e-05, 6.1e-05, 3.9e-05, 3.3e-05] # 3 runs

# Create the plot
plt.figure(figsize=(10, 6))  # Set figure size
plt.plot(processes, times_1_run, marker='o', label='1 Round', color='blue')
plt.plot(processes, times_2_runs, marker='s', label='2 Rounds', color='green')
plt.plot(processes, times_3_runs, marker='^', label='3 Rounds', color='red')

# Customize the plot
plt.xlabel('Number of Processes')
plt.ylabel('Execution Time (seconds)')
plt.title('Execution Time of symmetric.c vs. Number of Processes')
plt.legend()  # Add legend to distinguish rounds
plt.grid(True, linestyle='--', alpha=0.7)  # Add grid for readability
plt.xticks(processes)  # Ensure x-axis only shows 2, 4, 6, 8

# Save the plot to a file
plt.savefig('symmetric_plot.png')
# plt.show()  # Uncomment if you have a display setup