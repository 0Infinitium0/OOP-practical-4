# Practical 04: RAII and Smart Pointers

- **Student Name:** Satinderpal Singh Chaney
- **Roll Number:** E26B00S2034
- **Repository:** `practical 4`

## Aim
To learn how RAII (Resource Acquisition Is Initialization) ties resource lifetime to object lifetime and reduces manual memory-management errors in C++.

## Question
Replace suitable raw `new`/`delete` usage with `std::unique_ptr` and `std::shared_ptr`. Compare behaviour and check for memory leaks.

## Overview of Implementation (`practical4.cpp`)
1. **`std::unique_ptr` (Exclusive Ownership):**
   - Allocates a `Resource` object via `std::make_unique<Resource>(100)` and a dynamic array via `std::make_unique<int[]>(3)`.
   - Demonstrates ownership transfer using `std::move` (copying is prevented at compile time).
   - Automatically invokes `delete` / `delete[]` when leaving scope.
2. **`std::shared_ptr` (Shared Ownership):**
   - Allocates a shared `Resource` via `std::make_shared<Resource>(200)` (`use_count() == 1`).
   - Shares ownership with a second `std::shared_ptr` in an inner scope (`use_count() == 2`).
   - Automatically decrements the reference count to `1` when the inner pointer exits scope, and releases memory when the last owner is destroyed.
3. **Iteration Stress Check (`100,000` cycles):**
   - Exercises repeated allocation and cleanup across 100,000 iterations to verify leak-free execution under AddressSanitizer / Valgrind.

## Comparison: Raw Pointers vs `std::unique_ptr` vs `std::shared_ptr`

| Feature / Aspect | Raw Pointer (`T*`) | `std::unique_ptr<T>` | `std::shared_ptr<T>` |
| :--- | :--- | :--- | :--- |
| **Ownership Model** | Unclear / manual convention | Exclusive (single owner) | Shared (multiple owners via reference count) |
| **Deallocation** | Manual (`delete` / `delete[]`) | Automatic (RAII at scope exit) | Automatic when `use_count()` drops to `0` |
| **Copyable?** | Yes (shallow pointer copy; risks double-free) | No (compile-time error) | Yes (increments atomic reference count) |
| **Movable?** | Yes (leaves source dangling unless nulled) | Yes (`std::move` transfers ownership; source becomes `nullptr`) | Yes (`std::move` transfers ownership without changing `use_count()`) |
| **Memory Overhead** | 1 pointer (`sizeof(void*)`) | Zero overhead (1 pointer, same as raw pointer) | 2 pointers (object pointer + control block pointer) |
| **Runtime Overhead** | None | None | Atomic increment/decrement on copy/destruction |
| **Exception Safety** | Unsafe if exception occurs before `delete` | Exception-safe (stack unwinding cleans up) | Exception-safe (stack unwinding cleans up) |
| **When to Use** | Non-owning observation only (never for ownership) | Default choice for owning dynamic memory | Only when multiple owners genuinely share lifetime |

## Compilation & Execution

### Windows (PowerShell / MinGW `g++`)
```powershell
g++ -std=c++17 -Wall -Wextra practical4.cpp -o practical4.exe
.\practical4.exe
```

### Linux / macOS (`g++` / `clang++`)
```bash
g++ -std=c++17 -Wall -Wextra practical4.cpp -o practical4
./practical4
```

## Memory-Checking Instructions & Results

### Option 1: AddressSanitizer & LeakSanitizer (GCC / Clang — Linux, macOS, WSL)
```bash
g++ -std=c++17 -g -O1 -fsanitize=address,undefined practical4.cpp -o practical4_asan
./practical4_asan
```
- **Test Conditions:** Compiled with `g++ -std=c++17 -g -O1 -fsanitize=address,undefined` across `100,000` allocation/deallocation iterations plus `unique_ptr` and `shared_ptr` demonstrations.
- **Observation:** Exited cleanly with `0` memory leaks, `0` heap-use-after-free errors, and `0` double-free errors reported by AddressSanitizer/LeakSanitizer.

### Option 2: Valgrind (Linux / WSL)
```bash
g++ -std=c++17 -g practical4.cpp -o practical4_dbg
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./practical4_dbg
```
- **Expected Summary:** `All heap blocks were freed -- no leaks are possible` and `ERROR SUMMARY: 0 errors from 0 contexts`.
*(Note: Benchmark and memory metrics depend on compiler flags, OS allocator, and hardware.)*

## Uploading to GitHub (`practical 4`)
1. Create a new repository on GitHub named **`practical 4`** (do not initialize with a README).
2. Extract the ZIP archive, open a terminal inside the folder, and run:
   ```bash
   git init
   git add .
   git commit -m "Add Practical 04: RAII and Smart Pointers"
   git branch -M main
   git remote add origin <YOUR_GITHUB_REPOSITORY_URL>
   git push -u origin main
   ```
