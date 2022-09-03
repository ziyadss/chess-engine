#ifndef CHESS_ENGINE_BOARD_H
#define CHESS_ENGINE_BOARD_H

#include <array>
#include <sstream>

#include "Piece.h"
#include "File.h"
#include "Rank.h"

namespace chess
{
    class Board
    {
    public:
        using bitboard_t = uint64_t;
        static_assert(sizeof(bitboard_t) == 8, "bitboard_t must be 64 bits");
        consteval Board() noexcept: m_bitboards(s_startingPosition) {}
        constexpr explicit Board(const std::string &fen) { set(fen); }

        [[nodiscard]] consteval static auto pieces() noexcept { return s_piecesList; }
        [[nodiscard]] consteval static auto files() noexcept { return s_filesList; }
        [[nodiscard]] consteval static auto filesReversed() noexcept { return s_filesListReversed; }
        [[nodiscard]] consteval static auto ranks() noexcept { return s_ranksList; }
        [[nodiscard]] consteval static auto ranksReversed() noexcept { return s_ranksListReversed; }
        [[nodiscard]] consteval static auto starting() noexcept { return s_starting; }

        [[nodiscard]] consteval static bitboard_t file(File f) noexcept { return s_files[f]; }
        [[nodiscard]] consteval static bitboard_t rank(Rank r) noexcept { return s_ranks[r]; }
        [[nodiscard]] consteval static bitboard_t diagonal(File f, Rank r) noexcept { return s_diagonals[f][r]; }
        [[nodiscard]] consteval static bitboard_t antidiagonal(File f, Rank r) noexcept { return s_antidiagonals[f][r]; }
        [[nodiscard]] constexpr static bitboard_t square(File f, Rank r) noexcept { return s_squares[f][r]; }
        [[nodiscard]] constexpr static bitboard_t square(int index) noexcept { return s_squares[7 - (index & 7)][index >> 3]; }

        void set(const std::string &fen)
        {
            m_bitboards = std::array<bitboard_t, 14>{};
            int sqr = 63;
            size_t fenIdx = 0;
            while (fen[fenIdx] != ' ')
            {
                switch (fen[fenIdx++])
                {
                    case 'r':
                        m_bitboards[Piece::BRook] |= square(sqr--);
                        break;
                    case 'n':
                        m_bitboards[Piece::BKnight] |= square(sqr--);
                        break;
                    case 'b':
                        m_bitboards[Piece::BBishop] |= square(sqr--);
                        break;
                    case 'q':
                        m_bitboards[Piece::BQueen] |= square(sqr--);
                        break;
                    case 'k':
                        m_bitboards[Piece::BKing] |= square(sqr--);
                        break;
                    case 'p':
                        m_bitboards[Piece::BPawn] |= square(sqr--);
                        break;
                    case 'R':
                        m_bitboards[Piece::WRook] |= square(sqr--);
                        break;
                    case 'N':
                        m_bitboards[Piece::WKnight] |= square(sqr--);
                        break;
                    case 'B':
                        m_bitboards[Piece::WBishop] |= square(sqr--);
                        break;
                    case 'Q':
                        m_bitboards[Piece::WQueen] |= square(sqr--);
                        break;
                    case 'K':
                        m_bitboards[Piece::WKing] |= square(sqr--);
                        break;
                    case 'P':
                        m_bitboards[Piece::WPawn] |= square(sqr--);
                        break;
                    case '/':
                        break;
                    case '1':
                        sqr -= 1;
                        break;
                    case '2':
                        sqr -= 2;
                        break;
                    case '3':
                        sqr -= 3;
                        break;
                    case '4':
                        sqr -= 4;
                        break;
                    case '5':
                        sqr -= 5;
                        break;
                    case '6':
                        sqr -= 6;
                        break;
                    case '7':
                        sqr -= 7;
                        break;
                    case '8':
                        sqr -= 8;
                        break;
                }
            }

            m_bitboards[Piece::White] =
                    m_bitboards[Piece::WPawn] | m_bitboards[Piece::WRook] | m_bitboards[Piece::WKnight] | m_bitboards[Piece::WBishop] |
                    m_bitboards[Piece::WQueen] | m_bitboards[Piece::WKing];

            m_bitboards[Piece::Black] =
                    m_bitboards[Piece::BPawn] | m_bitboards[Piece::BRook] | m_bitboards[Piece::BKnight] | m_bitboards[Piece::BBishop] |
                    m_bitboards[Piece::BQueen] | m_bitboards[Piece::BKing];

            m_turn = fen[++fenIdx] == 'w' ? Piece::White : Piece::Black;

            // TODO: Castling rights
            // TODO: En passant square
            // TODO: Half-move clock
            // TODO: Full-move number
        }
        [[nodiscard]] std::string fen()
        {
            auto ss = std::ostringstream();
            int emptyCount = 0;
            for (auto sqr = 63; sqr >= 0; sqr--)
            {
                auto p = piece(sqr);

                bool empty = p == Piece::None;
                emptyCount += empty;

                bool endRank = sqr % 8 == 0;
                bool printEmpty = ((!empty || endRank) && emptyCount > 0);

                if (printEmpty)
                {
                    ss << emptyCount;
                    emptyCount = 0;
                }

                if (!empty)
                {
                    ss << s_pieceChars[p];
                }

                if (endRank && sqr != 0)
                {
                    ss << '/';
                }

            }
            ss << ' ';
            ss << s_pieceChars[m_turn];

            // TODO: Castling rights
            // TODO: En passant square
            // TODO: Half-move clock
            // TODO: Full-move number
            return ss.str();
        }

        static std::string draw(bitboard_t bits)
        {
            auto ss = std::ostringstream();
            ss << std::hex << std::showbase << bits << std::endl;
            for (int i = 7; i >= 0; i--)
            {
                for (int j = 7; j >= 0; j--)
                    ss << ((bits & (1ULL << (i * 8 + j))) ? 'X' : '.');
                ss << '\n';
            }
            ss << std::endl;
            return ss.str();
        }
        [[nodiscard]] std::string string()
        {
            auto ss = std::ostringstream();
            ss << "  A B C D E F G H\n";
            for (auto rank: ranksReversed())
            {
                ss << rank + 1 << " ";
                for (auto file: files())
                    ss << s_pieceChars[piece(file, rank)] << " ";
                ss << rank + 1 << "\n";
            }
            ss << "  A B C D E F G H" << std::endl;
            return ss.str();
        }

        template<Piece P>
        [[nodiscard]] bitboard_t consteval static blockerMask(File f, Rank r) noexcept
        {
            constexpr const bitboard_t borders = file(File::A) | file(File::H) | rank(Rank::One) | rank(Rank::Eight);

            if constexpr (P == Piece::WRook || P == Piece::BRook)
                return s_rookMoves[f][r] & ~borders;
            else if constexpr (P == Piece::WBishop || P == Piece::BBishop)
                return s_bishopMoves[f][r] & ~borders;
            else if constexpr (P == Piece::WQueen || P == Piece::BQueen)
                return s_queenMoves[f][r] & ~borders;
        }

        template<Piece P>
        [[nodiscard]] bitboard_t consteval static moveBoard(bitboard_t square, bitboard_t blockerBoard)
        {
            // TODO
            return 0;
        }

        [[nodiscard]] constexpr static bitboard_t blockerBoard(int index, bitboard_t blockerMask)
        {
            // TODO
            return 0;
        }

        template<Piece P>
        [[nodiscard]] constexpr bitboard_t get() const noexcept { return m_bitboards[P]; }

        [[nodiscard]] Piece turn() const noexcept { return m_turn; }

    private:
        constexpr static const auto s_starting = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        constexpr static const std::array<Piece, 12> s_piecesList{Piece::WPawn, Piece::WRook, Piece::WKnight, Piece::WBishop, Piece::WQueen,
                                                                  Piece::WKing, Piece::BPawn, Piece::BRook, Piece::BKnight, Piece::BBishop,
                                                                  Piece::BQueen, Piece::BKing};

        constexpr const static std::array<char, 15> s_pieceChars = {'P', 'R', 'N', 'B', 'Q', 'K', 'p', 'r', 'n', 'b', 'q', 'k', 'w', 'b',
                                                                    '.'};

        constexpr static const std::array<File, 8> s_filesList{File::A, File::B, File::C, File::D, File::E, File::F, File::G, File::H};
        constexpr static const std::array<File, 8> s_filesListReversed{File::H, File::G, File::F, File::E, File::D, File::C, File::B,
                                                                       File::A};

        constexpr static const std::array<Rank, 8> s_ranksList{Rank::One, Rank::Two, Rank::Three, Rank::Four, Rank::Five, Rank::Six,
                                                               Rank::Seven, Rank::Eight};
        constexpr static const std::array<Rank, 8> s_ranksListReversed{Rank::Eight, Rank::Seven, Rank::Six, Rank::Five, Rank::Four,
                                                                       Rank::Three, Rank::Two, Rank::One};

        constexpr static const std::array<bitboard_t, 8> s_files{0x0101010101010101, 0x0202020202020202, 0x0404040404040404,
                                                                 0x0808080808080808, 0x1010101010101010, 0x2020202020202020,
                                                                 0x4040404040404040, 0x8080808080808080};

        constexpr static const std::array<bitboard_t, 8> s_ranks{0x00000000000000FF, 0x000000000000FF00, 0x0000000000FF0000,
                                                                 0x00000000FF000000, 0x000000FF00000000, 0x0000FF0000000000,
                                                                 0x00FF000000000000, 0xFF00000000000000};

        constexpr static const std::array<const std::array<bitboard_t, 8>, 8> s_diagonals{
                {{0x0000000000000001, 0x0000000000000102, 0x0000000000010204, 0x0000000001020408, 0x0000000102040810, 0x0000010204081020,
                  0x0001020408102040, 0x0102040810204080},
                 {0x0000000000000102, 0x0000000000010204, 0x0000000001020408, 0x0000000102040810, 0x0000010204081020, 0x0001020408102040,
                  0x0102040810204080, 0x0204081020408000},
                 {0x0000000000010204, 0x0000000001020408, 0x0000000102040810, 0x0000010204081020, 0x0001020408102040, 0x0102040810204080,
                  0x0204081020408000, 0x0408102040800000},
                 {0x0000000001020408, 0x0000000102040810, 0x0000010204081020, 0x0001020408102040, 0x0102040810204080, 0x0204081020408000,
                  0x0408102040800000, 0x0810204080000000},
                 {0x0000000102040810, 0x0000010204081020, 0x0001020408102040, 0x0102040810204080, 0x0204081020408000, 0x0408102040800000,
                  0x0810204080000000, 0x1020408000000000},
                 {0x0000010204081020, 0x0001020408102040, 0x0102040810204080, 0x0204081020408000, 0x0408102040800000, 0x0810204080000000,
                  0x1020408000000000, 0x2040800000000000},
                 {0x0001020408102040, 0x0102040810204080, 0x0204081020408000, 0x0408102040800000, 0x0810204080000000, 0x1020408000000000,
                  0x2040800000000000, 0x4080000000000000},
                 {0x0102040810204080, 0x0204081020408000, 0x0408102040800000, 0x0810204080000000, 0x1020408000000000, 0x2040800000000000,
                  0x4080000000000000, 0x8000000000000000}}};

        constexpr static const std::array<const std::array<bitboard_t, 8>, 8> s_antidiagonals{
                {{0x8040201008040201, 0x4020100804020100, 0x2010080402010000, 0x1008040201000000, 0x0804020100000000, 0x0402010000000000,
                  0x0201000000000000, 0x0100000000000000},
                 {0x0080402010080402, 0x8040201008040201, 0x4020100804020100, 0x2010080402010000, 0x1008040201000000, 0x0804020100000000,
                  0x0402010000000000, 0x0201000000000000},
                 {0x0000804020100804, 0x0080402010080402, 0x8040201008040201, 0x4020100804020100, 0x2010080402010000, 0x1008040201000000,
                  0x0804020100000000, 0x0402010000000000},
                 {0x0000008040201008, 0x0000804020100804, 0x0080402010080402, 0x8040201008040201, 0x4020100804020100, 0x2010080402010000,
                  0x1008040201000000, 0x0804020100000000},
                 {0x0000000080402010, 0x0000008040201008, 0x0000804020100804, 0x0080402010080402, 0x8040201008040201, 0x4020100804020100,
                  0x2010080402010000, 0x1008040201000000},
                 {0x0000000000804020, 0x0000000080402010, 0x0000008040201008, 0x0000804020100804, 0x0080402010080402, 0x8040201008040201,
                  0x4020100804020100, 0x2010080402010000},
                 {0x0000000000008040, 0x0000000000804020, 0x0000000080402010, 0x0000008040201008, 0x0000804020100804, 0x0080402010080402,
                  0x8040201008040201, 0x4020100804020100},
                 {0x0000000000000080, 0x0000000000008040, 0x0000000000804020, 0x0000000080402010, 0x0000008040201008, 0x0000804020100804,
                  0x0080402010080402, 0x8040201008040201}}};

        constexpr static const std::array<const std::array<bitboard_t, 8>, 8> s_squares{
                {{0x0000000000000001, 0x0000000000000100, 0x0000000000010000, 0x0000000001000000, 0x0000000100000000, 0x0000010000000000,
                  0x0001000000000000, 0x0100000000000000},
                 {0x0000000000000002, 0x0000000000000200, 0x0000000000020000, 0x0000000002000000, 0x0000000200000000, 0x0000020000000000,
                  0x0002000000000000, 0x0200000000000000},
                 {0x0000000000000004, 0x0000000000000400, 0x0000000000040000, 0x0000000004000000, 0x0000000400000000, 0x0000040000000000,
                  0x0004000000000000, 0x0400000000000000},
                 {0x0000000000000008, 0x0000000000000800, 0x0000000000080000, 0x0000000008000000, 0x0000000800000000, 0x0000080000000000,
                  0x0008000000000000, 0x0800000000000000},
                 {0x0000000000000010, 0x0000000000001000, 0x0000000000100000, 0x0000000010000000, 0x0000001000000000, 0x0000100000000000,
                  0x0010000000000000, 0x1000000000000000},
                 {0x0000000000000020, 0x0000000000002000, 0x0000000000200000, 0x0000000020000000, 0x0000002000000000, 0x0000200000000000,
                  0x0020000000000000, 0x2000000000000000},
                 {0x0000000000000040, 0x0000000000004000, 0x0000000000400000, 0x0000000040000000, 0x0000004000000000, 0x0000400000000000,
                  0x0040000000000000, 0x4000000000000000},
                 {0x0000000000000080, 0x0000000000008000, 0x0000000000800000, 0x0000000080000000, 0x0000008000000000, 0x0000800000000000,
                  0x0080000000000000, 0x8000000000000000}}};

        constexpr static const std::array<bitboard_t, 14> s_startingPosition{0x000000000000FF00, 0x0000000000000081, 0x0000000000000042,
                                                                             0x0000000000000024, 0x0000000000000008, 0x0000000000000016,
                                                                             0x00FF000000000000, 0x8100000000000000, 0x4200000000000000,
                                                                             0x2400000000000000, 0x0800000000000000, 0x1600000000000000,
                                                                             0xFFFF000000000000, 0x000000000000FFFF};

        constexpr static const std::array<const std::array<bitboard_t, 8>, 8> s_wPawnMoves{
                {{0x0000000000000100, 0x0000000000010000, 0x0000000001000000, 0x0000000100000000, 0x0000010000000000, 0x0001000000000000,
                  0x0100000000000000, 0x0000000000000000},
                 {0x0000000000000200, 0x0000000000020000, 0x0000000002000000, 0x0000000200000000, 0x0000020000000000, 0x0002000000000000,
                  0x0200000000000000, 0x0000000000000000},
                 {0x0000000000000400, 0x0000000000040000, 0x0000000004000000, 0x0000000400000000, 0x0000040000000000, 0x0004000000000000,
                  0x0400000000000000, 0x0000000000000000},
                 {0x0000000000000800, 0x0000000000080000, 0x0000000008000000, 0x0000000800000000, 0x0000080000000000, 0x0008000000000000,
                  0x0800000000000000, 0x0000000000000000},
                 {0x0000000000001000, 0x0000000000100000, 0x0000000010000000, 0x0000001000000000, 0x0000100000000000, 0x0010000000000000,
                  0x1000000000000000, 0x0000000000000000},
                 {0x0000000000002000, 0x0000000000200000, 0x0000000020000000, 0x0000002000000000, 0x0000200000000000, 0x0020000000000000,
                  0x2000000000000000, 0x0000000000000000},
                 {0x0000000000004000, 0x0000000000400000, 0x0000000040000000, 0x0000004000000000, 0x0000400000000000, 0x0040000000000000,
                  0x4000000000000000, 0x0000000000000000},
                 {0x0000000000008000, 0x0000000000800000, 0x0000000080000000, 0x0000008000000000, 0x0000800000000000, 0x0080000000000000,
                  0x8000000000000000, 0x0000000000000000}}};

        constexpr static const std::array<const std::array<bitboard_t, 8>, 8> s_bPawnMoves{
                {{0x0000000000000000, 0x0000000000000001, 0x0000000000000100, 0x0000000000010000, 0x0000000001000000, 0x0000000100000000,
                  0x0000010000000000, 0x0001000000000000},
                 {0x0000000000000000, 0x0000000000000002, 0x0000000000000200, 0x0000000000020000, 0x0000000002000000, 0x0000000200000000,
                  0x0000020000000000, 0x0002000000000000},
                 {0x0000000000000000, 0x0000000000000004, 0x0000000000000400, 0x0000000000040000, 0x0000000004000000, 0x0000000400000000,
                  0x0000040000000000, 0x0004000000000000},
                 {0x0000000000000000, 0x0000000000000008, 0x0000000000000800, 0x0000000000080000, 0x0000000008000000, 0x0000000800000000,
                  0x0000080000000000, 0x0008000000000000},
                 {0x0000000000000000, 0x0000000000000010, 0x0000000000001000, 0x0000000000100000, 0x0000000010000000, 0x0000001000000000,
                  0x0000100000000000, 0x0010000000000000},
                 {0x0000000000000000, 0x0000000000000020, 0x0000000000002000, 0x0000000000200000, 0x0000000020000000, 0x0000002000000000,
                  0x0000200000000000, 0x0020000000000000},
                 {0x0000000000000000, 0x0000000000000040, 0x0000000000004000, 0x0000000000400000, 0x0000000040000000, 0x0000004000000000,
                  0x0000400000000000, 0x0040000000000000},
                 {0x0000000000000000, 0x0000000000000080, 0x0000000000008000, 0x0000000000800000, 0x0000000080000000, 0x0000008000000000,
                  0x0000800000000000, 0x0080000000000000}}};

        constexpr static const std::array<const std::array<bitboard_t, 8>, 8> s_wPawnAttacks{
                {{0x0000000000000200, 0x0000000000020000, 0x0000000002000000, 0x0000000200000000, 0x0000020000000000, 0x0002000000000000,
                  0x0200000000000000, 0x0000000000000000},
                 {0x0000000000000500, 0x0000000000050000, 0x0000000005000000, 0x0000000500000000, 0x0000050000000000, 0x0005000000000000,
                  0x0500000000000000, 0x0000000000000000},
                 {0x0000000000000A00, 0x00000000000A0000, 0x000000000A000000, 0x0000000A00000000, 0x00000A0000000000, 0x000A000000000000,
                  0x0A00000000000000, 0x0000000000000000},
                 {0x0000000000001400, 0x0000000000140000, 0x0000000014000000, 0x0000001400000000, 0x0000140000000000, 0x0014000000000000,
                  0x1400000000000000, 0x0000000000000000},
                 {0x0000000000002800, 0x0000000000280000, 0x0000000028000000, 0x0000002800000000, 0x0000280000000000, 0x0028000000000000,
                  0x2800000000000000, 0x0000000000000000},
                 {0x0000000000005000, 0x0000000000500000, 0x0000000050000000, 0x0000005000000000, 0x0000500000000000, 0x0050000000000000,
                  0x5000000000000000, 0x0000000000000000},
                 {0x000000000000A000, 0x0000000000A00000, 0x00000000A0000000, 0x000000A000000000, 0x0000A00000000000, 0x00A0000000000000,
                  0xA000000000000000, 0x0000000000000000},
                 {0x0000000000004000, 0x0000000000400000, 0x0000000040000000, 0x0000004000000000, 0x0000400000000000, 0x0040000000000000,
                  0x4000000000000000, 0x0000000000000000}}};

        constexpr static const std::array<const std::array<bitboard_t, 8>, 8> s_bPawnAttacks{
                {{0x0000000000000000, 0x0000000000000002, 0x0000000000000200, 0x0000000000020000, 0x0000000002000000, 0x0000000200000000,
                  0x0000020000000000, 0x0002000000000000},
                 {0x0000000000000000, 0x0000000000000005, 0x0000000000000500, 0x0000000000050000, 0x0000000005000000, 0x0000000500000000,
                  0x0000050000000000, 0x0005000000000000},
                 {0x0000000000000000, 0x000000000000000A, 0x0000000000000A00, 0x00000000000A0000, 0x000000000A000000, 0x0000000A00000000,
                  0x00000A0000000000, 0x000A000000000000},
                 {0x0000000000000000, 0x0000000000000014, 0x0000000000001400, 0x0000000000140000, 0x0000000014000000, 0x0000001400000000,
                  0x0000140000000000, 0x0014000000000000},
                 {0x0000000000000000, 0x0000000000000028, 0x0000000000002800, 0x0000000000280000, 0x0000000028000000, 0x0000002800000000,
                  0x0000280000000000, 0x0028000000000000},
                 {0x0000000000000000, 0x0000000000000050, 0x0000000000005000, 0x0000000000500000, 0x0000000050000000, 0x0000005000000000,
                  0x0000500000000000, 0x0050000000000000},
                 {0x0000000000000000, 0x00000000000000A0, 0x000000000000A000, 0x0000000000A00000, 0x00000000A0000000, 0x000000A000000000,
                  0x0000A00000000000, 0x00A0000000000000},
                 {0x0000000000000000, 0x0000000000000040, 0x0000000000004000, 0x0000000000400000, 0x0000000040000000, 0x0000004000000000,
                  0x0000400000000000, 0x0040000000000000}}};

        constexpr static const std::array<const std::array<bitboard_t, 8>, 8> s_rookMoves{
                {{0x01010101010101FE, 0x010101010101FE01, 0x0101010101FE0101, 0x01010101FE010101, 0x010101FE01010101, 0x0101FE0101010101,
                  0x01FE010101010101, 0xFE01010101010101},
                 {0x02020202020202FD, 0x020202020202FD02, 0x0202020202FD0202, 0x02020202FD020202, 0x020202FD02020202, 0x0202FD0202020202,
                  0x02FD020202020202, 0xFD02020202020202},
                 {0x04040404040404FB, 0x040404040404FB04, 0x0404040404FB0404, 0x04040404FB040404, 0x040404FB04040404, 0x0404FB0404040404,
                  0x04FB040404040404, 0xFB04040404040404},
                 {0x08080808080808F7, 0x080808080808F708, 0x0808080808F70808, 0x08080808F7080808, 0x080808F708080808, 0x0808F70808080808,
                  0x08F7080808080808, 0xF708080808080808},
                 {0x10101010101010EF, 0x101010101010EF10, 0x1010101010EF1010, 0x10101010EF101010, 0x101010EF10101010, 0x1010EF1010101010,
                  0x10EF101010101010, 0xEF10101010101010},
                 {0x20202020202020DF, 0x202020202020DF20, 0x2020202020DF2020, 0x20202020DF202020, 0x202020DF20202020, 0x2020DF2020202020,
                  0x20DF202020202020, 0xDF20202020202020},
                 {0x40404040404040BF, 0x404040404040BF40, 0x4040404040BF4040, 0x40404040BF404040, 0x404040BF40404040, 0x4040BF4040404040,
                  0x40BF404040404040, 0xBF40404040404040},
                 {0x808080808080807F, 0x8080808080807F80, 0x80808080807F8080, 0x808080807F808080, 0x8080807F80808080, 0x80807F8080808080,
                  0x807F808080808080, 0x7F80808080808080}}};

        constexpr static const std::array<const std::array<bitboard_t, 8>, 8> s_knightMoves{
                {{0x0000000000020400, 0x0000000002040004, 0x0000000204000402, 0x0000020400040200, 0x0002040004020000, 0x0204000402000000,
                  0x0400040200000000, 0x0004020000000000},
                 {0x0000000000050800, 0x0000000005080008, 0x0000000508000805, 0x0000050800080500, 0x0005080008050000, 0x0508000805000000,
                  0x0800080500000000, 0x0008050000000000},
                 {0x00000000000A1100, 0x000000000A110011, 0x0000000A1100110A, 0x00000A1100110A00, 0x000A1100110A0000, 0x0A1100110A000000,
                  0x1100110A00000000, 0x00110A0000000000},
                 {0x0000000000142200, 0x0000000014220022, 0x0000001422002214, 0x0000142200221400, 0x0014220022140000, 0x1422002214000000,
                  0x2200221400000000, 0x0022140000000000},
                 {0x0000000000284400, 0x0000000028440044, 0x0000002844004428, 0x0000284400442800, 0x0028440044280000, 0x2844004428000000,
                  0x4400442800000000, 0x0044280000000000},
                 {0x0000000000508800, 0x0000000050880088, 0x0000005088008850, 0x0000508800885000, 0x0050880088500000, 0x5088008850000000,
                  0x8800885000000000, 0x0088500000000000},
                 {0x0000000000A01000, 0x00000000A0100010, 0x000000A0100010A0, 0x0000A0100010A000, 0x00A0100010A00000, 0xA0100010A0000000,
                  0x100010A000000000, 0x0010A00000000000},
                 {0x0000000000402000, 0x0000000040200020, 0x0000004020002040, 0x0000402000204000, 0x0040200020400000, 0x4020002040000000,
                  0x2000204000000000, 0x0020400000000000}}};

        constexpr static const std::array<const std::array<bitboard_t, 8>, 8> s_bishopMoves{
                {{0x8040201008040200, 0x4020100804020002, 0x2010080402000204, 0x1008040200020408, 0x0804020002040810, 0x0402000204081020,
                  0x0200020408102040, 0x0002040810204080},
                 {0x0080402010080500, 0x8040201008050005, 0x4020100805000508, 0x2010080500050810, 0x1008050005081020, 0x0805000508102040,
                  0x0500050810204080, 0x0005081020408000},
                 {0x0000804020110A00, 0x00804020110A000A, 0x804020110A000A11, 0x4020110A000A1120, 0x20110A000A112040, 0x110A000A11204080,
                  0x0A000A1120408000, 0x000A112040800000},
                 {0x0000008041221400, 0x0000804122140014, 0x0080412214001422, 0x8041221400142241, 0x4122140014224180, 0x2214001422418000,
                  0x1400142241800000, 0x0014224180000000},
                 {0x0000000182442800, 0x0000018244280028, 0x0001824428002844, 0x0182442800284482, 0x8244280028448201, 0x4428002844820100,
                  0x2800284482010000, 0x0028448201000000},
                 {0x0000010204885000, 0x0001020488500050, 0x0102048850005088, 0x0204885000508804, 0x0488500050880402, 0x8850005088040201,
                  0x5000508804020100, 0x0050880402010000},
                 {0x000102040810A000, 0x0102040810A000A0, 0x02040810A000A010, 0x040810A000A01008, 0x0810A000A0100804, 0x10A000A010080402,
                  0xA000A01008040201, 0x00A0100804020100},
                 {0x0102040810204000, 0x0204081020400040, 0x0408102040004020, 0x0810204000402010, 0x1020400040201008, 0x2040004020100804,
                  0x4000402010080402, 0x0040201008040201}}};

        constexpr static const std::array<const std::array<bitboard_t, 8>, 8> s_queenMoves{
                {{0x81412111090503FE, 0x412111090503FE03, 0x2111090503FE0305, 0x11090503FE030509, 0x090503FE03050911, 0x0503FE0305091121,
                  0x03FE030509112141, 0xFE03050911214181},
                 {0x02824222120A07FD, 0x824222120A07FD07, 0x4222120A07FD070A, 0x22120A07FD070A12, 0x120A07FD070A1222, 0x0A07FD070A122242,
                  0x07FD070A12224282, 0xFD070A1222428202},
                 {0x0404844424150EFB, 0x04844424150EFB0E, 0x844424150EFB0E15, 0x4424150EFB0E1524, 0x24150EFB0E152444, 0x150EFB0E15244484,
                  0x0EFB0E1524448404, 0xFB0E152444840404},
                 {0x08080888492A1CF7, 0x080888492A1CF71C, 0x0888492A1CF71C2A, 0x88492A1CF71C2A49, 0x492A1CF71C2A4988, 0x2A1CF71C2A498808,
                  0x1CF71C2A49880808, 0xF71C2A4988080808},
                 {0x10101011925438EF, 0x101011925438EF38, 0x1011925438EF3854, 0x11925438EF385492, 0x925438EF38549211, 0x5438EF3854921110,
                  0x38EF385492111010, 0xEF38549211101010},
                 {0x2020212224A870DF, 0x20212224A870DF70, 0x212224A870DF70A8, 0x2224A870DF70A824, 0x24A870DF70A82422, 0xA870DF70A8242221,
                  0x70DF70A824222120, 0xDF70A82422212020},
                 {0x404142444850E0BF, 0x4142444850E0BFE0, 0x42444850E0BFE050, 0x444850E0BFE05048, 0x4850E0BFE0504844, 0x50E0BFE050484442,
                  0xE0BFE05048444241, 0xBFE0504844424140},
                 {0x8182848890A0C07F, 0x82848890A0C07FC0, 0x848890A0C07FC0A0, 0x8890A0C07FC0A090, 0x90A0C07FC0A09088, 0xA0C07FC0A0908884,
                  0xC07FC0A090888482, 0x7FC0A09088848281}}};

        constexpr static const std::array<const std::array<bitboard_t, 8>, 8> s_kingMoves{
                {{0x0000000000000102, 0x0000000000010201, 0x0000000001020100, 0x0000000102010000, 0x0000010201000000, 0x0001020100000000,
                  0x0102010000000000, 0x0201000000000000},
                 {0x0000000000000205, 0x0000000000020502, 0x0000000002050200, 0x0000000205020000, 0x0000020502000000, 0x0002050200000000,
                  0x0205020000000000, 0x0502000000000000},
                 {0x000000000000040A, 0x0000000000040A04, 0x00000000040A0400, 0x000000040A040000, 0x0000040A04000000, 0x00040A0400000000,
                  0x040A040000000000, 0x0A04000000000000},
                 {0x0000000000000814, 0x0000000000081408, 0x0000000008140800, 0x0000000814080000, 0x0000081408000000, 0x0008140800000000,
                  0x0814080000000000, 0x1408000000000000},
                 {0x0000000000001028, 0x0000000000102810, 0x0000000010281000, 0x0000001028100000, 0x0000102810000000, 0x0010281000000000,
                  0x1028100000000000, 0x2810000000000000},
                 {0x0000000000002050, 0x0000000000205020, 0x0000000020502000, 0x0000002050200000, 0x0000205020000000, 0x0020502000000000,
                  0x2050200000000000, 0x5020000000000000},
                 {0x00000000000040A0, 0x000000000040A040, 0x0000000040A04000, 0x00000040A0400000, 0x000040A040000000, 0x0040A04000000000,
                  0x40A0400000000000, 0xA040000000000000},
                 {0x0000000000008040, 0x0000000000804080, 0x0000000080408000, 0x0000008040800000, 0x0000804080000000, 0x0080408000000000,
                  0x8040800000000000, 0x4080000000000000}}};

        [[nodiscard]] consteval static bitboard_t up(bitboard_t bits) noexcept { return bits << 8; }
        [[nodiscard]] consteval static bitboard_t down(bitboard_t bits) noexcept { return bits >> 8; }
        [[nodiscard]] consteval static bitboard_t left(bitboard_t bits) noexcept { return bits >> 1 & ~file(File::H); }
        [[nodiscard]] consteval static bitboard_t right(bitboard_t bits) noexcept { return bits << 1 & ~file(File::A); }

        std::array<bitboard_t, 14> m_bitboards{};
        Piece m_turn = Piece::White;

        [[nodiscard]] constexpr bitboard_t white() const noexcept { return get<Piece::White>(); }
        [[nodiscard]] constexpr bitboard_t black() const noexcept { return get<Piece::Black>(); }

        [[nodiscard]] constexpr bitboard_t all() const noexcept { return white() | black(); }
        [[nodiscard]] constexpr bitboard_t empty() const noexcept { return ~all(); }
        [[nodiscard]] constexpr Piece piece(int index) const noexcept { return piece(square(index)); }
        [[nodiscard]] constexpr Piece piece(File f, Rank r) const noexcept { return piece(square(f, r)); }
        [[nodiscard]] constexpr Piece piece(bitboard_t square) const noexcept
        {
            for (const Piece &p: pieces())
                if (m_bitboards[p] & square)
                    return p;

            return Piece::None;
        }
    };

    [[nodiscard]] consteval auto pieces() noexcept { return Board::pieces(); }
    [[nodiscard]] consteval auto files() noexcept { return Board::files(); }
    [[nodiscard]] consteval auto filesReversed() noexcept { return Board::filesReversed(); }
    [[nodiscard]] consteval auto ranks() noexcept { return Board::ranks(); }
    [[nodiscard]] consteval auto ranksReversed() noexcept { return Board::ranksReversed(); }

} // namespace chess

#endif // CHESS_ENGINE_BOARD_H
