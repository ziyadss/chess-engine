#include <iostream>

#include "chess/Board.h"

int main()
{
    auto fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    auto board = chess::Board(fen);

    std::cout << fen << std::endl << board.fen() << std::endl;

    auto moves = {"a2a4", "c7c5", "b2b4", "c5c4", "d2d4", "c4d3"};
    for (auto move: moves) // Move is a 4 letter string, or 5 for promotions.
    {
        bool legal = board.move(move);
        if (legal)
            std::cout << "Legal move: " << move << ", fen: " << board.fen() << std::endl;
        else
            std::cout << "Illegal move: " << move << ", fen: " << board.fen() << std::endl;
    }
}
