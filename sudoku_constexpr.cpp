/**
 * sudoku_constexpr.cpp
 *
 * By Sebastian Raaphorst, 2018.
 *
 * A primitive compile-time Sudoku board solver, using backtracking across the empty entries.
 *
 * The reason for this project was to play around with constexpr and demonstrate to myself that it is indeed
 * possible (although via this algorithm, highly inefficient) to solve Sudoku boards at compile-time.
 */

#include <array>
#include <limits>
#include <tuple>
#include <type_traits>

#include <iostream>

namespace math {
    /**
     * For n = t^2 in an integral, unsigned type, find its square root, namely t.
     * If it does not exist, return zero.
     * @tparam T the type of n, which must be integral and unsigned, or we SFINAE.
     * @param n the value t^2 for which we want to find the square root t.
     * @return t if n is of the form t^2 for some t, or 0 otherwise.
     */
    template<typename T>
    constexpr std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>, T>
    isqrt(const T n) {
        // We could improve this bound substantially by using:
        // const T upper = static_cast<T>(std::sqrt(n));
        // but there is no point, because we assume it exists and will be found before that.
        for (T t{}; t < n; ++t)
            if (t*t == n)
                return t;

        // Just return zero if there is no integer square root.
        return T{};
    }
};

/**
 * A general type Sudoku board over an arithmetic type T.
 * It should be initialized with an N * N array representing the contents, with zeroes used to indicate spaces that
 * need to be filled.
 *
 * Note: There is a Sudoku alias below that represents the typical 9 x 9 sudoku board over unsigned integers.
 *
 * @tparam T the type of the board entries
 * @tparam N the dimension of the board
 */
template<typename T, size_t N>
class GenSudoku
{
public:
    using BoardState      = std::array<T, N*N>;

    /// bitsets can't be used in compile-time code.
//    using SectionCoverage = std::bitset<N>;
//    using Candidates      = std::bitset<N>;

    BoardState boardState;

    constexpr unsigned get(size_t x, size_t y) const {
        return x * N + y >= N * N ? 0 : boardState[x * N + y];
    }

    constexpr unsigned operator()(size_t x, size_t y) const {
        return get(x, y);
    }
    constexpr GenSudoku<T,N> put(size_t x, size_t y, T val) {
        boardState[x * N + y] = val;
        return *this;
    }

private:
    /// The contents of a section of the board: either a row, a column, or a quadrant.
    using BoardSection    = std::array<T, N>;

    /// This is the square root of the dimension, i.e. the side length of the quadrants.
    constexpr size_t side() const { return math::isqrt(N); }

    /// Extract a row's contents from the board.
    constexpr const BoardSection row(size_t x) const {
        BoardSection bs{};
        for (auto y = 0; y < N; ++y)
            bs[y] = boardState[x * N + y];
        return bs;
    }

    /// Extract a column's contents from the board.
    constexpr const BoardSection col(size_t y) const {
        BoardSection bs{};
        for (auto x = 0; x < N; ++x)
            bs[x] = boardState[x * N + y];
        return bs;
    }

    /// Extract a quadrant's contents from the board.
    constexpr const BoardSection quadrant(size_t x, size_t y) const {
        BoardSection bs{};
        for (auto i = 0; i < side(); ++i)
            for (auto j = 0; j < side(); ++j)
                bs[i * side() + j] = boardState[side() * (x * N + y) + (i * N + j)];
        return bs;
    }

    constexpr bool rowValid(size_t x) const {
        return sectionValid(row(x));
    }
    constexpr bool colValid(size_t y) const {
        return sectionValid(col(y));
    }
    constexpr bool quadrantValid(size_t x, size_t y) const {
        return sectionValid(quadrant(x, y));
    }

    constexpr bool rowComplete(size_t x) const {
        return sectionComplete(row(x));
    }
    constexpr bool colComplete(size_t y) const {
        return sectionComplete(col(y));
    }
    constexpr bool quadrantComplete(size_t x, size_t y) const {
        return sectionComplete(quadrant(x, y));
    }

    /**
     * This was part of my original attempt to limit the backtracking to only valid entries using bitsets.
     * bitsets, however, are not usable at compile-time as they are not constexpr.
     *
     * TODO: Represent a bitset as an integer type and do a similar calculation.
     * Then see the effect this has on compile-time.
     * I suspect it will be insignificant, as we throw away invalid boards right away anyway.
     */
//    constexpr Candidates coverage(const BoardSection &bs) const {
//        Candidates c;
//        std::for_each(std::cbegin(bs), std::cend(bs), [&c](const auto i) { c[i] = true; });
//        return c;
//    }
//
//    // Determine the candidates, i.e. the bits set to 1, for pos (x,y).
//    constexpr Candidates candidates(size_t x, size_t y) const {
//        return (coverage(row(x)) | coverage(col(y)) | coverage(quadrant(x, y))).flip();
//    }

    /**
     * Determine if a section of the board (could be a row, col, or quadrant) is valid, i.e. does not
     * violate constraints.
     */
    constexpr bool sectionValid(const BoardSection &bs) const {
        std::array<bool, N+1> positions{};
        for (const auto i: bs) {
            if (!i) continue;
            if (positions[i]) return false;
            positions[i] = true;
        }
        return true;
    }

    /**
     * Determine if a section of the board (could be a row, col, or quadrant) is complete, i.e. does not
     * contain any zeroes.
     */
    constexpr bool sectionComplete(const BoardSection &bs) const {
        return std::find(std::cbegin(bs), std::cend(bs), 0) == std::cend(bs);
    }

    /// Return the first empty position, i.e. position with entry 0.
    constexpr std::pair<size_t, size_t> next() const {
        for (size_t x = 0; x < N; ++x)
            for(size_t y = 0; y < N; ++y)
                if (get(x, y) == 0)
                    return {x, y};
        return {std::numeric_limits<size_t>::max(),std::numeric_limits<size_t>::max()};
    }

public:
    /// Determine if the board is valid, i.e. there are no constraints violated.
    constexpr bool isValid() const {
        for (size_t x = 0; x < N; ++x)
            if (!rowValid(x)) return false;
        for (size_t y = 0; y < N; ++y)
            if (!colValid(y)) return false;
        for (size_t x = 0; x < side(); ++x)
            for (size_t y = 0; y < side(); ++y)
                if (!(quadrantValid(x, y)))
                    return false;
        return true;
    }

    /// Determine if the board is complete, i.e. there are no empty spaces.
    constexpr bool isComplete() const {
        for (size_t x = 0; x < N; ++x)
            for (size_t y = 0; y < N; ++y)
                if (get(x,y) == 0)
                    return false;
        return true;
    }

    /// Convenience method to determine if a board is both valid and complete.
    constexpr bool isSolved() const {
        return isValid() && isComplete();
    }


    /**
     * A very primitive Sudoku solver.
     * It simply uses backtracking across the board to try to complete it.
     * This is clearly not an efficient way to solve Sudoku boards as there are many dead ends that could be
     * pruned right away by using a more suitable algorithm, such as Knuth's DLX.
     *
     * An example implementation of DLX used to solve Sudoku boards can be found in my Python DLX GitHub
     * repository:
     *
     * https://github.com/sraaphorst/dlxpy/blob/master/examples/sudoku.py
     *
     * The only reason for the choice of algorithm here is to explore compile-time solving of Sudoku boards.
     * It might be possible to implement a compile-time implementation of a specific DLX instance, but it would
     * be difficult and require some bizarre array manipulation due to the fact that the entire basis of DLX is
     * through sophisticated pointer manipulation.
     *
     * @return a GenSudoku that, if the original board was solvable, should contain the solution.
     */
    constexpr GenSudoku solve() const {
        if (!isValid()) return *this;

        const auto[x, y] = next();
        if (x == std::numeric_limits<size_t>::max()) return *this;


        for (unsigned i = 1; i <= N; ++i) {
            const auto ns = GenSudoku(*this).put(x, y, i);
            const auto res = ns.solve();
            if (res.isComplete() && res.isValid()) return res;
        }

        return *this;
    }
};


template<typename T, size_t N>
std::ostream &operator<<(std::ostream &out, const GenSudoku<T, N> &board) {
    for (auto x = 0; x < N; ++x) {
        for (auto y = 0; y < N; ++y)
            out << board(x, y) << ' ';
        out << '\n';
    }
    return out;
}


using Sudoku = GenSudoku<unsigned int, 9u>;

int main()
{
    /// Expert-level sudoku taken from: http://www.extremesudoku.info/sudoku.html
    constexpr Sudoku sudoku {
            {{5, 0, 0,  9, 0, 0,  8, 0, 0,
              0, 0, 7,  0, 0, 2,  0, 0, 0,
              0, 4, 0,  0, 7, 0,  0, 0, 3,

              9, 0, 0,  1, 0, 0,  0, 7, 0,
              0, 0, 4,  0, 6, 0,  3, 0, 0,
              0, 8, 0,  0, 0, 7,  0, 0, 9,

              1, 0, 0,  0, 4, 0,  0, 9, 0,
              0, 0, 0,  5, 0, 0,  7, 0, 0,
              0, 0, 6,  0, 0, 3,  0, 0, 2,
              }}
    };

    constexpr Sudoku res = sudoku.solve();
    std::cout << res;
    return res.isSolved() ? 0 : 1;
}
