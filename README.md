# cpplogo
This library provides a C++ implementation of algorithms derived from the Simultaneous Optimistic Optimization (SOO) algorithm, such as LOGO. Its focus is on correctness and ease of understanding/extension rather than speed.

## Implemented Algorithms
_Algorithm_ | _Reference_ | _Status_
------------|-------------|---------
SOO | [From Bandits to Monte-Carlo Tree Search: The Optimistic Principle Applied to Optimization and Planning](https://hal.archives-ouvertes.fr/hal-00747575) | Complete
LOGO | [Global Continuous Optimization with Error Bound and Fast Convergence](https://arxiv.org/abs/1607.04817) | Complete
BaMSOO | [Bayesian Multi-Scale Optimistic Optimization](http://jmlr.org/proceedings/papers/v33/wang14d.pdf) | Implemented on ```dev``` branch

## Build Instructions
This library was developed using gcc 5.2.0, CMake 3.5.2, and Boost 1.61.0 on CentOS 7.2.1511. Earlier versions of those tools would likely still work provided that they support the C++14 standard and the Boost.Log library.

To build the cpplogo library on its own:
```
$ cd cpplogo
$ mkdir build
$ cd build
$ cmake ..
$ make
```

Currently, the library is packaged with one example program that tests the performance of SOO and LOGO on the Rosenbrock function. To enable the compilation of this program, replace the `cmake` call above with:
```
$ cmake .. -DCPPLOGO_BUILD_EXAMPLES=ON -DCPPLOGO_ENABLE_LOGGING=ON
```

For the time being, this produces a `test_cpplogo` executable alongside the library itself.
