import matplotlib.pyplot as plt
import re

log_data = """
"""

lines = [line for line in log_data.strip().split('\n') if not re.match(r'^[-+ ]+$', line) and not line.startswith("PROCESS ID")]

entries = []
for line in lines:
    parts = re.split(r'\s*\|\s*', line.strip())
    if len(parts) < 3:
        continue
    pid, time, state = parts[0], int(parts[1]), parts[2]
    entries.append((pid, time, state))

state_map = {"Wait": 0, "Begin": 1, "End": 2}
process_dict = {}

for pid, time, state in entries:
    if state not in state_map:
        continue
    if pid not in process_dict:
        process_dict[pid] = []
    process_dict[pid].append((time, state_map[state]))

for pid in process_dict:
    data = process_dict[pid]
    times, states = zip(*data)

    plt.figure(figsize=(10, 4))
    plt.plot(times, states, color='gray', linestyle='-', linewidth=1, alpha=0.5)
    plt.scatter(times, states, c=states, cmap='viridis', marker='o', zorder=3)

    plt.yticks([0, 1, 2], ['Wait', 'Begin', 'End'])
    plt.xlabel('Time (UNIX)')
    plt.ylabel('State')
    plt.title(f'Process ID: {pid}')
    plt.grid(True)
    plt.tight_layout()

    plt.show()