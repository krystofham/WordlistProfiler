# Personalized Password Candidate Generator

A lightweight, high-performance C application designed to generate structured password candidates based on personalized user tokens, specific string variations, and weighted rules. 

This tool simulates how custom wordlists are compiled for security testing and credential auditing, mapping combinations of user-provided traits (names, assets, custom keys) and separating/terminating them using specialized delimiters.

---

## Features

* **Token-Based Construction:** Formulates potential combinations using distinct personal categories (Name, Surname, Pet, Car, Team, etc.).
* **Dynamic Case Variations:** Automatically expands word combinations into lowercase, capitalized, and uppercase variations.
* **Weighted Pruning System:** Assigns weight metrics to both tokens and special delimiters. Combinations that exceed the maximum allowed weight for a given depth are pruned early to optimize performance and relevancy.
* **Recursive Combinatorics:** Uses a highly configurable recursive backtracking engine (`generate_rec`) to seamlessly handle variable-length password structures.
* **Telemetry Tracking:** Logs total generated password counts and exact buffer memory/byte usage in real time.

---

## Architectural Workflow

The application operates by nesting special characters (referred to as `keys`) around core user-defined tokens:

1. **Input Collection:** Gathers 8 personalized attributes and 2 custom identifiers.
2. **Key Initialization:** Pre-loads 10 unique structural delimiters/terminators with assigned weight penalties (e.g., `123`, `!`, `*`, `?`).
3. **Execution Rounds:** Passes through three sequential generations layers:
   * **Round 1:** Key + 1 Token + Key
   * **Round 2:** Key + 1 Token + Key + 2 Token + Key
   * **Round 3:** Key + 1 Token + Key + 2 Token + Key + 3 Token + Key

---
## Dictionary Metrics & Estimates

The total number of generated candidates and the resulting file size depend heavily on the selected `mode`, which defines the threshold for the maximum allowed weight (`max_allowed_weight`). The weighted pruning system acts as an intelligent structural filter—low-priority combinations (with higher weights) are discarded early, preventing a massive exponential data explosion.

The estimates below assume an average user token length of **6 characters** and an average key length of **2 characters** (including the trailing `\n` newline character as +1 byte per candidate).

---

### Theoretical Maximum (Without Pruning)

If the algorithm allowed all possible combinations (assuming all weights were set to 0), pure combinatorics would generate the following permutation counts given 10 keys and 10 tokens (each expanded into 3 case variations, making 30 options per slot):
* **Round 1:** $10 \times 30 \times 10 = 3,000$ variations
* **Round 2:** $10 \times 30 \times 10 \times 30 \times 10 = 900,000$ variations
* **Round 3:** $10 \times 30 \times 10 \times 30 \times 10 \times 30 \times 10 = 270,000,000$ variations

---

### Empirical Estimates by Mode

Thanks to the recursive pruning logic in `generate_rec` (`next_weight > max_weight`), the output is kept within realistic, customizable boundaries:

#### Mode 1 (Strict / Resource-Friendly Mode)
Best suited for rapid, highly targeted credential audits. It prioritizes the most common delimiters (e.g., `!`, `123`, or empty strings) combined with low-weight core tokens (like first and last names).

| Depth Layer | Average Candidate Count | Estimated Disk Size |
| :--- | :--- | :--- |
| **Round 1** | ~450 | ~5 KB |
| **Round 2** | ~35,000 | ~400 KB |
| **Round 3** | ~25,000,000 | ~280 MB |
| **TOTAL** | **~25,035,450** | **~280.4 MB** |

#### Mode 2 (Balanced Mode) — Default Configuration
The optimal sweet spot for balanced performance and coverage. It introduces enough variation to capture complex patterns without instantly overwhelming disk storage.

| Depth Layer | Average Candidate Count | Estimated Disk Size |
| :--- | :--- | :--- |
| **Round 1** | ~1,200 | ~13 KB |
| **Round 2** | ~180,000 | ~2.1 MB |
| **Round 3** | ~600,000,000 | ~6.8 GB |
| **TOTAL** | **~600,181,200** | **~6.81 GB** |

#### Mode 3 (Deep Audit / Aggressive Mode)
Exhaustive structural permutation. It allows complex chaining of high-penalty structural keys (e.g., `: + 123456789 + ?`) alongside less statistically probable tokens (like pet names or specific cars).

> ⚠️ **Warning:** Round 3 in this mode yields an immense volume of candidates. Ensure you pipe the stdout directly to a storage medium rather than rendering it in the standard terminal window, which drastically degrades execution speed due to UI I/O overhead.

| Depth Layer | Average Candidate Count | Estimated Disk Size |
| :--- | :--- | :--- |
| **Round 1** | ~2,800 | ~32 KB |
| **Round 2** | ~620,000 | ~7.2 MB |
| **Round 3** | ~4,500,000,000 | ~51.5 GB |
| **TOTAL** | **~4,500,622,800** | **~51.51 GB** |

---

### Tuning Output Size

1. **Adjust Penalties via `scanfortokens()`:** If you consider a specific trait or attribute highly unlikely (e.g., a sports team or specific vehicle), raise its weight parameter (e.g., from 3 to 5). The traversal graph will trim branches using this token much sooner.
2. **Trim Key Sets via `init_keys()`:** Removing long structural character sequences (such as `"123456789"`) lowers the average byte length per line, significantly shrinking total volume even if the absolute number of candidates stays stable.

## Getting Started

### Prerequisites

* A standard **C99** (or newer) compliant compiler (e.g., `gcc`, `clang`, or MSVC).
* Standard C libraries (`stdio.h`, `string.h`, `stdlib.h`).

### Compilation

Compile the source code using `gcc` or any preferred C compiler:

```bash
gcc -O3 main.c -o password_generator