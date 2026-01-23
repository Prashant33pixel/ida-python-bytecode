# Python Bytecode Processor Module for IDA Pro

A comprehensive IDA Pro processor module for disassembling and analyzing Python compiled bytecode (`.pyc`) files. Supports Python versions from 2.7 through 3.14+.

## Features

- **Full Python Version Support**: Handles bytecode from Python 2.7 through 3.14+, including:
  - Python 2.7 (variable-width instructions)
  - Python 3.0-3.5 (variable-width instructions)
  - Python 3.6-3.10 (wordcode format)
  - Python 3.11-3.13 (wordcode with inline caches)
  - Python 3.14+ (renumbered opcodes)

- **Automatic Version Detection**: Identifies Python version from magic numbers in `.pyc` headers

- **Raw Marshal Support**: Can load raw marshal code objects (without `.pyc` header)

- **Rich Disassembly Output**:
  - Resolves constant pool references (strings, numbers, tuples)
  - Resolves global/local variable names
  - Shows comparison and binary operator symbols
  - Displays intrinsic function names (Python 3.12+)

- **Proper Control Flow Analysis**:
  - Handles relative and absolute jumps
  - Supports forward and backward branches
  - Accounts for inline cache entries (Python 3.11+)

- **Nested Code Object Support**: Automatically loads and creates segments for nested functions, classes, and comprehensions

## Requirements

- IDA Pro 8.0+ (64-bit)
- IDA SDK (matching your IDA Pro version)
- CMake 3.16+
- C++17 compatible compiler:
  - macOS: Xcode Command Line Tools / Clang
  - Windows: Visual Studio 2019+
  - Linux: GCC 8+ or Clang 7+

## Building

### macOS / Linux

```bash
# Clone the repository
git clone https://github.com/example/pyc-ida.git
cd pyc-ida

# Configure with CMake
# Option 1: Set IDA_SDK_DIR directly
cmake -B build -DIDA_SDK_DIR=/path/to/idasdk

# Option 2: Use environment variable
export IDASDK=/path/to/idasdk
cmake -B build

# Build
cmake --build build --parallel

# Output files:
#   build/pyc.dylib      - Processor module
#   build/pyc_ldr.dylib  - Loader module
```

### Windows

```powershell
# Configure with CMake
cmake -B build -DIDA_SDK_DIR=C:\path\to\idasdk -G "Visual Studio 17 2022" -A x64

# Build
cmake --build build --config Release

# Output files:
#   build/Release/pyc.dll      - Processor module
#   build/Release/pyc_ldr.dll  - Loader module
```

## Installation

### Automatic Installation

```bash
# Build and install to IDA user directory
cmake --build build --target install_local
```

This copies the modules to:
- macOS/Linux: `~/.idapro/procs/` and `~/.idapro/loaders/`
- Windows: `%APPDATA%\Hex-Rays\IDA Pro\procs\` and `loaders\`

### Manual Installation

Copy the built modules to your IDA Pro installation:

| Module | Destination |
|--------|-------------|
| `pyc.dylib` / `pyc.dll` / `pyc.so` | `<IDA>/procs/` or `~/.idapro/procs/` |
| `pyc_ldr.dylib` / `pyc_ldr.dll` / `pyc_ldr.so` | `<IDA>/loaders/` or `~/.idapro/loaders/` |

## Usage

1. Open a `.pyc` file in IDA Pro
2. The loader will automatically detect the file format and display:
   ```
   Python Compiled Bytecode (Python 3.11)
   ```
3. Click "OK" to load the file
4. IDA will create code segments for each code object found in the file

### Disassembly View

The disassembly shows Python bytecode with resolved operands:

```
.code:00010000                 RESUME          0
.code:00010004                 LOAD_CONST      0                ; 'Hello, World!'
.code:00010006                 STORE_NAME      print
.code:0001000A                 PUSH_NULL
.code:0001000C                 LOAD_NAME       print
.code:00010010                 LOAD_NAME       message
.code:00010014                 CALL            1
.code:0001001A                 POP_TOP
.code:0001001C                 RETURN_CONST    None
```

### Segment Structure

Each code object creates a separate segment:
- `.code` - Module-level code
- `.code_funcname` - Function code
- `.code_ClassName.method` - Method code

## Supported File Formats

| Format | Description |
|--------|-------------|
| `.pyc` | Standard Python compiled bytecode |
| `.pyo` | Optimized bytecode (Python 2.x) |
| Raw marshal | Code objects without header (auto-detected) |

## Python Version Magic Numbers

The loader recognizes magic numbers for all Python versions:

| Version | Magic Range | Header Size | Format |
|---------|-------------|-------------|--------|
| 2.7 | 62171-62211 | 8 bytes | Variable-width |
| 3.0-3.2 | 3131-3160 | 8 bytes | Variable-width |
| 3.3-3.6 | 3190-3379 | 12 bytes | Variable-width / Wordcode |
| 3.7-3.10 | 3390-3439 | 16 bytes | Wordcode |
| 3.11-3.13 | 3450-3570 | 16 bytes | Wordcode + Caches |
| 3.14+ | 3600+ | 16 bytes | Wordcode + Caches |

## Technical Details

### Instruction Encoding

**Pre-wordcode (Python < 3.6)**:
- Instructions without arguments: 1 byte
- Instructions with arguments: 3 bytes (opcode + 2-byte little-endian arg)
- `EXTENDED_ARG` prefixes for arguments > 65535

**Wordcode (Python 3.6+)**:
- All instructions: 2 bytes (opcode + 1-byte arg)
- `EXTENDED_ARG` prefixes for arguments > 255

**Inline Caches (Python 3.11+)**:
- Certain instructions followed by `CACHE` pseudo-instructions
- Cache entries used by the interpreter for optimization
- Sizes vary by instruction (1-9 cache entries)

### Code Object Metadata

The loader stores code object metadata in IDA netnodes:
- Argument counts (positional, keyword-only, positional-only)
- Local variable count, stack size, flags
- First line number
- Constant pool, names, and variable names

### Jump Target Calculation

**Relative jumps (Python 3.10+)**:
```
target = instruction_address + instruction_size + (arg * 2)
```

**Backward jumps (Python 3.11+)**:
```
target = instruction_address + instruction_size - (arg * 2)
```

**Absolute jumps (Python < 3.10)**:
```
target = code_base + arg  // or arg * 2 for wordcode
```

## Limitations

- No decompilation (disassembly only)
- Marshal reference resolution is incomplete (some `<ref>` placeholders)
- No support for PEP 552 hash-based pyc validation display
- Python 1.x not supported

## License

This project is provided as-is for educational and research purposes.

## References

- [Python Developer's Guide - Exploring CPython's Internals](https://devguide.python.org/internals/)
- [PEP 3147 - PYC Repository Directories](https://peps.python.org/pep-3147/)
- [PEP 552 - Deterministic pycs](https://peps.python.org/pep-0552/)
- [dis - Disassembler for Python bytecode](https://docs.python.org/3/library/dis.html)
- [IDA SDK Documentation](https://hex-rays.com/products/ida/support/sdkdoc/)

## Acknowledgments

- Hex-Rays for IDA Pro and the SDK
- Python Software Foundation for CPython source code and documentation
