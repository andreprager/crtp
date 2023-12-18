# crtp
Common CRTP classes \
Library still in development, currently only tested with Visual Studio 2022 on Windows 10

### Required libraries:

*   crtp \
    git clone https://github.com/andreprager/crtp
*   build_chain (cmake utilities) \
    git clone https://github.com/andreprager/build_chain
*   googletest [optional] \
    git clone https://github.com/google/googletest
*   benchmark [optional] \
    git clone https://github.com/google/benchmark

### Getting Started

For straight forward build setup it is recommended to clone `crtp` and `build_chain` repository in parallel to each other,
so the default relative paths are working out of the box. \
If not you need to manually set the correct path to build_chain repository in cmake configuration. \

#### Recommended structure:
```
/root
  /build_chain [repo](https://github.com/andreprager/build_chain)
  /crtp [repo](https://github.com/andreprager/crtp)
  /googletest
    /sources [repo](https://github.com/google/googletest)
  /googlebenchmark
    /sources [repo](https://github.com/google/benchmark)
```

### Usage with CMake

Following above recommended structure saves a lot of manual configuration.
Otherwise `cmake` variable must be set if defaults don't do the job.

#### For library only:
```cmake
crtp_master_3rdparty_DIR "path/to/root"
crtp_3rdparty_DIR "path/to/root"
crtp_build_chain_DIR "path/to/root/build_chain/cmake"
```

#### With tests:
```cmake
crtp_master_BUILD_TEST ON
crtp_unit_test_3rdparty_DIR "path/to/root"
crtp_unit_test_build_chain_DIR "path/to/root/build_chain/cmake"
crtp_unit_test_gtest_DIR "path/to/root/googletest/sources"
crtp_mock_3rdparty_DIR "path/to/root"
crtp_mock_build_chain_DIR "path/to/root/build_chain/cmake"
crtp_mock_gtest_DIR "path/to/root/googletest/sources"
```

#### With benchmark:
```cmake
crtp_master_BUILD_BENCHMARK ON
crtp_benchmark_3rdparty_DIR "path/to/root"
crtp_benchmark_build_chain_DIR "path/to/root/build_chain/cmake"
crtp_benchmark_gbenchmark_DIR "path/to/root/googlebenchmark/sources"
```

### Import library
If using CMake, link you target against the project-provided `crtp` target using `target_link_libraries`.

Import the library with
```cmake
add_subdirectory(crtp)
```

Link to the library as follows.
```cmake
target_link_libraries(MyTarget crtp)
```