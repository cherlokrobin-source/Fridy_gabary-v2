# 🌌 Gabary V2 & Djomoa Temporal Engine
> **High-Performance Deterministic Calendar Engine & TUI Dashboard for 50,000-Year Horizon Tracking**
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Termux%20%7C%20Linux-orange.svg)]()
[![Architecture](https://img.shields.io/badge/Architecture-Modular-green.svg)]()
---
## 📌 Executive Summary & Project Significance
**Gabary V2** is an advanced, high-precision Terminal User Interface (TUI) dashboard built around the **Djomoa Solar Engine V2**. It is engineered to solve a fundamental challenge in long-range computational chronometry: **calculating and visualizing absolute time tracking over a 50,000-year horizon in constant time $\mathcal{O}(1)$ without relying on iterative date loops or heavy database indexing.**
### 💡 Why It Matters to Systems Engineers:
1. **Zero-Iteration Chronometry:** Standard date libraries often rely on sequential iteration or cache tables for massive temporal offsets. Djomoa Engine computes calendar attributes strictly using deterministic algebraic transformations.
2. **Minimal Resource Footprint:** Designed specifically to execute natively in resource-constrained environments (e.g., Termux on ARM mobile platforms) with a memory footprint strictly under **1.0 MB Stack**.
3. **High-Density TUI System Architecture:** Utilizes modern functional C++ reactive UI components via FTXUI to render live data metrics, dynamic academic monthly matrices, and long-range horizon gauges at high frame rates.
---
## ⚙️ Core Technical Specifications

| Parameter | Specification | Engineering Rationale |
| :--- | :--- | :--- |
| **Language Standard** | C++17 | Clean abstractions, high runtime speed, strict type safety |
| **UI Framework** | FTXUI (Functional TUI) | Reactive component rendering without heavy GUI overhead |
| **Time Complexity** | $\mathcal{O}(1)$ Constant Time | Direct arithmetic index resolution for any Absolute Day ID |
| **Space Complexity** | $\mathcal{O}(1)$ | Memory-bounded execution ($< 1.0 \text{ MB}$ Stack usage) |
| **Temporal Horizon** | 50,000 Solar Years | ~18,262,125 unique absolute daily records |
| **Mean Year Model** | 365.2425 Mean Days | Strict adherence to 400-year leap cycle rules |

---
## 📐 System Architecture & Module Division
The repository follows a clean **Separation of Concerns (SoC)** modular paradigm to ensure testability and fast incremental compilation:
```text
Fridy_gabary-v2_CLEAN/
├── CMakeLists.txt             # Build automation & dependency manager
├── Gabary_V2_UI/
│   ├── SolarEngineV2.h/cpp    # Core deterministic time-calculation engine
│   ├── GlobalSolarDay.h/cpp   # Data models for solar state representation
│   ├── GabaryUI.h/cpp         # Custom TUI layout & progress bar renderers
│   └── main.cpp               # Event loop handler & key-binding routing
└── README.md                  # Comprehensive technical documentation
