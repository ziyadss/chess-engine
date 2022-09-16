#ifndef CHESS_ENGINE_PIECE_H
#define CHESS_ENGINE_PIECE_H

namespace chess
{
    enum Color { White = 0, Black = 8 };

    constexpr Color operator~(Color c) noexcept { return Color(c ^ Color::Black); }

    enum Piece
    {
        WPawn = 1,

        WKnight, WRook, WBishop, WQueen, WKing,

        None,

        BPawn = WPawn + Black,

        BKnight, BRook, BBishop, BQueen, BKing
    };

    namespace ColoredPiece
    {
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
                return ColoredPiece::Pawn<C>;
            case 'n':
                return ColoredPiece::Knight<C>;
            case 'r':
                return ColoredPiece::Rook<C>;
            case 'b':
                return ColoredPiece::Bishop<C>;
            case 'q':
                return ColoredPiece::Queen<C>;
            case 'k':
                return ColoredPiece::King<C>;
            default:
                return Piece::None;
        }
    }
} // namespace chess

#endif // CHESS_ENGINE_PIECE_H
