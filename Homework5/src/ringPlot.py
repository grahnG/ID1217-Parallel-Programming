import matplotlib.pyplot as plt

# Data for ring.c
processes = [2, 4, 6, 8]
times_1_run = [2.5e-05, 0.0001, 7.3e-05, 0.000213]    # 1 run
times_2_runs = [3.1e-05, 0.000121, 0.000203, 0.000132]  # 2 runs
times_3_runs = [0.000116, 8.1e-05, 0.0001, 0.000172]   # 3 runs

# Create the plot
plt.figure(figsize=(10, 6))  # Set figure size
plt.plot(processes, times_1_run, marker='o', label='1 Round', color='blue')
plt.plot(processes, times_2_runs, marker='s', label='2 Rounds', color='green')
plt.plot(processes, times_3_runs, marker='^', label='3 Rounds', color='red')

# Customize the plot
plt.xlabel('Number of Processes')
plt.ylabel('Execution Time (seconds)')
plt.title('Execution Time of ring.c vs. Number of Processes')
plt.legend()  # Add legend to distinguish rounds
plt.grid(True, linestyle='--', alpha=0.7)  # Add grid for readability
plt.xticks(processes)  # Ensure x-axis only shows 2, 4, 6, 8

# Save the plot to a file
plt.savefig('ring_plot.png')
# plt.show()  # Uncomment if you have a display setup