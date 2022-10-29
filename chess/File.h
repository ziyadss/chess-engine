#ifndef CHESS_ENGINE_FILE_H
#define CHESS_ENGINE_FILE_H

namespace chess
{
    enum class File : unsigned char
    {
        A, B, C, D, E, F, G, H
    };

    constexpr File charFile(char c) noexcept { return File(c - 'a'); }
} // namespace chess

#endif // CHESS_ENGINE_FILE_H
