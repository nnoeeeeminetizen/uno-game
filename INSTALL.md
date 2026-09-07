# Installation Guide

## System Requirements

- **C++17 compatible compiler** (GCC 7+, Clang 5+, MSVC 2017+)
- **CMake 3.10+**
- **SQLite3 dev libraries**
- **Git**

---

## Linux (Ubuntu/Debian)

### Step 1: Install Dependencies

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake git libsqlite3-dev
```

### Step 2: Clone Repository

```bash
git clone https://github.com/nnoeeeeminetizen/uno-game.git
cd uno-game
```

### Step 3: Run Setup Script

```bash
chmod +x setup.sh
./setup.sh
```

OR manually:

```bash
mkdir build
cd build
cmake ..
make
```

### Step 4: Run

```bash
# Single player
./uno_single

# Server
./uno_server

# Client (in another terminal)
./uno_client
```

---

## macOS

### Step 1: Install Dependencies

```bash
brew install cmake sqlite3
```

### Step 2: Clone Repository

```bash
git clone https://github.com/nnoeeeeminetizen/uno-game.git
cd uno-game
```

### Step 3: Run Setup Script

```bash
chmod +x setup.sh
./setup.sh
```

OR manually:

```bash
mkdir build
cd build
cmake ..
make
```

### Step 4: Run

```bash
# Single player
./uno_single

# Server
./uno_server

# Client (in another terminal)
./uno_client
```

---

## Windows

### Step 1: Install Dependencies

1. **Visual Studio Community** (with C++ development tools)
   - Download: https://visualstudio.microsoft.com/downloads/
   - Install "Desktop development with C++" workload

2. **CMake**
   - Download: https://cmake.org/download/
   - Add to PATH during installation

3. **SQLite3 Development Libraries**
   - Option A: Download precompiled binaries from https://www.sqlite.org/download.html
   - Option B: Use vcpkg: `vcpkg install sqlite3:x64-windows`

4. **Git**
   - Download: https://git-scm.com/download/win

### Step 2: Clone Repository

```bash
git clone https://github.com/nnoeeeeminetizen/uno-game.git
cd uno-game
```

### Step 3: Run Setup Script

Double-click `setup.bat` or run in Command Prompt:

```cmd
setup.bat
```

OR manually:

```cmd
mkdir build
cd build
cmake -G "Visual Studio 16 2019" ..
cmake --build . --config Release
```

### Step 4: Run

```cmd
REM Single player
Release\uno_single.exe

REM Server
Release\uno_server.exe

REM Client (in another terminal)
Release\uno_client.exe
```

---

## Troubleshooting

### CMake not found
- **Linux**: `sudo apt-get install cmake`
- **macOS**: `brew install cmake`
- **Windows**: Download from cmake.org and add to PATH

### SQLite3 not found
- **Linux**: `sudo apt-get install libsqlite3-dev`
- **macOS**: `brew install sqlite3`
- **Windows**: Download from sqlite.org or use vcpkg

### Compiler errors
- Ensure C++17 support: `g++ --version` should show version 7.0+
- Update compiler: `sudo apt-get install g++-9` (Linux)

### Build directory issues
- Clean build: `rm -rf build && mkdir build && cd build && cmake .. && make`

---

## Verify Installation

After successful build:

```bash
cd build
ls -la | grep uno_  # Should show three executables
ctest --verbose      # Run tests
```

---

## Optional: Generate Documentation

```bash
cd build
make doc  # Requires Doxygen
```

Documentation will be in `build/html/index.html`
