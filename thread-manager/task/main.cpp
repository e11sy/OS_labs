#include <iostream>
#include <chrono>
#include <omp.h>
#include <vector>

const int N = 100000000;
const int STUDENT_ID = 331410;
const int BLOCK_SIZE = 10 * STUDENT_ID;

int main() {
    std::vector<int> thread_counts = {1, 2, 4, 8, 12, 16};

    for (int num_threads : thread_counts) {
        double pi = 0.0;

        auto start_time = std::chrono::high_resolution_clock::now();


        omp_set_num_threads(num_threads); 
        omp_set_schedule(omp_sched_dynamic, BLOCK_SIZE);
        #pragma omp parallel for schedule(runtime) reduction(+:pi)
        for (int i = 0; i < N; ++i) {
            double x = (i + 0.5) / N;
            pi += 4.0 / (1.0 + x * x);
        }

        pi /= N;

        auto end_time = std::chrono::high_resolution_clock::now();
        double seconds = std::chrono::duration<double>(end_time - start_time).count();

        std::cout << "Threads: " << num_threads << ", Pi ≈ " << pi << ", Time: " << seconds << "s\n";
    }

    return 0;
}
