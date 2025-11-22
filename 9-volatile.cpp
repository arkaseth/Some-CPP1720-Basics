// volatile.cpp
// Demo: correct use of volatile (signal handler) and recommended alternative for threads.
//
// Build: g++ -std=c++17 volatile.cpp -pthread -o volatile_demo
// Run and press Ctrl-C to stop the signal-handler demo.

#include <csignal>
#include <cstdint>
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

// Correct use for interaction with a signal handler:
volatile sig_atomic_t g_stop_by_signal = 0;

extern "C" void signal_handler(int) {
    // Only async-signal-safe operations are allowed here.
    g_stop_by_signal = 1; // volatile sig_atomic_t is recommended for signals.
}

// Wrong tool for thread synchronization: volatile does not give atomicity or happens-before.
// For threads, use std::atomic.
void thread_demo() {
    std::cout << "--- thread demo (use std::atomic) ---\n";
    std::atomic<bool> stop{false};

    std::thread worker([&]{
        std::uint64_t counter = 0;
        while (!stop.load(std::memory_order_relaxed)) {
            ++counter;
        }
        std::cout << "Worker stopped, counter=" << counter << '\n';
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    stop.store(true, std::memory_order_relaxed);
    worker.join();
}

// Signal-demo: uses volatile sig_atomic_t to safely communicate with a signal handler.
// The volatile tells the compiler not to optimize away repeated reads of the flag.
void signal_demo() {
    std::cout << "\n--- signal demo (volatile sig_atomic_t) ---\n";
    std::cout << "PID: " << getpid() << ". Press Ctrl-C to set the flag and stop.\n";

    std::signal(SIGINT, signal_handler);

    int ticks = 0;
    while (!g_stop_by_signal) {
        // The compiler must actually load g_stop_by_signal each loop iteration
        // because it is volatile, otherwise it could hoist the load out of the loop.
        if ((++ticks % 10) == 0) {
            std::cout << "tick " << ticks << '\n';
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    std::cout << "Signal received, exiting signal demo.\n";
}

int main() {
    thread_demo();

    // Run the signal demo last because it waits for Ctrl-C from the user.
    signal_demo();
    return 0;
}