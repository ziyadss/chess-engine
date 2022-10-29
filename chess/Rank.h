#ifndef CHESS_ENGINE_RANK_H
#define CHESS_ENGINE_RANK_H

namespace chess
{
    enum class Rank : unsigned char
    {
        One, Two, Three, Four, Five, Six, Seven, Eight
    };

    constexpr Rank charRank(char c) noexcept { return Rank(c - '1'); }
} // namespace chess

#endif // CHESS_ENGINE_RANK_H
