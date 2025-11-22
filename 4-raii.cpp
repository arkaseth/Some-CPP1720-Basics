// raii.cpp
// Several small RAII examples: file wrapper, unique_ptr with custom deleter,
// a simple ScopedLock, and a ScopedTimer.
// Compile with: g++ -std=c++17 raii.cpp -pthread -O2

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <chrono>

// Simple RAII wrapper for a FILE* (moveable, non-copyable).
class File {
public:
    explicit File(const char* path, const char* mode) {
        file_ = std::fopen(path, mode);
        if (!file_) {
            std::perror("fopen");
        }
    }

    ~File() {
        if (file_) {
            std::fclose(file_);
        }
    }

    // non-copyable
    File(const File&) = delete;
    File& operator=(const File&) = delete;

    // moveable
    File(File&& other) noexcept : file_(other.file_) {
        other.file_ = nullptr;
    }
    File& operator=(File&& other) noexcept {
        if (this != &other) {
            if (file_) std::fclose(file_);
            file_ = other.file_;
            other.file_ = nullptr;
        }
        return *this;
    }

    bool valid() const { return file_ != nullptr; }

    // write bytes; returns number of bytes written
    std::size_t write(const void* data, std::size_t bytes) {
        if (!file_) return 0;
        return std::fwrite(data, 1, bytes, file_);
    }

    // convenience for text
    void puts(const std::string& s) {
        if (!file_) return;
        std::fwrite(s.data(), 1, s.size(), file_);
        std::fwrite("\n", 1, 1, file_);
    }

private:
    std::FILE* file_ = nullptr;
};

// Example of using std::unique_ptr with a custom deleter for FILE*.
using FilePtr = std::unique_ptr<std::FILE, decltype(&std::fclose)>;

// Simple manual RAII lock that wraps std::mutex::lock/unlock.
// This mirrors std::lock_guard but shows the pattern.
class ScopedLock {
public:
    explicit ScopedLock(std::mutex& m) : m_(m) { m_.lock(); }
    ~ScopedLock() noexcept { m_.unlock(); }

    // non-copyable and non-movable
    ScopedLock(const ScopedLock&) = delete;
    ScopedLock& operator=(const ScopedLock&) = delete;

private:
    std::mutex& m_;
};

// A tiny RAII timer that logs duration when it goes out of scope.
class ScopedTimer {
public:
    explicit ScopedTimer(const char* name) : name_(name), start_(Clock::now()) {}
    ~ScopedTimer() {
        auto end = Clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_).count();
        std::cout << "[Timer] " << name_ << " took " << ms << " ms\n";
    }

private:
    using Clock = std::chrono::high_resolution_clock;
    const char* name_;
    Clock::time_point start_;
};

int main() {
    // 1) RAII File wrapper usage
    {
        ScopedTimer t("FileWrapper block");
        File f("example.txt", "w");
        if (f.valid()) {
            f.puts("Hello from File RAII wrapper");
            f.puts("Another line");
            // File closed automatically at scope end
        }
    }

    // 2) unique_ptr with custom deleter
    {
        ScopedTimer t("unique_ptr FILE block");
        FilePtr fp(std::fopen("example2.txt", "w"), &std::fclose);
        if (fp) {
            const char* msg = "Hello from unique_ptr with custom deleter\n";
            std::fwrite(msg, 1, std::strlen(msg), fp.get());
            // fp will call fclose automatically
        }
    }

    // 3) RAII locking demonstration with threads
    {
        ScopedTimer t("Mutex + ScopedLock block");
        std::mutex m;
        int counter = 0;

        auto worker = [&](int id) {
            for (int i = 0; i < 1000; ++i) {
                ScopedLock lock(m); // locks on entry, unlocks on exit
                ++counter;
                // lock released at end of this scope
            }
            // show each thread finished (no locking needed for single print here)
        };

        std::vector<std::thread> threads;
        for (int i = 0; i < 4; ++i) threads.emplace_back(worker, i);
        for (auto& th : threads) th.join();

        std::cout << "Counter after threads: " << counter << " (expected 4000)\n";
    }

    // 4) demonstrating unique_ptr for dynamically allocated memory
    {
        ScopedTimer t("unique_ptr memory block");
        std::unique_ptr<int[]> arr(new int[1000]); // frees automatically
        for (int i = 0; i < 1000; ++i) arr[i] = i;
        std::cout << "arr[42] = " << arr[42] << "\n";
    }

    return 0;
}