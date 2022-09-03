#include <iostream>

#include "chess/Board.h"

int main()
{
    auto board = chess::Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    std::cout << board.fen();
}
