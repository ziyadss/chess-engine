#include <iostream>

#include "chess/Board.h"

int main()
{
    auto fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    auto board = chess::Board(fen);

    std::cout << fen << std::endl << board.fen() << std::endl;
}
