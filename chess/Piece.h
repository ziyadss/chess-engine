#ifndef CHESS_ENGINE_PIECE_H
#define CHESS_ENGINE_PIECE_H

#include <utility>

namespace chess
{
    enum class Color : unsigned char { White = 0, Black = 8 };

    enum class Piece : unsigned char
    {
        WPawn = 1,

        WKnight, WRook, WBishop, WQueen, WKing,

        None,

        BPawn = Piece::WPawn + std::to_underlying(Color::Black),

        BKnight, BRook, BBishop, BQueen, BKing
    };

    namespace Colored
    {
        template<Color C> constexpr Color Opposite = C == Color::White ? Color::Black : Color::White;

        template<Color C> constexpr Piece Pawn = C == Color::White ? Piece::WPawn : Piece::BPawn;
        template<Color C> constexpr Piece Knight = C == Color::White ? Piece::WKnight : Piece::BKnight;
        template<Color C> constexpr Piece Rook = C == Color::White ? Piece::WRook : Piece::BRook;
        template<Color C> constexpr Piece Bishop = C == Color::White ? Piece::WBishop : Piece::BBishop;
        template<Color C> constexpr Piece Queen = C == Color::White ? Piece::WQueen : Piece::BQueen;
        template<Color C> constexpr Piece King = C == Color::White ? Piece::WKing : Piece::BKing;
    }

    template<Color C>
    constexpr Piece charPiece(char c) noexcept
    {
        switch (c)
        {
            case 'p':
                return Colored::Pawn<C>;
            case 'n':
                return Colored::Knight<C>;
            case 'r':
                return Colored::Rook<C>;
            case 'b':
                return Colored::Bishop<C>;
            case 'q':
                return Colored::Queen<C>;
            case 'k':
                return Colored::King<C>;
            default:
                return Piece::None;
        }
    }
} // namespace chess

#endif // CHESS_ENGINE_PIECE_H
