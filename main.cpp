#include <iostream>

#include "chess/Board.h"

int main()
{
    auto fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    auto board = chess::Board(fen);

    std::cout << fen << std::endl << board.fen() << std::endl;

    // {"b2b4", "a7a5", "b4a5", "a8a6", "a2a3", "a6h6", "a5a6", "h6g6", "a6a7", "g6h6", "a7a8k"};
    auto moves = {"b2b4", "a7a5", "b4a5", "a8a6", "a2a3", "a6h6", "a5a6", "h6g6"};
    for (auto move: moves) // Move is a 4 letter string, or 5 for promotions.
    {
        bool legal = board.move(move);
        if (legal)
            std::cout << "Legal move: " << move << ", fen: " << board.fen() << std::endl;
        else
            std::cout << "Illegal move: " << move << ", fen: " << board.fen() << std::endl;
    }
}
