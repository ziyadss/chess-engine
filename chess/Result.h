#ifndef CHESS_ENGINE_RESULT_H
#define CHESS_ENGINE_RESULT_H

namespace chess
{
    enum class Result : unsigned char
    {
        LegalMove, IllegalMove, WhiteWin, BlackWin, Draw
    };
} // namespace chess

#endif // CHESS_ENGINE_RESULT_H
