# Combinatorial Priority Vector Calculation with OpenMP

## Overview

This project implements a **parallel combinatorial algorithm** for determining the priority vector (weights of objects) based on an incomplete matrix of expert pairwise comparisons. The solution leverages **OpenMP** to accelerate exhaustive search over all spanning trees of a complete graph, using Prüfer sequences for efficient tree enumeration.

**Key features:**
- Generation of all spanning trees via Prüfer sequences (Cayley's formula)
- Validation of trees against the incomplete pairwise comparison matrix
- Calculation of priority vectors using **arithmetic** and **geometric** means
- Parallel execution with OpenMP (`#pragma omp parallel for reduction`)
- Race-condition-free design with thread-local accumulators
- Diagnostic test suite for correctness validation

### Prerequisites

- **C++17** compatible compiler with OpenMP support
- **macOS/Linux/Windows** (tested on macOS with Apple Clang + Homebrew OpenMP)

#### Installation on macOS
```bash
brew install libomp
```

### Build Instructions

Generate the build files and compile the executables:

```bash
cmake ..
make
```
Usage
After a successful build, the compiled executables will be located in the build directory.

# Run the sequential version
./task1

# Run the parallel OpenMP version
./task2


**Interactive input:**
1. Enter the number of objects (`n`)
2. Fill the pairwise comparison matrix (enter `0` for missing comparisons)
3. Choose averaging method:
   - `0` — Arithmetic mean
   - `1` — Geometric mean


## Performance Summary

The algorithm demonstrates **significant speedup** for larger problem sizes, achieving **up to 4.03×** acceleration on an 8-core processor.

| Dimension (N) | Trees (N^(N-2)) | Speedup (S) |
|---------------|-----------------|-------------|
| 5             | 125             | 0.40        |
| 6             | 1,296           | 0.75        |
| 7             | 16,807          | 1.87        |
| 8             | 262,144         | **4.03**    |

>  **Detailed performance analysis, including sequential and parallel execution times, is available in results.docx.**

##  Algorithm Overview

1. **Prüfer Sequence Generation**: All possible Prüfer sequences of length `(n-2)` are generated, each corresponding to a unique spanning tree (Cayley's formula: `n^(n-2)` trees).

2. **Tree Validation**: Each generated tree is checked against the incomplete matrix — if any edge is missing (`0`), the tree is discarded.

3. **Priority Vector Calculation**: For each valid tree, weights are computed using transitivity:
   - Start from vertex `0` with weight `1.0`
   - Propagate weights along edges: `w[v] = w[u] / A[u][v]`

4. **Averaging**: Accumulate weights across all valid trees using either:
   - **Arithmetic mean**: `(1/k) * Σ w_i`
   - **Geometric mean**: `exp((1/k) * Σ ln(w_i))`

5. **Normalization**: Final vector is normalized to sum to `1.0`.
