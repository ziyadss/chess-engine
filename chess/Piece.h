#ifndef CHESS_ENGINE_PIECE_H
#define CHESS_ENGINE_PIECE_H

namespace chess
{
    enum Color { White = 0, Black = 8 };

    enum Piece
    {
        WPawn = 1,

        WKnight, WRook, WBishop, WQueen, WKing,

        None,

        BPawn = WPawn + Black,

        BKnight, BRook, BBishop, BQueen, BKing
    };
} // namespace chess

#endif // CHESS_ENGINE_PIECE_H
