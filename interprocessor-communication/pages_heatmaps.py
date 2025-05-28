import re
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors

# Пример входных данных (заполнить своими логами)
readerData = """
"""

writerData = """
"""

def parse_entries(log_data):
    lines = [line for line in log_data.strip().split('\n')
             if not re.match(r'^[-+ ]+$', line) and not line.startswith("PROCESS ID")]
    entries = []
    for line in lines:
        parts = re.split(r'\s*\|\s*', line.strip())
        if len(parts) < 4:
            continue
        pid, time_str, state, page_str = parts
        try:
            time = int(time_str)
        except ValueError:
            continue
        page = int(page_str) if page_str.isdigit() else -1
        entries.append((pid.strip(), time, state.strip(), page))
    return entries

def fill_heatmap(heatmap, entries, op_type, start_time, num_slices, slice_ms, num_pages):
    for pageNumber in range(num_pages):
        i = 0
        while i < len(entries):
            pid, time, state, page = entries[i]
            if state == 'Begin' and page == pageNumber:
                for j in range(i + 1, len(entries)):
                    pid_j, time_j, state_j, page_j = entries[j]
                    if state_j == 'End' and page_j == pageNumber:
                        for t in range(time, time_j + 1, slice_ms):
                            idx = (t - start_time) // slice_ms
                            if 0 <= idx < num_slices:
                                current = heatmap[idx, pageNumber]
                                if current == 0:
                                    heatmap[idx, pageNumber] = op_type
                                elif current != op_type:
                                    heatmap[idx, pageNumber] = 2 if op_type == 1 else 1
                        i = j
                        break
            i += 1

NUM_PAGES = 16
SLICE_MS = 100

reader_entries = parse_entries(readerData)
writer_entries = parse_entries(writerData)

all_times = [e[1] for e in reader_entries + writer_entries if e[3] >= 0]
start_time = min(all_times)
end_time = max(all_times)
num_slices = (end_time - start_time) // SLICE_MS + 1

heatmap = np.zeros((num_slices, NUM_PAGES), dtype=int)

fill_heatmap(heatmap, reader_entries, op_type=1, start_time=start_time, num_slices=num_slices, slice_ms=SLICE_MS, num_pages=NUM_PAGES)
fill_heatmap(heatmap, writer_entries, op_type=2, start_time=start_time, num_slices=num_slices, slice_ms=SLICE_MS, num_pages=NUM_PAGES)

heatmap_rotated = heatmap.T

cmap = mcolors.ListedColormap(['white', 'blue', 'orange', 'cyan'])
bounds = [0, 1, 2, 3, 4]
norm = mcolors.BoundaryNorm(bounds, cmap.N)

plt.figure(figsize=(12, 6))
im = plt.imshow(heatmap_rotated, aspect='auto', cmap=cmap, norm=norm, origin='lower')

cbar = plt.colorbar(im, ticks=[0, 1, 2, 3])
cbar.ax.set_yticklabels(['None', 'Read', 'Write', 'Both'])
cbar.set_label('Access Type')

plt.xlabel('Time intervals, 100ms')
plt.ylabel('Memory Page Number')
plt.title('Memory Access Heatmap')

num_x_ticks = 10
x_ticks = np.linspace(0, heatmap_rotated.shape[1] - 1, num_x_ticks, dtype=int)
plt.xticks(x_ticks, (x_ticks * SLICE_MS + start_time).astype(int))

plt.yticks(range(NUM_PAGES))

plt.tight_layout()
plt.show()
