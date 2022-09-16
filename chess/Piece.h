#ifndef CHESS_ENGINE_PIECE_H
#define CHESS_ENGINE_PIECE_H

namespace chess
{
    enum Color { White = 0, Black = 8 };

    consteval Color operator~(Color c) noexcept { return Color(c ^ Color::Black); }

    enum Piece
    {
        WPawn = 1,

        WKnight, WRook, WBishop, WQueen, WKing,

        None,

        BPawn = WPawn + Black,

        BKnight, BRook, BBishop, BQueen, BKing
    };

    namespace TemplatePiece
    {
        template<Color C> constexpr Piece Pawn = C == Color::White ? Piece::WPawn : Piece::BPawn;
        template<Color C> constexpr Piece Knight = C == Color::White ? Piece::WKnight : Piece::BKnight;
        template<Color C> constexpr Piece Rook = C == Color::White ? Piece::WRook : Piece::BRook;
        template<Color C> constexpr Piece Bishop = C == Color::White ? Piece::WBishop : Piece::BBishop;
        template<Color C> constexpr Piece Queen = C == Color::White ? Piece::WQueen : Piece::BQueen;
        template<Color C> constexpr Piece King = C == Color::White ? Piece::WKing : Piece::BKing;
    }
} // namespace chess

#endif // CHESS_ENGINE_PIECE_H
