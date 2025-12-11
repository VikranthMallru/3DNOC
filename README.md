## Project Overview

This project implements a configurable simulator for testing various 3-D stacked cache network configurations in a Network-on-Chip (NoC) environment. The simulator supports multiple build configurations (e.g., verbose logging, debug symbols, and optimized release builds) and uses multithreading via Pthreads.

---

## Prerequisites

* A Unix-like environment (Linux or macOS)
* GNU `g++` (version 9.0 or later recommended)
* `make` build tool
* Pthreads library (usually included by default)

---

## Repository Structure

```
├── build/                   # Output directory for object files and binaries
├── compression/             # Compression module source files
├── config/                  # Configuration parsing files
├── controller/              # Cache controller logic
├── integration/             # Integration tests and harnesses
├── logger/                  # Logging utilities
├── multithreading/          # Threading support and abstractions
├── packet/                  # Packet definitions and handling
├── port/                    # Port-level interfaces
├── prefetcher/              # Prefetcher implementations
├── protocol/                # Communication protocols
├── routing/                 # Routing algorithms
├── stats/                   # Statistics collection
├── storage/                 # Storage-layer abstractions
├── structures/              # Data structures
├── test/                    # Unit and integration test files
├── topology/                # Network topology builders
├── types/                   # Shared type definitions
├── Makefile                 # Build instructions
└── README.md                # Project documentation (this file)
```

---

## Getting Started

Follow these steps to clone, build, and run the simulator.

### 1. Clone the Repository

```bash
git clone https://github.com/<username>/<repository>.git
cd <repository>
```

### 2. Changes

Change the path and log path in src/input/sim.cfg file. 

### 3. Configure Build Options

You can customize the build by setting one of the following flags when invoking `make`:

* **Verbose (with logging):**

  ```bash
  make VERBOSE=1
  ```

  * Enables `VERBOSE_LOG` and compiles with `-O3 -w -Wall -D VERBOSE_LOG`.
  * Output binary: `build/ucc_verbose`

* **Verbose (no logging):**

  ```bash
  make VERBOSE_NOLOG=1
  ```

  * Compiles with `-O3 -g -w -Wall` (no logging).
  * Output binary: `build/ucc_verbose`

* **Debug:**

  ```bash
  make DEBUG=1
  ```

  * Enables `VERBOSE_LOG` and compiles with `-g -w -Wall -D VERBOSE_LOG`.
  * Output binary: `build/ucc_debug`

* **Release (optimized):**

  ```bash
  make RELEASE=1
  ```

  * Compiles with `-O3 -w -Wall` (no debug symbols).
  * Output binary: `build/ucc_release`

> **Note:** If no flag is provided, `make` will use the last set configuration or default to the first target defined.

### 4. Build the Project

Once you’ve chosen a build configuration, simply run:

```bash
make all
```

This will create the `build/` directory (if not already present), compile all `.cpp` source files into object files, and link them into the target binary.

### 5. Run the Simulator

Activate the binary corresponding to your configuration:

```bash
# For release build:
./build/ucc_release [options]

# For debug build:
./build/ucc_debug [options]
```

Replace `[options]` with the sim.cfg file path and any extra parameters you wish to add.

### 6. Clean Build Artifacts

To remove all generated object files and binaries and reset the `build/` directory:

```bash
make clean
```

This will delete and recreate the `build/` folder as empty.

---

## Additional Notes

* **Threading:** The project is compiled with `-DENABLE_PTHREADS=ON`. Ensure your system supports Pthreads.
* **Extending the Simulator:** Add new modules under the appropriate directories and update the Makefile if you introduce new source folders.
* **Logging:** Logging macros are controlled by the `VERBOSE_LOG` definition. You can sprinkle `LOG(...)` calls in code to output runtime information when enabled.

---

Happy simulating! Feel free to open issues or pull requests on GitHub for suggestions or bug reports.
