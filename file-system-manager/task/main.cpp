#include <iostream>
#include <cstring>
#include <cerrno>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <aio.h>
#include <signal.h>
#include <chrono>

// Global configuration
static int BLOCK_SIZE = -1;
static int THREAD_COUNT = 1;
static off_t FILE_SIZE = 0;

// File descriptors
static int src_fd = -1;
static int dst_fd = -1;

// Shared buffer
static char* buffer = nullptr;

// State control
std::atomic<int64_t> current_offset = 0;
std::atomic<int> pending_operations = 0;
static std::mutex mtx;
static std::condition_variable cv;

// Error handling
[[noreturn]] static void die(const char* msg) {
    std::cerr << "[" << errno << "] " << strerror(errno) << ": " << msg << std::endl;
    std::abort();
}

// Async I/O operation structure
struct aio_operation {
    int src_fd;
    int dst_fd;
    char* buffer_base;
    bool is_write;
    struct aiocb cb;
};

// Completion handler for AIO
void aio_completion_handler(sigval sigval_t) {
    auto* op = reinterpret_cast<aio_operation*>(sigval_t.sival_ptr);
    if (!op) return;

    if (!op->is_write) {
        // Begin write after read completes
        op->is_write = true;
        op->cb.aio_fildes = op->dst_fd;

        if (aio_write(&op->cb) < 0) {
            die("aio_write() failed in completion handler");
        }
        return;
    }

    // Write completed
    int64_t offset = current_offset.fetch_add(BLOCK_SIZE);
    if (offset < 0) die("int64_t overflow");

    if (offset >= FILE_SIZE) {
        if (--pending_operations == 0) {
            cv.notify_one();
        }
        delete op;
        return;
    }

    // Prepare next read operation
    op->is_write = false;
    op->cb.aio_fildes = op->src_fd;
    op->cb.aio_offset = offset;

    size_t bytes_to_read = (offset + BLOCK_SIZE > FILE_SIZE)
                            ? FILE_SIZE - offset
                            : BLOCK_SIZE;

    op->cb.aio_nbytes = bytes_to_read;
    op->cb.aio_buf = op->buffer_base + offset;

    if (aio_read(&op->cb) < 0) {
        die("aio_read() failed in completion handler");
    }
}

// Starts initial set of AIO read operations
void start_read_operations() {
    current_offset.store(0);

    for (int i = 0; i < THREAD_COUNT; ++i) {
        auto* op = new aio_operation{src_fd, dst_fd, buffer, false, {}};

        int64_t offset = current_offset.fetch_add(BLOCK_SIZE);
        if (offset >= FILE_SIZE) {
            delete op;
            break;
        }

        size_t bytes_to_read = (offset + BLOCK_SIZE > FILE_SIZE)
                                ? FILE_SIZE - offset
                                : BLOCK_SIZE;

        op->cb.aio_fildes = src_fd;
        op->cb.aio_offset = offset;
        op->cb.aio_buf = buffer + offset;
        op->cb.aio_nbytes = bytes_to_read;

        op->cb.aio_sigevent.sigev_notify = SIGEV_THREAD;
        op->cb.aio_sigevent.sigev_notify_function = aio_completion_handler;
        op->cb.aio_sigevent.sigev_value.sival_ptr = op;


        if (aio_read(&op->cb) < 0) {
            delete op;
            die("aio_read() failed");
        }

        pending_operations++;
    }
}

// Open input/output files
void initialize_fds(const char* src_path, const char* dst_path) {
    src_fd = open(src_path, O_RDONLY);
    dst_fd = open(dst_path, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (src_fd < 0 || dst_fd < 0) die("Failed to open source/destination file");

    struct stat st;
    if (fstat(src_fd, &st) < 0) die("fstat() failed");
    FILE_SIZE = st.st_size;

    std::cout << "FILE_SIZE: " << FILE_SIZE << std::endl;
}

// Allocate memory buffer
void initialize_buffer() {
    buffer = new char[FILE_SIZE];
    if (!buffer) die("Memory allocation failed");
}

// Main entry point
int main(int argc, char** argv) {
    if (argc < 5) {
        die("Usage: <src> <dst> <thread_count> <block_size> [runs]");
    }

    THREAD_COUNT = std::stoi(argv[3]);
    BLOCK_SIZE = std::stoi(argv[4]) * 1024;
    if (THREAD_COUNT <= 0 || BLOCK_SIZE <= 0) {
        die("Invalid thread count or block size");
    }

    int test_runs = (argc > 5) ? std::stoi(argv[5]) : 1;
    if (test_runs <= 0) die("Invalid test runs count");

    initialize_fds(argv[1], argv[2]);
    initialize_buffer();

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < test_runs; ++i) {
        start_read_operations();

        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return pending_operations.load() == 0; });
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto average_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() / test_runs;

    std::cout << "Average duration (ms): " << average_duration << std::endl;

    close(src_fd);
    close(dst_fd);
    delete[] buffer;

    return 0;
}
