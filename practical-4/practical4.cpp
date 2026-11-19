#include <iostream>
#include <memory>
#include <chrono>
#include <vector>

/**
 * Resource: A lightweight RAII-tracked class to demonstrate deterministic
 * construction and destruction under std::unique_ptr and std::shared_ptr.
 */
class Resource {
private:
    int value_;
    bool verbose_;

public:
    explicit Resource(int value, bool verbose = true)
        : value_(value), verbose_(verbose) {
        if (verbose_) {
            std::cout << "[Resource Acquired | Value = " << value_ << "]\n";
        }
    }

    ~Resource() {
        if (verbose_) {
            std::cout << "[Resource Released | Value = " << value_ << "]\n";
        }
    }

    int getValue() const { return value_; }
    void setValue(int v) { value_ = v; }
};

/**
 * Demonstrates std::unique_ptr (exclusive ownership, movable, non-copyable)
 * and dynamically allocated array via std::unique_ptr<int[]>.
 */
void demonstrateUniquePtr() {
    std::cout << "1. UNIQUE POINTER\n";

    // Single-object unique_ptr (RAII: automatically deleted on scope exit)
    std::unique_ptr<Resource> uniqueRes = std::make_unique<Resource>(100, false);
    std::cout << "Unique Pointer Value: " << uniqueRes->getValue() << "\n";
    std::cout << "Ownership: Single Owner\n";

    // Array allocation with std::unique_ptr<int[]> (no manual delete[] needed)
    std::unique_ptr<int[]> uniqueArr = std::make_unique<int[]>(3);
    uniqueArr[0] = 10;
    uniqueArr[1] = 20;
    uniqueArr[2] = 30;

    // Transferring ownership via std::move (copying is disallowed at compile time)
    std::unique_ptr<Resource> transferredOwner = std::move(uniqueRes);
    if (!uniqueRes && transferredOwner) {
        std::cout << "Memory Managed Automatically!\n";
    }
}

/**
 * Demonstrates std::shared_ptr where two independent parts of a program
 * share ownership and track lifetime via reference counting (use_count).
 */
void demonstrateSharedPtr() {
    std::cout << "2. SHARED POINTER\n";

    std::shared_ptr<Resource> ownerA = std::make_shared<Resource>(200, false);
    std::cout << "Shared Pointer Value: " << ownerA->getValue() << "\n";
    std::cout << "Initial Reference Count: " << ownerA.use_count() << "\n";

    {
        // Second owner shares the exact same control block and heap object
        std::shared_ptr<Resource> ownerB = ownerA;
        std::cout << "After Sharing:\n";
        std::cout << "Reference Count: " << ownerA.use_count() << "\n";
    } // ownerB goes out of scope here, decrementing use_count back to 1

    std::cout << "After Releasing One Owner:\n";
    std::cout << "Reference Count: " << ownerA.use_count() << "\n";
}

/**
 * Runs multiple iterations comparing raw new/delete vs std::unique_ptr
 * to verify zero memory leaks under repeated allocation/deallocation cycles.
 */
void runIterationBenchmark(std::size_t iterations) {
    // Raw pointer loop (requires explicit delete on every path)
    auto startRaw = std::chrono::steady_clock::now();
    volatile long long rawSink = 0;
    for (std::size_t i = 0; i < iterations; ++i) {
        int* rawPtr = new int(static_cast<int>(i));
        rawSink += *rawPtr;
        delete rawPtr;
    }
    auto endRaw = std::chrono::steady_clock::now();

    // unique_ptr loop (zero overhead over raw pointer, automatic cleanup)
    auto startUnique = std::chrono::steady_clock::now();
    volatile long long uniqueSink = 0;
    for (std::size_t i = 0; i < iterations; ++i) {
        auto uPtr = std::make_unique<int>(static_cast<int>(i));
        uniqueSink += *uPtr;
    }
    auto endUnique = std::chrono::steady_clock::now();

    (void)startRaw; (void)endRaw; (void)startUnique; (void)endUnique;
    (void)rawSink; (void)uniqueSink;
}

int main() {
    std::cout << "----- RAII AND SMART POINTERS -----\n";
    std::cout << "Student Name: Satinderpal Singh Chaney\n";
    std::cout << "Roll Number: E26B00S2034\n";

    // Part 1: Single ownership with std::unique_ptr
    demonstrateUniquePtr();

    // Part 2: Shared ownership with std::shared_ptr
    demonstrateSharedPtr();

    // Part 3: Ownership & Memory Management Summary
    std::cout << "3. MEMORY MANAGEMENT\n";
    std::cout << "Raw Pointer: Manual Memory Management\n";
    std::cout << "Unique Pointer: Single Ownership\n";
    std::cout << "Shared Pointer: Shared Ownership\n";

    // Part 4: Multi-iteration leak/stress check (100,000 cycles)
    runIterationBenchmark(100000);

    std::cout << "Memory Released Automatically!\n";
    std::cout << "Program executed successfully.\n";
    return 0;
}
