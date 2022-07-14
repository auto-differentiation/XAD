# XAD

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/9d0cbd845d6948e4ac68de5d74a35b16)](https://app.codacy.com/gh/xcelerit/XAD?utm_source=github.com&utm_medium=referral&utm_content=xcelerit/XAD&utm_campaign=Badge_Grade_Settings)
[![Tests Badge](https://gist.githubusercontent.com/xcelerit-dev/e0eab86863fac6da5e44d20df215f836/raw/badge.svg)](https://github.com/xcelerit/xad/actions?query=workflow%3ACI)
[![Coverage Status](https://coveralls.io/repos/github/xcelerit/XAD/badge.svg?branch=main)](https://coveralls.io/github/xcelerit/XAD?branch=main)

[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/xcelerit/XAD.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/xcelerit/XAD/context:cpp)

XAD is a fast and comprehensive C++ library for automatic differentiation by Xcelerit.
It targets production-quality code at any scale, striving for both ease of use and high performance.

Key features:

- Forward and adjoint mode for any order, using operator-overloading
- Checkpointing support (for tape memory management)
- External functions interface (to integrate external libraries)
- Thread-safe tape
- Formal exception-safety guarantees
- High performance
- Battle-tested in large production code bases

The documentation of the latest version is available [here](https://xad.xcelerit.com).
AAD training, consultancy, and commercial licensing is available from [Xcelerit](https://www.xcelerit.com/adjoint-algorithmic-differentiation/).

Contents:

- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Cloning the Repository](#cloning-the-repository)
  - [Building](#building)
  - [Running the tests](#running-the-tests)
  - [Installing](#installing)
- [Tuning Behaviour and Performance](#tuning-behaviour-and-performance)
- [Building the User Documentation](#building-the-user-documentation)
- [Integrating into Client Code](#integrating-into-client-code)
  - [Approach 1: Submodule + CMake](#approach-1-submodule--cmake)
  - [Approach 2: FetchContent + CMake](#approach-2-fetchcontent--cmake)
  - [Approach 3: Install XAD and Link](#approach-3-install-xad-and-link)
- [Getting Help](#getting-help)
- [Planned Features](#planned-features)
- [Contributing](#contributing)
- [Versioning](#versioning)
- [History](#history)
- [Authors](#authors)
- [License](#license)

## Getting Started

### Prerequisites

- [CMake][cmake], version 3.15 or newer
- Linux: GCC 4.8 or newer, or Clang 4 or newer
- Windows:
  - Visual Studio 2015 or newer
  - Visual Studio with Clang toolset, 2019 or newer
- Git client

### Cloning the Repository

```bash
    git clone https://github.com/xcelerit/xad.git
```

### Building

1. Create a directory for the build artefacts

```bash
cd xad
mkdir build
cd build
```

2. Run cmake to generate the build files

```
cmake ..
```

3. Build using the native build system or with the generic cmake build command

```
cmake --build .
```
### Running the tests

The tests are executed with the `test` target:

```
cmake --build . --target test
```

Alternatively, `ctest` can be used to run them:

```
ctest
```

Or if only the unit tests should be run, the `xad_test` executable in the bin directory
can be executed directly.

### Installing

Run the `install` build target to place the header files, library files, docs, and samples
into the `CMAKE_INSTALL_PREFIX`.

```
cmake --install .
```

## Tuning Behaviour and Performance

A number of options are available via CMake to control the build
and tune the performance.
They can be specified using the CMake command-line with `-DVARIABLE=value`,
or with the CMake GUI.

Influential variables controlling the build are:

| Variable | Description | Default |
|---|---|---|
| `XAD_BUILD_DOCS` | Enable building the XAD user manual as part of the build. This requires Python 3. | `OFF` |
| `XAD_DOCS_ONLY` | Build only the docs and not the code. Implies `XAD_BUILD_DOCS`. | `OFF` |
| `XAD_ENABLE_TESTS` | Enable building tests and samples. | `ON` if main project<br>`OFF` if sub project |
| `XAD_WARNINGS_PARANOID` | Enable a high warning level and flag warnings as errors. | `ON` |
| `XAD_STATIC_MSVC_RUNTIME` | Use the static multi-threaded runtime in Visual C++ (default is dynamic) |
| `XAD_POSITION_INDEPENDENT_CODE` | Generate position-indepent code, i.e. allow linking into a shared library. | `ON` |
| `XAD_ENABLE_ADDRESS_SANITIZER` | Enable address sanitizer (leak detector) - GCC/Clang only. | `OFF` |

Options with an impact on the performance of the tape in adjoint mode (application-specific).
These should not be changed in client code after the tape has been compiled:

| Variable | Description | Default |
|---|---|---|
| `XAD_SIMD_OPTION` | Select between `SSE2`, `AVX`, `AVX2`, and `AVX512` instruction sets. Only enable what the target CPU supports.  | `AVX` |
| `XAD_TAPE_REUSE_SLOTS` | Keep track of unused slots in tape and re-use them (less memory, more compute) | `OFF` |
| `XAD_NO_THREADLOCAL` | Disable thread-local tapes (use with single-threaded code only | `OFF` |

Options that can be set by client code as well, adjusting settings after the
XAD library has already been compiled (in `Config.hpp` or client code compiler definitions):

| Variable | Description | Default |
|---|---|---|
| `XAD_USE_STRONG_INLINE` | Force inlining expression templates, rather than letting the compiler decide. (faster, slow compilation, possible compiler crashes) | `OFF` |
| `XAD_ALLOW_INT_CONVERSION` | Add real -> integer conversion operator, similar to `double`. This may result missing some variable dependency tracking for AAD. | `ON`  |

## Building the User Documentation

The user documentation uses [Sphinx][sphinx].

1. Install the pre-requisites
   * [Python 3](https://www.python.org/) (we recommend Python 3.7 or newer)
   * A LaTeX installation (required for rendering the math formulas):
      - for Windows, we recommend the basic [MiKTeX](https://miktex.org) package,
       and enabling automatic install of missing ones
      - for Linux, we recommend `texlive-full`
2. Enable the `BUILD_DOC` cmake variable and re-run cmake.
   Note that this will create a Python virtual environment within the build directory
   and install the dependencies in [requirements.txt](requirements.txt) into it.
   If any dependencies changed, the CMake variable `XAD_RECONFIGURE_VENV` can be
   set to `TRUE` - this re-runs the dependency installation during the next CMake run.
3. Then build the docs:
```
cmake --build . --target documentation
```
This generates the html user documentation in `doc/out` within the build tree.

For working on the documentation, it is more convenient to work within the
Python environment directly, after the steps above have been performed in CMake once.
Then you can activate the environment on the command line:

* `<buildfolder>\venv\Scripts\activate.bat` (Windows)
* `source <buildfolder>/venv/bin/activate` (Linux)

And run sphinx autobuild (which watches for changes in the source files, re-runs itself, and opens a live-view on http://127.0.0.1:8000/index.html):

```
sphinx-autobuild -d <buildfolder>\doc\_doctrees -c <buildfolder>\doc\_build doc <buildfolder>\doc\html
```
(Replace `<buildfolder>` with the CMake binary directory that you have created above.)



## Integrating into Client Code

In order to use XAD as part of other code, we recommend one of the following approaches.

### Approach 1: Submodule + CMake

If your codebase is using CMake, XAD can be integrated easily into your project
by adding it as a git submodule.

To add the submodule in a subdirectory `extern/xad` (assuming that you are hosting on GitHub):

```
git submodule add ../../xcelerit/xad.git extern/xad
```

If your project is hosted elsewhere, the full github clone URL has to be used instead
of the relative path.

Users then need to clone recursively (`git clone --recursive ...`) or initialise and update
the submodules (`git submodule init && git submodule update`).
More information about submodules can be found in the Git documentation.

To add XAD to the project, all that is needed in one of the `CMakeLists.txt` files is
to add the xad directory, and then link the relevant libraries or executables to `XAD::xad`:

```cmake
add_subdirectory(extern/xad)

add_executable(some_executable ...)
target_link_libraries(some_executable PRIVATE XAD::xad)
```

### Approach 2: FetchContent + CMake

The CMake FetchContent module allows to clone the git repository at configure-time into the
build folder and add it to your project after:

```cmake
include(FetchContent)

FetchContent_Declare(xad
    GIT_REPOSITORY https://github.com/xcelerit/xad.git
    GIT_TAG 1.0.0    # pick a tag, hash, or branch here
)
FetchContent_MakeAvailable(xad)
```

Note that this requires at CMake least 3.14.

### Approach 3: Install XAD and Link

Another approach is to install XAD into a convenient prefix (e.g. `/usr/local/`) first
(instructions above, setting `CMAKE_INSTALL_PREFIX` appropriately).
Note that the package can also be zipped on one machine and downloaded/extracted on another.

**Important:** Since XAD is built as a static library, be careful to use the same compiler and flags for your project as well as XAD itself. Otherwise
the binaries may not be compatible. We therefore recommend to the subproject
approach, building from source within your project.
The library builds very fast.

#### CMake

Then, when you use CMake, you can setup your project to find the XAD dependency in a `CMakeLists.txt` file as:

```cmake
find_package(XAD REQUIRED)
```

If XAD is installed in a standard location, CMake automatically looks for it there and finds it.
Otherwise, the `CMAKE_PREFIX_PATH` variable can be set at configure-time to
add a different directory to its search path:

```bash
cmake /path/to/src -DCMAKE_PREFIX_PATH=/path/to/xad/installprefix
```

#### Other Build Tools

If your project does not use CMake, an installed package can also be linked by adding the following settings:

* Add `/path/to/xad/include` to the compiler's include path
* Enable at least C++ 11 support (`-std=c++11` in GCC)
* Enable threading (requires `-pthread` in GCC for compile and link)
* Add the library path `/path/to/xad/lib` to the linker search paths
* Link `libxad.a` (Release) or `libxad_d.a` (Debug) - or the alternative names
  on Windows


## Getting Help

If you have found an issue, want to report a bug, or have a feature request, please raise a [GitHub issue](https://github.com/xcelerit/XAD/issues).

For general questions about XAD, sharing ideas, engaging with community members, etc, please use [GitHub Discussions](https://github.com/xcelerit/XAD/discussion).


## Planned Features

These features are planned for the near future.
You are very welcome to contribute towards these (or other) features - please contact the project maintainers before.

* *Vector Adjoint Mode* - allow rolling back multiple adjoints at once in the tape,
  e.g. for functions with multiple outputs.
* *Vector Forward Mode* - Calculate multiple forward-mode derivatives at once (for multiple derivatives)
* *Jacobian and Hessian Functions* - higher-level functions to compute full Jacobians or Hessians,
  possibly in a multi-threaded fashion
* *Disable Expression Templates* - allow users to disable expression templates, for debugging purposes


## Contributing

Please read [CONTRIBUTING](CONTRIBUTING.md) for the process of contributing to this project.
Please also obey our [Code of Conduct](CODE_OF_CONDUCT.md) in all communication.

## Versioning

We use [SemVer](http://semver.org/) for versioning,
making a new release available as soon as sufficient new features have been merged into master.
The final decision about a release and which features are included is with the project maintainers.
For the versions available, see the [releases in GitHub](https://github.com/xcelerit/xad/releases).

Every new version is also added to the [Changelog](CHANGELOG.md),
which needs to be maintained throughout the development.
That is, every pull request should also update the Changelog accordingly.

## History

In 2010, Xcelerit starting working on AAD, 
building on early works of Professor Mike Giles and Professor Paul Glasserman 
for efficiently calculating greeks in quantitative finance with AAD.
Xcelerit's closed-source tool QuantAD was first released in 2014,
targeting the quantitative finance industry. 
The tool evolved over the years with more features and better performance.
In July 2022, Xcelerit generalised it so it can be used in other industries and published it as an open-source tool,
rebranded as XAD 1.0.0.

## Authors

* Various contributors from Xcelerit
* See also the list of [contributors](https://github.com/xcelerit/xad/contributors) who participated in this project.

## License

This project is licensed under the GNU Affero General Public License - see the [LICENSE.md](LICENSE.md) file for details.


[cmake]: https://cmake.org
[sphinx]: http://www.sphinx-doc.org
