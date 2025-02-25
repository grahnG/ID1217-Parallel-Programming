import matplotlib.pyplot as plt

# Data for centralized.c
processes = [2, 4, 6, 8]
times_1_run = [3.8e-05, 0.000161, 9.9e-05, 0.000137]  # 1 run
times_2_runs = [2.9e-05, 4.1e-05, 8.6e-05, 0.000148]  # 2 runs
times_3_runs = [7.9e-05, 0.000132, 0.000154, 0.000128]  # 3 runs

# Create the plot
plt.figure(figsize=(10, 6))  # Set figure size
plt.plot(processes, times_1_run, marker='o', label='1 Round', color='blue')
plt.plot(processes, times_2_runs, marker='s', label='2 Rounds', color='green')
plt.plot(processes, times_3_runs, marker='^', label='3 Rounds', color='red')

# Customize the plot
plt.xlabel('Number of Processes')
plt.ylabel('Execution Time (seconds)')
plt.title('Execution Time of centralize.c vs. Number of Processes')
plt.legend()  # Add legend to distinguish rounds
plt.grid(True, linestyle='--', alpha=0.7)  # Add grid for readability
plt.xticks(processes)  # Ensure x-axis only shows 2, 4, 6, 8

# Display the plot
plt.savefig('centralized_plot.png')