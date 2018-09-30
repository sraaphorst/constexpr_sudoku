# constexpr_sudoku

This is a C++17 constexpr implementation of a Sudoku board solver, i.e. it solves Sudoku boards
at compile-time.

(It could be easily modified to run under C++14 if one eliminated the tuple binding.)

This uses a primitive backtracking approach, and is thus not meant for serious use,
but just as an exercise in exploring compile-time implementations of algorithms.

Efficient Sudoku solvers rely on more advanced techniques than naive backtracking, with one of
the most common techniques using Donald Knuth's "algorithm X" (generally called DLX or Dancing
Links).

I have an implementation of DLX and a Sudoku solver using DLX coded in C and Python.

The Python implementation can be found here on GitHub:

https://github.com/sraaphorst/dlxpy/blob/master/examples/sudoku.py