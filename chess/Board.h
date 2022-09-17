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
        using bitboard_t = uint64_t;
        static_assert(sizeof(bitboard_t) == 8, "bitboard_t must be 64 bits");

        std::array<bitboard_t, 15> m_bitboards{};
        Color m_turn = Color::White;
        bitboard_t m_enPassantSquare = s_emptyBoard;

        [[nodiscard]] constexpr bitboard_t all() const noexcept { return ~m_bitboards[Piece::None]; }
        [[nodiscard]] constexpr Piece piece(bitboard_t square) const noexcept
        {
            for (const auto &p: s_piecesList)
                if (m_bitboards[p] & square)
                    return p;

            return Piece::None;
        }

        template<Piece P>
        [[nodiscard]] constexpr std::pair<File, Rank> find() const noexcept
        {
            const auto bitLocation = __builtin_ffsll(m_bitboards[P]) - 1;
            const auto f = 7 - (bitLocation & 7);
            const auto r = bitLocation >> 3;
            return {File(f), Rank(r)};
        }

        template<Color C>
        [[nodiscard]] constexpr bitboard_t attackedBy(File f, Rank r) const noexcept
        {
            const auto pawn = ColoredPiece::Pawn<C>;
            const auto knight = ColoredPiece::Knight<C>;
            const auto rook = ColoredPiece::Rook<C>;
            const auto bishop = ColoredPiece::Bishop<C>;
            const auto queen = ColoredPiece::Queen<C>;
            const auto king = ColoredPiece::King<C>;

            const auto opponentColor = ~C;

            const auto occupied = all();
            return (pawnAttacks<opponentColor>(f, r) & m_bitboards[pawn]) |
                   (rookMoves<opponentColor>(f, r, occupied) & (m_bitboards[rook] | m_bitboards[queen])) |
                   (knightMoves<opponentColor>(f, r) & m_bitboards[knight]) |
                   (bishopMoves<opponentColor>(f, r, occupied) & (m_bitboards[bishop] | m_bitboards[queen])) |
                   (kingMoves<opponentColor>(f, r) & m_bitboards[king]);
        }

        template<Color C>
        [[nodiscard]] constexpr bool inCheck() const noexcept
        {
            const auto king = ColoredPiece::King<C>;
            const auto [f, r] = find<king>();

            return attackedBy<~C>(f, r) != s_emptyBoard;
        }

        template<Color C>
        [[nodiscard]] constexpr bitboard_t pawnAttacks(File f, Rank r) const noexcept
        {
            if constexpr (C == Color::White) return (s_wPawnAttacks[f][r] & (m_bitboards[Color::Black] | m_enPassantSquare));
            else if constexpr (C == Color::Black) return (s_bPawnAttacks[f][r] & (m_bitboards[Color::White] | m_enPassantSquare));
        }

        template<Color C>
        [[nodiscard]] constexpr bitboard_t pawnMoves(File f, Rank r) const noexcept
        {
            const auto empty = m_bitboards[Piece::None];
            const auto attacks = pawnAttacks<C>(f, r);

            if constexpr (C == Color::White)
            {
                const auto moves = s_wPawnMoves[f][r] & empty;
                const auto doubleMoves = r == Rank::Two ? (moves << 8) & empty : s_emptyBoard;
                return attacks | moves | doubleMoves;
            }
            else if constexpr (C == Color::Black)
            {
                const auto moves = s_bPawnMoves[f][r] & empty;
                const auto doubleMoves = r == Rank::Seven ? (moves >> 8) & empty : s_emptyBoard;
                return attacks | moves | doubleMoves;
            }
        }

        template<Color C>
        [[nodiscard]] constexpr bitboard_t knightMoves(File f, Rank r) const noexcept
        {
            return s_knightMoves[f][r] & ~m_bitboards[C];
        }

        template<Color C>
        [[nodiscard]] constexpr bitboard_t kingMoves(File f, Rank r) const noexcept
        {
            return s_kingMoves[f][r] & ~m_bitboards[C];
        }

        template<Color C>
        [[nodiscard]] constexpr bitboard_t rookMoves(File f, Rank r, bitboard_t occupancy) const noexcept
        {
            return rookMoves(f, r, occupancy) & ~m_bitboards[C];
        }

        template<Color C>
        [[nodiscard]] constexpr bitboard_t bishopMoves(File f, Rank r, bitboard_t occupancy) const noexcept
        {
            return bishopMoves(f, r, occupancy) & ~m_bitboards[C];
        }

        template<Color C>
        [[nodiscard]] constexpr bitboard_t queenMoves(File f, Rank r, bitboard_t occupancy) const noexcept
        {
            return queenMoves(f, r, occupancy) & ~m_bitboards[C];
        }

        template<Piece P>
        [[nodiscard]] constexpr bitboard_t moves(File f, Rank r) const noexcept
        {
            if constexpr (P == Piece::WPawn) return pawnMoves<Color::White>(f, r);
            else if constexpr (P == Piece::WRook) return rookMoves<Color::White>(f, r, all());
            else if constexpr (P == Piece::WKnight) return knightMoves<Color::White>(f, r);
            else if constexpr (P == Piece::WBishop) return bishopMoves<Color::White>(f, r, all());
            else if constexpr (P == Piece::WQueen) return queenMoves<Color::White>(f, r, all());
            else if constexpr (P == Piece::WKing) return kingMoves<Color::White>(f, r);

            else if constexpr (P == Piece::BPawn) return pawnMoves<Color::Black>(f, r);
            else if constexpr (P == Piece::BRook) return rookMoves<Color::Black>(f, r, all());
            else if constexpr (P == Piece::BKnight) return knightMoves<Color::Black>(f, r);
            else if constexpr (P == Piece::BBishop) return bishopMoves<Color::Black>(f, r, all());
            else if constexpr (P == Piece::BQueen) return queenMoves<Color::Black>(f, r, all());
            else if constexpr (P == Piece::BKing) return kingMoves<Color::Black>(f, r);
        }

        [[nodiscard]] constexpr bitboard_t moves(Piece p, File f, Rank r) const noexcept
        {
            switch (p)
            {
                case Piece::WPawn:
                    return moves<Piece::WPawn>(f, r);
                case Piece::WKnight:
                    return moves<Piece::WKnight>(f, r);
                case Piece::WRook:
                    return moves<Piece::WRook>(f, r);
                case Piece::WBishop:
                    return moves<Piece::WBishop>(f, r);
                case Piece::WQueen:
                    return moves<Piece::WQueen>(f, r);
                case Piece::WKing:
                    return moves<Piece::WKing>(f, r);
                case Piece::BPawn:
                    return moves<Piece::BPawn>(f, r);
                case Piece::BKnight:
                    return moves<Piece::BKnight>(f, r);
                case Piece::BRook:
                    return moves<Piece::BRook>(f, r);
                case Piece::BBishop:
                    return moves<Piece::BBishop>(f, r);
                case Piece::BQueen:
                    return moves<Piece::BQueen>(f, r);
                case Piece::BKing:
                    return moves<Piece::BKing>(f, r);
                case None:
                default:
                    return s_emptyBoard;
            }
        }

        template<Color C>
        constexpr void move(bitboard_t fromSquare, Piece fromPiece, bitboard_t toSquare, Piece toPiece) noexcept
        {
            m_bitboards[fromPiece] ^= (fromSquare | toSquare);
            m_bitboards[C] ^= (fromSquare | toSquare);
            m_bitboards[Piece::None] ^= fromSquare;

            m_bitboards[toPiece] ^= toSquare;
            if (toPiece != Piece::None) { m_bitboards[~C] ^= toSquare; }
        }

        template<Color C>
        constexpr void promote(bitboard_t fromSquare, Piece fromPiece, bitboard_t toSquare, Piece toPiece, Piece newPiece) noexcept
        {
            move<C>(fromSquare, fromPiece, toSquare, toPiece);
            m_bitboards[fromPiece] ^= toSquare;
            m_bitboards[newPiece] ^= toSquare;
        }

        template<Color C>
        [[nodiscard]] static consteval bitboard_t castlingSquares() noexcept
        {
            const auto rank = (C == Color::White) ? Rank::One : Rank::Eight;
            return square(File::C, rank) | square(File::G, rank);
        }

        template<Color C>
        constexpr bool moveHelper(File fromFile, Rank fromRank, File toFile, Rank toRank, const std::string_view &uciMove)
        {
            const auto fromSquare = square(fromFile, fromRank);
            const auto fromPiece = piece(fromSquare);

            const auto legalMoves = moves(fromPiece, fromFile, fromRank);

            const auto toSquare = square(toFile, toRank);
            if ((legalMoves & toSquare) == s_emptyBoard) { return false; }
            const auto toPiece = piece(toSquare);

            const bool promotion = fromPiece == ColoredPiece::Pawn<C> && toRank == Rank::Eight;
            if (!promotion)
            {
                move<C>(fromSquare, fromPiece, toSquare, toPiece);

                const bool doublePush = (fromPiece == Piece::WPawn && fromRank == Rank::Two && toRank == Rank::Four) ||
                                        (fromPiece == Piece::BPawn && fromRank == Rank::Seven && toRank == Rank::Five);

                m_enPassantSquare = doublePush ? toSquare >> 8 : s_emptyBoard;
                return true;
            }
            else
            {
                // If piece is unspecified, assume queen
                const auto newPiece = (uciMove.size() == 4) ? ColoredPiece::Queen<C> : charPiece<C>(uciMove[4]);
                const bool validPromotion =
                        newPiece == ColoredPiece::Queen<C> || newPiece == ColoredPiece::Bishop<C> || newPiece == ColoredPiece::Rook<C> ||
                        newPiece == ColoredPiece::Knight<C>;
                if (!validPromotion) { return false; }
                promote<C>(fromSquare, fromPiece, toSquare, toPiece, newPiece);
                m_enPassantSquare = s_emptyBoard;
                return true;
            }
        }

        [[nodiscard]] static constexpr bitboard_t square(int index) noexcept { return s_squares[7 - (index & 7)][index >> 3]; }

        [[nodiscard]] static constexpr bitboard_t square(File f, Rank r) noexcept { return s_squares[f][r]; }

        struct SquareRays { bitboard_t lower, upper, line; };
        [[nodiscard]] static constexpr bitboard_t lineAttacks(bitboard_t occupancy, const SquareRays &rays) noexcept
        {
            bitboard_t lower = rays.lower & occupancy;
            bitboard_t upper = rays.upper & occupancy;
            bitboard_t ms1B = (0x8000000000000000) >> __builtin_clzll(lower | 1);
            bitboard_t diff = upper ^ (upper - ms1B);
            return rays.line & diff;
        }

        [[nodiscard]] static constexpr bitboard_t rookMoves(File f, Rank r, bitboard_t occupancy) noexcept
        {
            return lineAttacks(occupancy, s_fileRays[f][r]) | lineAttacks(occupancy, s_rankRays[f][r]);
        }

        [[nodiscard]] static constexpr bitboard_t bishopMoves(File f, Rank r, bitboard_t occupancy) noexcept
        {
            return lineAttacks(occupancy, s_diagonalRays[f][r]) | lineAttacks(occupancy, s_antidiagonalRays[f][r]);
        }

        [[nodiscard]] static constexpr bitboard_t queenMoves(File f, Rank r, bitboard_t occupancy) noexcept
        {
            return rookMoves(f, r, occupancy) | bishopMoves(f, r, occupancy);
        }

        static constexpr const bitboard_t s_emptyBoard = 0;
        static constexpr const std::array<Piece, 12> s_piecesList{Piece::WPawn, Piece::WRook, Piece::WKnight, Piece::WBishop, Piece::WQueen,
                                                                  Piece::WKing, Piece::BPawn, Piece::BRook, Piece::BKnight, Piece::BBishop,
                                                                  Piece::BQueen, Piece::BKing};

        static constexpr const std::array<char, 15> s_pieceChars = {'w', 'P', 'N', 'R', 'B', 'Q', 'K', '.', 'b', 'p', 'n', 'r', 'b', 'q',
                                                                    'k'};

        static constexpr const std::array<const std::array<bitboard_t, 8>, 8> s_squares{
                {{0x0000000000000080, 0x0000000000008000, 0x0000000000800000, 0x0000000080000000, 0x0000008000000000, 0x0000800000000000,
                  0x0080000000000000, 0x8000000000000000},
                 {0x0000000000000040, 0x0000000000004000, 0x0000000000400000, 0x0000000040000000, 0x0000004000000000, 0x0000400000000000,
                  0x0040000000000000, 0x4000000000000000},
                 {0x0000000000000020, 0x0000000000002000, 0x0000000000200000, 0x0000000020000000, 0x0000002000000000, 0x0000200000000000,
                  0x0020000000000000, 0x2000000000000000},
                 {0x0000000000000010, 0x0000000000001000, 0x0000000000100000, 0x0000000010000000, 0x0000001000000000, 0x0000100000000000,
                  0x0010000000000000, 0x1000000000000000},
                 {0x0000000000000008, 0x0000000000000800, 0x0000000000080000, 0x0000000008000000, 0x0000000800000000, 0x0000080000000000,
                  0x0008000000000000, 0x0800000000000000},
                 {0x0000000000000004, 0x0000000000000400, 0x0000000000040000, 0x0000000004000000, 0x0000000400000000, 0x0000040000000000,
                  0x0004000000000000, 0x0400000000000000},
                 {0x0000000000000002, 0x0000000000000200, 0x0000000000020000, 0x0000000002000000, 0x0000000200000000, 0x0000020000000000,
                  0x0002000000000000, 0x0200000000000000},
                 {0x0000000000000001, 0x0000000000000100, 0x0000000000010000, 0x0000000001000000, 0x0000000100000000, 0x0000010000000000,
                  0x0001000000000000, 0x0100000000000000}}};

        static constexpr const std::array<bitboard_t, 15> s_startingPosition{0xFFFF000000000000, 0x000000000000FF00, 0x0000000000000042,
                                                                             0x0000000000000081, 0x0000000000000024, 0x0000000000000010,
                                                                             0x0000000000000008, 0x0000FFFFFFFF0000, 0x000000000000FFFF,
                                                                             0x00FF000000000000, 0x4200000000000000, 0x8100000000000000,
                                                                             0x2400000000000000, 0x1000000000000000, 0x8000000000000000};

        static constexpr const std::array<const std::array<bitboard_t, 8>, 8> s_wPawnMoves{
                {{0x0000000000008000, 0x0000000000800000, 0x0000000080000000, 0x0000008000000000, 0x0000800000000000, 0x0080000000000000,
                  0x8000000000000000, 0x0000000000000000},
                 {0x0000000000004000, 0x0000000000400000, 0x0000000040000000, 0x0000004000000000, 0x0000400000000000, 0x0040000000000000,
                  0x4000000000000000, 0x0000000000000000},
                 {0x0000000000002000, 0x0000000000200000, 0x0000000020000000, 0x0000002000000000, 0x0000200000000000, 0x0020000000000000,
                  0x2000000000000000, 0x0000000000000000},
                 {0x0000000000001000, 0x0000000000100000, 0x0000000010000000, 0x0000001000000000, 0x0000100000000000, 0x0010000000000000,
                  0x1000000000000000, 0x0000000000000000},
                 {0x0000000000000800, 0x0000000000080000, 0x0000000008000000, 0x0000000800000000, 0x0000080000000000, 0x0008000000000000,
                  0x0800000000000000, 0x0000000000000000},
                 {0x0000000000000400, 0x0000000000040000, 0x0000000004000000, 0x0000000400000000, 0x0000040000000000, 0x0004000000000000,
                  0x0400000000000000, 0x0000000000000000},
                 {0x0000000000000200, 0x0000000000020000, 0x0000000002000000, 0x0000000200000000, 0x0000020000000000, 0x0002000000000000,
                  0x0200000000000000, 0x0000000000000000},
                 {0x0000000000000100, 0x0000000000010000, 0x0000000001000000, 0x0000000100000000, 0x0000010000000000, 0x0001000000000000,
                  0x0100000000000000, 0x0000000000000000}}};

        static constexpr const std::array<const std::array<bitboard_t, 8>, 8> s_bPawnMoves{
                {{0x0000000000000000, 0x0000000000000080, 0x0000000000008000, 0x0000000000800000, 0x0000000080000000, 0x0000008000000000,
                  0x0000800000000000, 0x0080000000000000},
                 {0x0000000000000000, 0x0000000000000040, 0x0000000000004000, 0x0000000000400000, 0x0000000040000000, 0x0000004000000000,
                  0x0000400000000000, 0x0040000000000000},
                 {0x0000000000000000, 0x0000000000000020, 0x0000000000002000, 0x0000000000200000, 0x0000000020000000, 0x0000002000000000,
                  0x0000200000000000, 0x0020000000000000},
                 {0x0000000000000000, 0x0000000000000010, 0x0000000000001000, 0x0000000000100000, 0x0000000010000000, 0x0000001000000000,
                  0x0000100000000000, 0x0010000000000000},
                 {0x0000000000000000, 0x0000000000000008, 0x0000000000000800, 0x0000000000080000, 0x0000000008000000, 0x0000000800000000,
                  0x0000080000000000, 0x0008000000000000},
                 {0x0000000000000000, 0x0000000000000004, 0x0000000000000400, 0x0000000000040000, 0x0000000004000000, 0x0000000400000000,
                  0x0000040000000000, 0x0004000000000000},
                 {0x0000000000000000, 0x0000000000000002, 0x0000000000000200, 0x0000000000020000, 0x0000000002000000, 0x0000000200000000,
                  0x0000020000000000, 0x0002000000000000},
                 {0x0000000000000000, 0x0000000000000001, 0x0000000000000100, 0x0000000000010000, 0x0000000001000000, 0x0000000100000000,
                  0x0000010000000000, 0x0001000000000000}}};

        static constexpr const std::array<const std::array<bitboard_t, 8>, 8> s_wPawnAttacks{
                {{0x0000000000004000, 0x0000000000400000, 0x0000000040000000, 0x0000004000000000, 0x0000400000000000, 0x0040000000000000,
                  0x4000000000000000, 0x0000000000000000},
                 {0x000000000000A000, 0x0000000000A00000, 0x00000000A0000000, 0x000000A000000000, 0x0000A00000000000, 0x00A0000000000000,
                  0xA000000000000000, 0x0000000000000000},
                 {0x0000000000005000, 0x0000000000500000, 0x0000000050000000, 0x0000005000000000, 0x0000500000000000, 0x0050000000000000,
                  0x5000000000000000, 0x0000000000000000},
                 {0x0000000000002800, 0x0000000000280000, 0x0000000028000000, 0x0000002800000000, 0x0000280000000000, 0x0028000000000000,
                  0x2800000000000000, 0x0000000000000000},
                 {0x0000000000001400, 0x0000000000140000, 0x0000000014000000, 0x0000001400000000, 0x0000140000000000, 0x0014000000000000,
                  0x1400000000000000, 0x0000000000000000},
                 {0x0000000000000A00, 0x00000000000A0000, 0x000000000A000000, 0x0000000A00000000, 0x00000A0000000000, 0x000A000000000000,
                  0x0A00000000000000, 0x0000000000000000},
                 {0x0000000000000500, 0x0000000000050000, 0x0000000005000000, 0x0000000500000000, 0x0000050000000000, 0x0005000000000000,
                  0x0500000000000000, 0x0000000000000000},
                 {0x0000000000000200, 0x0000000000020000, 0x0000000002000000, 0x0000000200000000, 0x0000020000000000, 0x0002000000000000,
                  0x0200000000000000, 0x0000000000000000}}};

        static constexpr const std::array<const std::array<bitboard_t, 8>, 8> s_bPawnAttacks{
                {{0x0000000000000000, 0x0000000000000040, 0x0000000000004000, 0x0000000000400000, 0x0000000040000000, 0x0000004000000000,
                  0x0000400000000000, 0x0040000000000000},
                 {0x0000000000000000, 0x00000000000000A0, 0x000000000000A000, 0x0000000000A00000, 0x00000000A0000000, 0x000000A000000000,
                  0x0000A00000000000, 0x00A0000000000000},
                 {0x0000000000000000, 0x0000000000000050, 0x0000000000005000, 0x0000000000500000, 0x0000000050000000, 0x0000005000000000,
                  0x0000500000000000, 0x0050000000000000},
                 {0x0000000000000000, 0x0000000000000028, 0x0000000000002800, 0x0000000000280000, 0x0000000028000000, 0x0000002800000000,
                  0x0000280000000000, 0x0028000000000000},
                 {0x0000000000000000, 0x0000000000000014, 0x0000000000001400, 0x0000000000140000, 0x0000000014000000, 0x0000001400000000,
                  0x0000140000000000, 0x0014000000000000},
                 {0x0000000000000000, 0x000000000000000A, 0x0000000000000A00, 0x00000000000A0000, 0x000000000A000000, 0x0000000A00000000,
                  0x00000A0000000000, 0x000A000000000000},
                 {0x0000000000000000, 0x0000000000000005, 0x0000000000000500, 0x0000000000050000, 0x0000000005000000, 0x0000000500000000,
                  0x0000050000000000, 0x0005000000000000},
                 {0x0000000000000000, 0x0000000000000002, 0x0000000000000200, 0x0000000000020000, 0x0000000002000000, 0x0000000200000000,
                  0x0000020000000000, 0x0002000000000000}}};

        static constexpr const std::array<const std::array<bitboard_t, 8>, 8> s_knightMoves{
                {{0x0000000000402000, 0x0000000040200020, 0x0000004020002040, 0x0000402000204000, 0x0040200020400000, 0x4020002040000000,
                  0x2000204000000000, 0x0020400000000000},
                 {0x0000000000A01000, 0x00000000A0100010, 0x000000A0100010A0, 0x0000A0100010A000, 0x00A0100010A00000, 0xA0100010A0000000,
                  0x100010A000000000, 0x0010A00000000000},
                 {0x0000000000508800, 0x0000000050880088, 0x0000005088008850, 0x0000508800885000, 0x0050880088500000, 0x5088008850000000,
                  0x8800885000000000, 0x0088500000000000},
                 {0x0000000000284400, 0x0000000028440044, 0x0000002844004428, 0x0000284400442800, 0x0028440044280000, 0x2844004428000000,
                  0x4400442800000000, 0x0044280000000000},
                 {0x0000000000142200, 0x0000000014220022, 0x0000001422002214, 0x0000142200221400, 0x0014220022140000, 0x1422002214000000,
                  0x2200221400000000, 0x0022140000000000},
                 {0x00000000000A1100, 0x000000000A110011, 0x0000000A1100110A, 0x00000A1100110A00, 0x000A1100110A0000, 0x0A1100110A000000,
                  0x1100110A00000000, 0x00110A0000000000},
                 {0x0000000000050800, 0x0000000005080008, 0x0000000508000805, 0x0000050800080500, 0x0005080008050000, 0x0508000805000000,
                  0x0800080500000000, 0x0008050000000000},
                 {0x0000000000020400, 0x0000000002040004, 0x0000000204000402, 0x0000020400040200, 0x0002040004020000, 0x0204000402000000,
                  0x0400040200000000, 0x0004020000000000}}};

        static constexpr const std::array<const std::array<bitboard_t, 8>, 8> s_kingMoves{
                {{0x000000000000C040, 0x0000000000C040C0, 0x00000000C040C000, 0x000000C040C00000, 0x0000C040C0000000, 0x00C040C000000000,
                  0xC040C00000000000, 0x40C0000000000000},
                 {0x000000000000E0A0, 0x0000000000E0A0E0, 0x00000000E0A0E000, 0x000000E0A0E00000, 0x0000E0A0E0000000, 0x00E0A0E000000000,
                  0xE0A0E00000000000, 0xA0E0000000000000},
                 {0x0000000000007050, 0x0000000000705070, 0x0000000070507000, 0x0000007050700000, 0x0000705070000000, 0x0070507000000000,
                  0x7050700000000000, 0x5070000000000000},
                 {0x0000000000003828, 0x0000000000382838, 0x0000000038283800, 0x0000003828380000, 0x0000382838000000, 0x0038283800000000,
                  0x3828380000000000, 0x2838000000000000},
                 {0x0000000000001C14, 0x00000000001C141C, 0x000000001C141C00, 0x0000001C141C0000, 0x00001C141C000000, 0x001C141C00000000,
                  0x1C141C0000000000, 0x141C000000000000},
                 {0x0000000000000E0A, 0x00000000000E0A0E, 0x000000000E0A0E00, 0x0000000E0A0E0000, 0x00000E0A0E000000, 0x000E0A0E00000000,
                  0x0E0A0E0000000000, 0x0A0E000000000000},
                 {0x0000000000000705, 0x0000000000070507, 0x0000000007050700, 0x0000000705070000, 0x0000070507000000, 0x0007050700000000,
                  0x0705070000000000, 0x0507000000000000},
                 {0x0000000000000302, 0x0000000000030203, 0x0000000003020300, 0x0000000302030000, 0x0000030203000000, 0x0003020300000000,
                  0x0302030000000000, 0x0203000000000000}}};

        static constexpr const std::array<const std::array<const SquareRays, 8>, 8> s_fileRays{
                {{SquareRays{0x0000000000000000, 0x8080808080808000, 0x8080808080808000},
                  SquareRays{0x0000000000000080, 0x8080808080800000, 0x8080808080800080},
                  SquareRays{0x0000000000008080, 0x8080808080000000, 0x8080808080008080},
                  SquareRays{0x0000000000808080, 0x8080808000000000, 0x8080808000808080},
                  SquareRays{0x0000000080808080, 0x8080800000000000, 0x8080800080808080},
                  SquareRays{0x0000008080808080, 0x8080000000000000, 0x8080008080808080},
                  SquareRays{0x0000808080808080, 0x8000000000000000, 0x8000808080808080},
                  SquareRays{0x0080808080808080, 0x0000000000000000, 0x0080808080808080}},
                 {SquareRays{0x0000000000000000, 0x4040404040404000, 0x4040404040404000},
                  SquareRays{0x0000000000000040, 0x4040404040400000, 0x4040404040400040},
                  SquareRays{0x0000000000004040, 0x4040404040000000, 0x4040404040004040},
                  SquareRays{0x0000000000404040, 0x4040404000000000, 0x4040404000404040},
                  SquareRays{0x0000000040404040, 0x4040400000000000, 0x4040400040404040},
                  SquareRays{0x0000004040404040, 0x4040000000000000, 0x4040004040404040},
                  SquareRays{0x0000404040404040, 0x4000000000000000, 0x4000404040404040},
                  SquareRays{0x0040404040404040, 0x0000000000000000, 0x0040404040404040}},
                 {SquareRays{0x0000000000000000, 0x2020202020202000, 0x2020202020202000},
                  SquareRays{0x0000000000000020, 0x2020202020200000, 0x2020202020200020},
                  SquareRays{0x0000000000002020, 0x2020202020000000, 0x2020202020002020},
                  SquareRays{0x0000000000202020, 0x2020202000000000, 0x2020202000202020},
                  SquareRays{0x0000000020202020, 0x2020200000000000, 0x2020200020202020},
                  SquareRays{0x0000002020202020, 0x2020000000000000, 0x2020002020202020},
                  SquareRays{0x0000202020202020, 0x2000000000000000, 0x2000202020202020},
                  SquareRays{0x0020202020202020, 0x0000000000000000, 0x0020202020202020}},
                 {SquareRays{0x0000000000000000, 0x1010101010101000, 0x1010101010101000},
                  SquareRays{0x0000000000000010, 0x1010101010100000, 0x1010101010100010},
                  SquareRays{0x0000000000001010, 0x1010101010000000, 0x1010101010001010},
                  SquareRays{0x0000000000101010, 0x1010101000000000, 0x1010101000101010},
                  SquareRays{0x0000000010101010, 0x1010100000000000, 0x1010100010101010},
                  SquareRays{0x0000001010101010, 0x1010000000000000, 0x1010001010101010},
                  SquareRays{0x0000101010101010, 0x1000000000000000, 0x1000101010101010},
                  SquareRays{0x0010101010101010, 0x0000000000000000, 0x0010101010101010}},
                 {SquareRays{0x0000000000000000, 0x0808080808080800, 0x0808080808080800},
                  SquareRays{0x0000000000000008, 0x0808080808080000, 0x0808080808080008},
                  SquareRays{0x0000000000000808, 0x0808080808000000, 0x0808080808000808},
                  SquareRays{0x0000000000080808, 0x0808080800000000, 0x0808080800080808},
                  SquareRays{0x0000000008080808, 0x0808080000000000, 0x0808080008080808},
                  SquareRays{0x0000000808080808, 0x0808000000000000, 0x0808000808080808},
                  SquareRays{0x0000080808080808, 0x0800000000000000, 0x0800080808080808},
                  SquareRays{0x0008080808080808, 0x0000000000000000, 0x0008080808080808}},
                 {SquareRays{0x0000000000000000, 0x0404040404040400, 0x0404040404040400},
                  SquareRays{0x0000000000000004, 0x0404040404040000, 0x0404040404040004},
                  SquareRays{0x0000000000000404, 0x0404040404000000, 0x0404040404000404},
                  SquareRays{0x0000000000040404, 0x0404040400000000, 0x0404040400040404},
                  SquareRays{0x0000000004040404, 0x0404040000000000, 0x0404040004040404},
                  SquareRays{0x0000000404040404, 0x0404000000000000, 0x0404000404040404},
                  SquareRays{0x0000040404040404, 0x0400000000000000, 0x0400040404040404},
                  SquareRays{0x0004040404040404, 0x0000000000000000, 0x0004040404040404}},
                 {SquareRays{0x0000000000000000, 0x0202020202020200, 0x0202020202020200},
                  SquareRays{0x0000000000000002, 0x0202020202020000, 0x0202020202020002},
                  SquareRays{0x0000000000000202, 0x0202020202000000, 0x0202020202000202},
                  SquareRays{0x0000000000020202, 0x0202020200000000, 0x0202020200020202},
                  SquareRays{0x0000000002020202, 0x0202020000000000, 0x0202020002020202},
                  SquareRays{0x0000000202020202, 0x0202000000000000, 0x0202000202020202},
                  SquareRays{0x0000020202020202, 0x0200000000000000, 0x0200020202020202},
                  SquareRays{0x0002020202020202, 0x0000000000000000, 0x0002020202020202}},
                 {SquareRays{0x0000000000000000, 0x0101010101010100, 0x0101010101010100},
                  SquareRays{0x0000000000000001, 0x0101010101010000, 0x0101010101010001},
                  SquareRays{0x0000000000000101, 0x0101010101000000, 0x0101010101000101},
                  SquareRays{0x0000000000010101, 0x0101010100000000, 0x0101010100010101},
                  SquareRays{0x0000000001010101, 0x0101010000000000, 0x0101010001010101},
                  SquareRays{0x0000000101010101, 0x0101000000000000, 0x0101000101010101},
                  SquareRays{0x0000010101010101, 0x0100000000000000, 0x0100010101010101},
                  SquareRays{0x0001010101010101, 0x0000000000000000, 0x0001010101010101}}}};

        static constexpr const std::array<const std::array<const SquareRays, 8>, 8> s_rankRays{
                {{SquareRays{0x000000000000007F, 0x0000000000000000, 0x000000000000007F},
                  SquareRays{0x0000000000007F00, 0x0000000000000000, 0x0000000000007F00},
                  SquareRays{0x00000000007F0000, 0x0000000000000000, 0x00000000007F0000},
                  SquareRays{0x000000007F000000, 0x0000000000000000, 0x000000007F000000},
                  SquareRays{0x0000007F00000000, 0x0000000000000000, 0x0000007F00000000},
                  SquareRays{0x00007F0000000000, 0x0000000000000000, 0x00007F0000000000},
                  SquareRays{0x007F000000000000, 0x0000000000000000, 0x007F000000000000},
                  SquareRays{0x7F00000000000000, 0x0000000000000000, 0x7F00000000000000}},
                 {SquareRays{0x000000000000003F, 0x0000000000000080, 0x00000000000000BF},
                  SquareRays{0x0000000000003F00, 0x0000000000008000, 0x000000000000BF00},
                  SquareRays{0x00000000003F0000, 0x0000000000800000, 0x0000000000BF0000},
                  SquareRays{0x000000003F000000, 0x0000000080000000, 0x00000000BF000000},
                  SquareRays{0x0000003F00000000, 0x0000008000000000, 0x000000BF00000000},
                  SquareRays{0x00003F0000000000, 0x0000800000000000, 0x0000BF0000000000},
                  SquareRays{0x003F000000000000, 0x0080000000000000, 0x00BF000000000000},
                  SquareRays{0x3F00000000000000, 0x8000000000000000, 0xBF00000000000000}},
                 {SquareRays{0x000000000000001F, 0x00000000000000C0, 0x00000000000000DF},
                  SquareRays{0x0000000000001F00, 0x000000000000C000, 0x000000000000DF00},
                  SquareRays{0x00000000001F0000, 0x0000000000C00000, 0x0000000000DF0000},
                  SquareRays{0x000000001F000000, 0x00000000C0000000, 0x00000000DF000000},
                  SquareRays{0x0000001F00000000, 0x000000C000000000, 0x000000DF00000000},
                  SquareRays{0x00001F0000000000, 0x0000C00000000000, 0x0000DF0000000000},
                  SquareRays{0x001F000000000000, 0x00C0000000000000, 0x00DF000000000000},
                  SquareRays{0x1F00000000000000, 0xC000000000000000, 0xDF00000000000000}},
                 {SquareRays{0x000000000000000F, 0x00000000000000E0, 0x00000000000000EF},
                  SquareRays{0x0000000000000F00, 0x000000000000E000, 0x000000000000EF00},
                  SquareRays{0x00000000000F0000, 0x0000000000E00000, 0x0000000000EF0000},
                  SquareRays{0x000000000F000000, 0x00000000E0000000, 0x00000000EF000000},
                  SquareRays{0x0000000F00000000, 0x000000E000000000, 0x000000EF00000000},
                  SquareRays{0x00000F0000000000, 0x0000E00000000000, 0x0000EF0000000000},
                  SquareRays{0x000F000000000000, 0x00E0000000000000, 0x00EF000000000000},
                  SquareRays{0x0F00000000000000, 0xE000000000000000, 0xEF00000000000000}},
                 {SquareRays{0x0000000000000007, 0x00000000000000F0, 0x00000000000000F7},
                  SquareRays{0x0000000000000700, 0x000000000000F000, 0x000000000000F700},
                  SquareRays{0x0000000000070000, 0x0000000000F00000, 0x0000000000F70000},
                  SquareRays{0x0000000007000000, 0x00000000F0000000, 0x00000000F7000000},
                  SquareRays{0x0000000700000000, 0x000000F000000000, 0x000000F700000000},
                  SquareRays{0x0000070000000000, 0x0000F00000000000, 0x0000F70000000000},
                  SquareRays{0x0007000000000000, 0x00F0000000000000, 0x00F7000000000000},
                  SquareRays{0x0700000000000000, 0xF000000000000000, 0xF700000000000000}},
                 {SquareRays{0x0000000000000003, 0x00000000000000F8, 0x00000000000000FB},
                  SquareRays{0x0000000000000300, 0x000000000000F800, 0x000000000000FB00},
                  SquareRays{0x0000000000030000, 0x0000000000F80000, 0x0000000000FB0000},
                  SquareRays{0x0000000003000000, 0x00000000F8000000, 0x00000000FB000000},
                  SquareRays{0x0000000300000000, 0x000000F800000000, 0x000000FB00000000},
                  SquareRays{0x0000030000000000, 0x0000F80000000000, 0x0000FB0000000000},
                  SquareRays{0x0003000000000000, 0x00F8000000000000, 0x00FB000000000000},
                  SquareRays{0x0300000000000000, 0xF800000000000000, 0xFB00000000000000}},
                 {SquareRays{0x0000000000000001, 0x00000000000000FC, 0x00000000000000FD},
                  SquareRays{0x0000000000000100, 0x000000000000FC00, 0x000000000000FD00},
                  SquareRays{0x0000000000010000, 0x0000000000FC0000, 0x0000000000FD0000},
                  SquareRays{0x0000000001000000, 0x00000000FC000000, 0x00000000FD000000},
                  SquareRays{0x0000000100000000, 0x000000FC00000000, 0x000000FD00000000},
                  SquareRays{0x0000010000000000, 0x0000FC0000000000, 0x0000FD0000000000},
                  SquareRays{0x0001000000000000, 0x00FC000000000000, 0x00FD000000000000},
                  SquareRays{0x0100000000000000, 0xFC00000000000000, 0xFD00000000000000}},
                 {SquareRays{0x0000000000000000, 0x00000000000000FE, 0x00000000000000FE},
                  SquareRays{0x0000000000000000, 0x000000000000FE00, 0x000000000000FE00},
                  SquareRays{0x0000000000000000, 0x0000000000FE0000, 0x0000000000FE0000},
                  SquareRays{0x0000000000000000, 0x00000000FE000000, 0x00000000FE000000},
                  SquareRays{0x0000000000000000, 0x000000FE00000000, 0x000000FE00000000},
                  SquareRays{0x0000000000000000, 0x0000FE0000000000, 0x0000FE0000000000},
                  SquareRays{0x0000000000000000, 0x00FE000000000000, 0x00FE000000000000},
                  SquareRays{0x0000000000000000, 0xFE00000000000000, 0xFE00000000000000}}}};

        static constexpr const std::array<const std::array<const SquareRays, 8>, 8> s_diagonalRays{
                {{SquareRays{0x0000000000000000, 0x0102040810204000, 0x0102040810204000},
                  SquareRays{0x0000000000000000, 0x0204081020400000, 0x0204081020400000},
                  SquareRays{0x0000000000000000, 0x0408102040000000, 0x0408102040000000},
                  SquareRays{0x0000000000000000, 0x0810204000000000, 0x0810204000000000},
                  SquareRays{0x0000000000000000, 0x1020400000000000, 0x1020400000000000},
                  SquareRays{0x0000000000000000, 0x2040000000000000, 0x2040000000000000},
                  SquareRays{0x0000000000000000, 0x4000000000000000, 0x4000000000000000},
                  SquareRays{0x0000000000000000, 0x0000000000000000, 0x0000000000000000}},
                 {SquareRays{0x0000000000000000, 0x0001020408102000, 0x0001020408102000},
                  SquareRays{0x0000000000000080, 0x0102040810200000, 0x0102040810200080},
                  SquareRays{0x0000000000008000, 0x0204081020000000, 0x0204081020008000},
                  SquareRays{0x0000000000800000, 0x0408102000000000, 0x0408102000800000},
                  SquareRays{0x0000000080000000, 0x0810200000000000, 0x0810200080000000},
                  SquareRays{0x0000008000000000, 0x1020000000000000, 0x1020008000000000},
                  SquareRays{0x0000800000000000, 0x2000000000000000, 0x2000800000000000},
                  SquareRays{0x0080000000000000, 0x0000000000000000, 0x0080000000000000}},
                 {SquareRays{0x0000000000000000, 0x0000010204081000, 0x0000010204081000},
                  SquareRays{0x0000000000000040, 0x0001020408100000, 0x0001020408100040},
                  SquareRays{0x0000000000004080, 0x0102040810000000, 0x0102040810004080},
                  SquareRays{0x0000000000408000, 0x0204081000000000, 0x0204081000408000},
                  SquareRays{0x0000000040800000, 0x0408100000000000, 0x0408100040800000},
                  SquareRays{0x0000004080000000, 0x0810000000000000, 0x0810004080000000},
                  SquareRays{0x0000408000000000, 0x1000000000000000, 0x1000408000000000},
                  SquareRays{0x0040800000000000, 0x0000000000000000, 0x0040800000000000}},
                 {SquareRays{0x0000000000000000, 0x0000000102040800, 0x0000000102040800},
                  SquareRays{0x0000000000000020, 0x0000010204080000, 0x0000010204080020},
                  SquareRays{0x0000000000002040, 0x0001020408000000, 0x0001020408002040},
                  SquareRays{0x0000000000204080, 0x0102040800000000, 0x0102040800204080},
                  SquareRays{0x0000000020408000, 0x0204080000000000, 0x0204080020408000},
                  SquareRays{0x0000002040800000, 0x0408000000000000, 0x0408002040800000},
                  SquareRays{0x0000204080000000, 0x0800000000000000, 0x0800204080000000},
                  SquareRays{0x0020408000000000, 0x0000000000000000, 0x0020408000000000}},
                 {SquareRays{0x0000000000000000, 0x0000000001020400, 0x0000000001020400},
                  SquareRays{0x0000000000000010, 0x0000000102040000, 0x0000000102040010},
                  SquareRays{0x0000000000001020, 0x0000010204000000, 0x0000010204001020},
                  SquareRays{0x0000000000102040, 0x0001020400000000, 0x0001020400102040},
                  SquareRays{0x0000000010204080, 0x0102040000000000, 0x0102040010204080},
                  SquareRays{0x0000001020408000, 0x0204000000000000, 0x0204001020408000},
                  SquareRays{0x0000102040800000, 0x0400000000000000, 0x0400102040800000},
                  SquareRays{0x0010204080000000, 0x0000000000000000, 0x0010204080000000}},
                 {SquareRays{0x0000000000000000, 0x0000000000010200, 0x0000000000010200},
                  SquareRays{0x0000000000000008, 0x0000000001020000, 0x0000000001020008},
                  SquareRays{0x0000000000000810, 0x0000000102000000, 0x0000000102000810},
                  SquareRays{0x0000000000081020, 0x0000010200000000, 0x0000010200081020},
                  SquareRays{0x0000000008102040, 0x0001020000000000, 0x0001020008102040},
                  SquareRays{0x0000000810204080, 0x0102000000000000, 0x0102000810204080},
                  SquareRays{0x0000081020408000, 0x0200000000000000, 0x0200081020408000},
                  SquareRays{0x0008102040800000, 0x0000000000000000, 0x0008102040800000}},
                 {SquareRays{0x0000000000000000, 0x0000000000000100, 0x0000000000000100},
                  SquareRays{0x0000000000000004, 0x0000000000010000, 0x0000000000010004},
                  SquareRays{0x0000000000000408, 0x0000000001000000, 0x0000000001000408},
                  SquareRays{0x0000000000040810, 0x0000000100000000, 0x0000000100040810},
                  SquareRays{0x0000000004081020, 0x0000010000000000, 0x0000010004081020},
                  SquareRays{0x0000000408102040, 0x0001000000000000, 0x0001000408102040},
                  SquareRays{0x0000040810204080, 0x0100000000000000, 0x0100040810204080},
                  SquareRays{0x0004081020408000, 0x0000000000000000, 0x0004081020408000}},
                 {SquareRays{0x0000000000000000, 0x0000000000000000, 0x0000000000000000},
                  SquareRays{0x0000000000000002, 0x0000000000000000, 0x0000000000000002},
                  SquareRays{0x0000000000000204, 0x0000000000000000, 0x0000000000000204},
                  SquareRays{0x0000000000020408, 0x0000000000000000, 0x0000000000020408},
                  SquareRays{0x0000000002040810, 0x0000000000000000, 0x0000000002040810},
                  SquareRays{0x0000000204081020, 0x0000000000000000, 0x0000000204081020},
                  SquareRays{0x0000020408102040, 0x0000000000000000, 0x0000020408102040},
                  SquareRays{0x0002040810204080, 0x0000000000000000, 0x0002040810204080}}}};

        static constexpr const std::array<const std::array<const SquareRays, 8>, 8> s_antidiagonalRays{
                {{SquareRays{0x0000000000000000, 0x0000000000000000, 0x0000000000000000},
                  SquareRays{0x0000000000000040, 0x0000000000000000, 0x0000000000000040},
                  SquareRays{0x0000000000004020, 0x0000000000000000, 0x0000000000004020},
                  SquareRays{0x0000000000402010, 0x0000000000000000, 0x0000000000402010},
                  SquareRays{0x0000000040201008, 0x0000000000000000, 0x0000000040201008},
                  SquareRays{0x0000004020100804, 0x0000000000000000, 0x0000004020100804},
                  SquareRays{0x0000402010080402, 0x0000000000000000, 0x0000402010080402},
                  SquareRays{0x0040201008040201, 0x0000000000000000, 0x0040201008040201}},
                 {SquareRays{0x0000000000000000, 0x0000000000008000, 0x0000000000008000},
                  SquareRays{0x0000000000000020, 0x0000000000800000, 0x0000000000800020},
                  SquareRays{0x0000000000002010, 0x0000000080000000, 0x0000000080002010},
                  SquareRays{0x0000000000201008, 0x0000008000000000, 0x0000008000201008},
                  SquareRays{0x0000000020100804, 0x0000800000000000, 0x0000800020100804},
                  SquareRays{0x0000002010080402, 0x0080000000000000, 0x0080002010080402},
                  SquareRays{0x0000201008040201, 0x8000000000000000, 0x8000201008040201},
                  SquareRays{0x0020100804020100, 0x0000000000000000, 0x0020100804020100}},
                 {SquareRays{0x0000000000000000, 0x0000000000804000, 0x0000000000804000},
                  SquareRays{0x0000000000000010, 0x0000000080400000, 0x0000000080400010},
                  SquareRays{0x0000000000001008, 0x0000008040000000, 0x0000008040001008},
                  SquareRays{0x0000000000100804, 0x0000804000000000, 0x0000804000100804},
                  SquareRays{0x0000000010080402, 0x0080400000000000, 0x0080400010080402},
                  SquareRays{0x0000001008040201, 0x8040000000000000, 0x8040001008040201},
                  SquareRays{0x0000100804020100, 0x4000000000000000, 0x4000100804020100},
                  SquareRays{0x0010080402010000, 0x0000000000000000, 0x0010080402010000}},
                 {SquareRays{0x0000000000000000, 0x0000000080402000, 0x0000000080402000},
                  SquareRays{0x0000000000000008, 0x0000008040200000, 0x0000008040200008},
                  SquareRays{0x0000000000000804, 0x0000804020000000, 0x0000804020000804},
                  SquareRays{0x0000000000080402, 0x0080402000000000, 0x0080402000080402},
                  SquareRays{0x0000000008040201, 0x8040200000000000, 0x8040200008040201},
                  SquareRays{0x0000000804020100, 0x4020000000000000, 0x4020000804020100},
                  SquareRays{0x0000080402010000, 0x2000000000000000, 0x2000080402010000},
                  SquareRays{0x0008040201000000, 0x0000000000000000, 0x0008040201000000}},
                 {SquareRays{0x0000000000000000, 0x0000008040201000, 0x0000008040201000},
                  SquareRays{0x0000000000000004, 0x0000804020100000, 0x0000804020100004},
                  SquareRays{0x0000000000000402, 0x0080402010000000, 0x0080402010000402},
                  SquareRays{0x0000000000040201, 0x8040201000000000, 0x8040201000040201},
                  SquareRays{0x0000000004020100, 0x4020100000000000, 0x4020100004020100},
                  SquareRays{0x0000000402010000, 0x2010000000000000, 0x2010000402010000},
                  SquareRays{0x0000040201000000, 0x1000000000000000, 0x1000040201000000},
                  SquareRays{0x0004020100000000, 0x0000000000000000, 0x0004020100000000}},
                 {SquareRays{0x0000000000000000, 0x0000804020100800, 0x0000804020100800},
                  SquareRays{0x0000000000000002, 0x0080402010080000, 0x0080402010080002},
                  SquareRays{0x0000000000000201, 0x8040201008000000, 0x8040201008000201},
                  SquareRays{0x0000000000020100, 0x4020100800000000, 0x4020100800020100},
                  SquareRays{0x0000000002010000, 0x2010080000000000, 0x2010080002010000},
                  SquareRays{0x0000000201000000, 0x1008000000000000, 0x1008000201000000},
                  SquareRays{0x0000020100000000, 0x0800000000000000, 0x0800020100000000},
                  SquareRays{0x0002010000000000, 0x0000000000000000, 0x0002010000000000}},
                 {SquareRays{0x0000000000000000, 0x0080402010080400, 0x0080402010080400},
                  SquareRays{0x0000000000000001, 0x8040201008040000, 0x8040201008040001},
                  SquareRays{0x0000000000000100, 0x4020100804000000, 0x4020100804000100},
                  SquareRays{0x0000000000010000, 0x2010080400000000, 0x2010080400010000},
                  SquareRays{0x0000000001000000, 0x1008040000000000, 0x1008040001000000},
                  SquareRays{0x0000000100000000, 0x0804000000000000, 0x0804000100000000},
                  SquareRays{0x0000010000000000, 0x0400000000000000, 0x0400010000000000},
                  SquareRays{0x0001000000000000, 0x0000000000000000, 0x0001000000000000}},
                 {SquareRays{0x0000000000000000, 0x8040201008040200, 0x8040201008040200},
                  SquareRays{0x0000000000000000, 0x4020100804020000, 0x4020100804020000},
                  SquareRays{0x0000000000000000, 0x2010080402000000, 0x2010080402000000},
                  SquareRays{0x0000000000000000, 0x1008040200000000, 0x1008040200000000},
                  SquareRays{0x0000000000000000, 0x0804020000000000, 0x0804020000000000},
                  SquareRays{0x0000000000000000, 0x0402000000000000, 0x0402000000000000},
                  SquareRays{0x0000000000000000, 0x0200000000000000, 0x0200000000000000},
                  SquareRays{0x0000000000000000, 0x0000000000000000, 0x0000000000000000}}}};
    public:
        constexpr Board() noexcept: m_bitboards(s_startingPosition) {}
        explicit constexpr Board(const std::string_view &fenString) { set(fenString); }

        [[nodiscard]] std::string fen() const
        {
            auto ss = std::ostringstream();
            int emptyCount = 0;
            for (auto idx = 63; idx >= 0; idx--)
            {
                const auto p = piece(square(idx));

                const bool empty = p == Piece::None;
                emptyCount += empty;

                const bool endRank = idx % 8 == 0;
                const bool printEmpty = ((!empty || endRank) && emptyCount > 0);

                if (printEmpty)
                {
                    ss << emptyCount;
                    emptyCount = 0;
                }

                if (!empty)
                {
                    ss << s_pieceChars[p];
                }

                if (endRank && idx != 0)
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

        constexpr void set(const std::string_view &fenString)
        {
            m_bitboards = std::array<bitboard_t, 15>{};
            int sqr = 63;
            size_t fenIdx = 0;
            while (fenString[fenIdx] != ' ')
            {
                switch (fenString[fenIdx++])
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

            m_bitboards[Color::White] =
                    m_bitboards[Piece::WPawn] | m_bitboards[Piece::WRook] | m_bitboards[Piece::WKnight] | m_bitboards[Piece::WBishop] |
                    m_bitboards[Piece::WQueen] | m_bitboards[Piece::WKing];

            m_bitboards[Color::Black] =
                    m_bitboards[Piece::BPawn] | m_bitboards[Piece::BRook] | m_bitboards[Piece::BKnight] | m_bitboards[Piece::BBishop] |
                    m_bitboards[Piece::BQueen] | m_bitboards[Piece::BKing];

            m_bitboards[Piece::None] = ~m_bitboards[Color::White] & ~m_bitboards[Color::Black];

            m_turn = fenString[++fenIdx] == 'w' ? Color::White : Color::Black;

            // TODO: Castling rights
            // TODO: En passant square
            // TODO: Half-move clock
            // TODO: Full-move number
        }

        constexpr bool move(const std::string_view &uciMove) noexcept
        {
            const auto fromFile = charFile(uciMove[0]);
            const auto fromRank = charRank(uciMove[1]);
            const auto toFile = charFile(uciMove[2]);
            const auto toRank = charRank(uciMove[3]);

            const bool moved = (m_turn == Color::White) ? moveHelper<Color::White>(fromFile, fromRank, toFile, toRank, uciMove)
                                                        : moveHelper<Color::Black>(fromFile, fromRank, toFile, toRank, uciMove);

            if (moved) { m_turn = ~m_turn; }

            return moved;
        }
    };
} // namespace chess

#endif // CHESS_ENGINE_BOARD_H
