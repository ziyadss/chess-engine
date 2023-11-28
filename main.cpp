#include <iostream>

#include "chess/Board.hpp"

int main()
{
    auto fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    auto board = chess::Board(fen);

    std::cout << fen << '\n' << board.fen() << '\n';

    chess::Result moveResult;
    while (!GameOver(moveResult))
    {
        std::cout << board.fen() << '\n' << board.display() << "\nEnter move: ";

        std::string move;
        std::cin >> move;

        moveResult = board.move(move);
        if (moveResult == chess::Result::IllegalMove)
            std::cout << "Invalid move" << '\n';

        std::cout << "moveResult = " << std::to_underlying(moveResult) << '\n' << '\n';
    }

    auto moves = {"a2a4", "a7a5", "b2b4", "b7b5", "b1c3", "b8c6", "c1a3", "c8a6", "d2d4", "d7d5", "d1d3", "d8d6", "e1c1"};
    for (auto move: moves) // Move is a 4 letter string, or 5 for promotions.
    {
        bool legal = board.move(move) != chess::Result::IllegalMove;
        if (legal)
            std::cout << "Legal move: " << move << ", fen: " << board.fen() << '\n';
        else
            std::cout << "Illegal move: " << move << ", fen: " << board.fen() << '\n';
    }
}
