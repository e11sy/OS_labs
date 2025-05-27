#include <windows.h>
#include <iostream>
#include <vector>
#include <chrono>

const int N = 100000000;
const int STUDENT_ID = 331410;
const int BLOCK_SIZE = 10 * STUDENT_ID;

volatile long current_block = 0;
int total_blocks = (N + BLOCK_SIZE - 1) / BLOCK_SIZE;
double pi = 0.0;
CRITICAL_SECTION cs_sum;

struct ThreadData {
    int thread_index;
    HANDLE thread_handle;
};

DWORD WINAPI PiWorker(LPVOID param) {
    while (true) {
        int block_index;
        // Получаем номер блокаы
        block_index = InterlockedIncrement(&current_block) - 1;
        if (block_index >= total_blocks) break;

        int start = block_index * BLOCK_SIZE;
        int end = std::min(start + BLOCK_SIZE, N);

        double local_sum = 0.0;
        for (int i = start; i < end; ++i) {
            double x = (i + 0.5) / N;
            local_sum += 4.0 / (1.0 + x * x);
        }

        EnterCriticalSection(&cs_sum);
        pi += local_sum;
        LeaveCriticalSection(&cs_sum);
    }

    return 0;
}

void run_test(int num_threads) {
    pi = 0.0;
    current_block = 0;
    InitializeCriticalSection(&cs_sum);

    std::vector<ThreadData> threads(num_threads);

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_threads; ++i) {
        threads[i].thread_handle = CreateThread(
            nullptr, 0, PiWorker, nullptr, 0, nullptr
        );
    }

    for (int i = 0; i < num_threads; ++i) {
        WaitForSingleObject(threads[i].thread_handle, INFINITE);
        CloseHandle(threads[i].thread_handle);
    }

    pi /= N;

    auto end_time = std::chrono::high_resolution_clock::now();
    double seconds = std::chrono::duration<double>(end_time - start_time).count();

    std::cout << "Threads: " << num_threads << ", Pi ≈ " << pi
              << ", Time: " << seconds << "s\n";

    DeleteCriticalSection(&cs_sum);
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::vector<int> thread_counts = {1, 2, 4, 8, 12, 16};
    for (int n : thread_counts) {
        run_test(n);
    }
    return 0;
}
