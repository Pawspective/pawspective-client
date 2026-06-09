# 🐾 Pawspective Client

**Desktop Qt client for the Pawspective pet-care platform, built with C++20 and Qt 6.**

[![C++](https://img.shields.io/badge/C%2B%2B-20-blue?logo=cplusplus&logoColor=white)](https://en.cppreference.com/w/cpp/20)
[![Qt](https://img.shields.io/badge/Qt-6-41CD52?logo=qt&logoColor=white)](https://www.qt.io/)
[![QML](https://img.shields.io/badge/QML-UI-41CD52?logo=qt&logoColor=white)](https://doc.qt.io/qt-6/qmlapplications.html)
[![License: GPL-3.0](https://img.shields.io/badge/License-GPL--3.0-green.svg)](LICENSE)
[![CI](https://github.com/Pawspective/pawspective-client/actions/workflows/ci.yml/badge.svg)](https://github.com/Pawspective/pawspective-client/actions)

---

[Quick Start](#-quick-start) · [Architecture](#-architecture) · [Development](#-development) · [Testing](#-testing) · [Contributing](#-contributing)

---

## 📖 About

Pawspective Client is a desktop application that provides a graphical interface to the [Pawspective REST API](https://github.com/Pawspective/pawspective-backend). It is written in **C++20** with a **Qt 6 / QML** UI stack and follows the MVVM pattern — ViewModels written in C++ expose data and commands to declarative QML views.

---

## 🚀 Quick Start

### Option A — Download a release binary (recommended)

Go to the [Releases page](https://github.com/Pawspective/pawspective-client/releases) and grab the latest build for your platform:

Just download, and run — no build tools required.

---

### Option B — Build from source

#### Prerequisites

- **CMake ≥ 3.21**
- **Qt 6** (Core, Gui, Widgets, Network, Qml, Quick, QuickControls2)
- A C++20-capable compiler (GCC 12+, Clang 15+, MSVC 2022+)
- **Python 3** (for `manage.py`)

Set the `QT_DIR` environment variable to your Qt installation prefix if CMake cannot find Qt automatically:

```bash
export QT_DIR=/path/to/Qt/6.x.x/gcc_64   # Linux
# or set QT_DIR=C:\Qt\6.x.x\msvc2022_64  # Windows
```

#### Build

```bash
# Debug build
python manage.py build debug

# Release build
python manage.py build release
```

#### Run

```bash
python manage.py run debug    # runs build-debug/pawspective-client
python manage.py run release  # runs build-release/pawspective-client(.exe)
```

On Windows, `manage.py run` automatically calls `windeployqt` before launching so all Qt DLLs are in place.

---

## 🏗 Architecture

```
pawspective-client/
├── src/
│   ├── viewmodels/     # MVVM ViewModels — C++ objects exposed to QML
│   ├── services/       # API services (HTTP requests via Qt Network)
│   ├── models/         # DTOs and domain entities
│   ├── state/          # Global application state (auth, session)
│   └── utils/          # Shared helpers (JSON, validation)
│
├── include/            # C++ header files (mirrors src/ structure)
│
├── qml/                # QML views and reusable UI components
│
├── resources/          # Static assets (images, icons)
│
├── tests/              # Qt Test unit tests for services
│
├── CMakeLists.txt      # CMake build definition
├── CMakePresets.json   # Debug / release presets
└── manage.py           # Python-based developer task runner
```

## 🛠 Development

All common tasks are driven by **`manage.py`** — a Python developer tool that wraps CMake and the linting toolchain.

### manage.py commands

| Command | Description |
|---------|-------------|
| `python manage.py build [debug\|release]` | Configure and build (default: `debug`) |
| `python manage.py run [debug\|release]` | Build (if needed) and launch the application |
| `python manage.py test [debug\|release]` | Build and run all unit tests via CTest |
| `python manage.py clean` | Remove all `build-*` directories and caches |
| `python manage.py format` | Auto-format C++ sources with clang-format |
| `python manage.py format-check` | Check formatting without modifying files |
| `python manage.py cppcheck` | Run cppcheck static analysis |
| `python manage.py tidy` | Run clang-tidy analysis |
| `python manage.py lint [steps]` | Run all linters; optionally pass a comma-separated subset, e.g. `format-check,cppcheck` |
| `python manage.py lint-changed [base_ref] [steps]` | Lint only files changed relative to `base_ref` (default: `HEAD~1`) |

Override build defaults without touching `manage.py` by creating `local_config.py` in the project root:

```python
# local_config.py (not committed)
NPROCS = 16
```

### Code style

- **C++20**, formatted with **clang-format** (see `.clang-format`).
- Checked with **clang-tidy** (`.clang-tidy`) and **cppcheck** (`.cppcheck_suppressions`).
- The CI pipeline enforces all linting rules on every pull request.

---

## 🧪 Testing

Unit tests are written with **Qt Test** and built as separate CTest targets. Each service layer module has a corresponding test executable.

```bash
# Build debug and run all tests
python manage.py test debug
```
The CI workflow runs the full test suite on every push and pull request.

---

## 🤝 Contributing

We welcome contributions! Please read [CONTRIBUTING.md](CONTRIBUTING.md) for the full guidelines. The short version:

- Every change starts with a **GitHub Issue**.
- Branch naming: `feature/<issue-id>-short-description` or `fix/<issue-id>-short-description`.
- Each PR references exactly one issue.
- Commits follow **Conventional Commits**: `<type>(<scope>): <description>`.

**Commit types:** `feat`, `fix`, `docs`, `style`, `refactor`, `perf`, `test`, `build`, `ci`, `chore`, `revert`.

**Example:**

```
feat(auth): persist JWT token in OS keychain
fix(animals): correct breed filter mapping
```

Direct pushes to `main` are disabled — all changes go through reviewed pull requests.

---

## 📄 License

Distributed under the **GNU General Public License v3.0**. See [LICENSE](LICENSE) for details.
