#include "chess/Board.h"

using chess::Board, chess::Piece, chess::File, chess::Rank;

int main()
{
    auto rookBlockerMask = Board::blockerMask<Piece::WRook>(File::D, Rank::Five);

    auto rookBlockerBoard = std::array<std::array<std::array<Board::bitboard_t, 2048>, 8>, 8>{};

    int bits = __builtin_popcountll(rookBlockerMask);

    for (int i = 0; i < (1 << bits); i++)
        rookBlockerBoard[File::D][Rank::Five][i] = Board::blockerBoard(i, rookBlockerMask);

    auto relevant = rookBlockerBoard[File::D][Rank::Five];

    return 0;
}
