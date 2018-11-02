# sudoku_constexpr

**Status:** Complete, verson 1.0.0.

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

## Performance

Using what is considered an expert-level Sudoku board (as hard-coded in the implementation), on my MacBook, compilation takes approximately 3.5 minutes.

Using the well-known, extreme Sudoku from here:

https://www.telegraph.co.uk/news/science/science-news/9359579/Worlds-hardest-sudoku-can-you-crack-it.html

Compilation takes slightly over 20 minutes on my MacBook.

## Compilation

This may not run under GCC due to GCC complaining that the call to solve is not actually `constexpr`. I suggest using clang instead.
