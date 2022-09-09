#include <iostream>

#include "chess/Board.h"

std::string squareName(chess::File f, chess::Rank r)
{
    char first = 'A' + f;
    char second = '1' + r;
    return std::string{first, second};
}

int main()
{
    auto fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    auto board = chess::Board(fen);

    constexpr const std::array<chess::File, 8> files = {chess::File::A, chess::File::B, chess::File::C, chess::File::D, chess::File::E,
                                                        chess::File::F, chess::File::G, chess::File::H};
    constexpr const std::array<chess::Rank, 8> ranks = {chess::Rank::One, chess::Rank::Two, chess::Rank::Three, chess::Rank::Four,
                                                        chess::Rank::Five, chess::Rank::Six, chess::Rank::Seven, chess::Rank::Eight};
    for (auto rank: ranks)
    {
        for (auto file: files)
        {
            auto moves = board.moves(file, rank);
            std::cout << "Moves for " << squareName(file, rank) << ": " << moves << std::endl;
        }
    }

    std::cout << fen << std::endl << board.fen() << std::endl;
}
