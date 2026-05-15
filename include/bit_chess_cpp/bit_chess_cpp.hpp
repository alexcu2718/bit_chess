#pragma once

#ifdef _MSC_VER
#error "Not supported on MSVC due to lacking __uint128_T"
#endif

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#warning "Not sure how this will work on big endian"
#endif

#if !defined(__GNUC__) && !defined(__clang__)
#warning "READ THIS\NThis compiler is not supported (expected GCC or Clang)"
#endif

#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
namespace bit_chess_cpp {

using u8 = uint8_t;
using u32 = uint32_t;
using u128 = __uint128_t;

enum class Piece : u8 {
  Empty = 0,
  BlackPawn = 2,
  BlackBishop = 4,
  BlackKnight = 6,
  BlackCastle = 8,
  BlackQueen = 10,
  BlackKing = 12,
  WhitePawn = 3,
  WhiteBishop = 5,
  WhiteKnight = 7,
  WhiteCastle = 9,
  WhiteQueen = 11,
  WhiteKing = 13,
};

inline constexpr char ANSI_RESET[] = "\x1b[0m";
inline constexpr char ANSI_WHITE[] = "\x1b[38;2;255;255;255m";
inline constexpr char ANSI_BLACK[] = "\x1b[38;2;0;0;0m";
inline constexpr char ANSI_EMPTY[] = "\x1b[38;2;50;50;50m";

/// Very unsafe function beware
constexpr Piece piece_from_bits(u8 bits) { return static_cast<Piece>(bits); }

constexpr Piece color_swapped(Piece piece) {
  switch (piece) {
  case Piece::Empty:
    return Piece::Empty;
  default: {
    const u8 piece_value = std::to_underlying(piece);
    const int delta = (piece_value & 1U) == 0 ? 1 : -1; // should be branchless
    return static_cast<Piece>(piece_value + delta);
  }
  }
}

constexpr char piece_to_char(Piece piece) {
  switch (piece) {
  case Piece::Empty:
    return '.';
  case Piece::BlackPawn:
    return 'p';
  case Piece::BlackBishop:
    return 'b';
  case Piece::BlackKnight:
    return 'n';
  case Piece::BlackCastle:
    return 'r';
  case Piece::BlackQueen:
    return 'q';
  case Piece::BlackKing:
    return 'k';
  case Piece::WhitePawn:
    return 'P';
  case Piece::WhiteBishop:
    return 'B';
  case Piece::WhiteKnight:
    return 'N';
  case Piece::WhiteCastle:
    return 'R';
  case Piece::WhiteQueen:
    return 'Q';
  case Piece::WhiteKing:
    return 'K';
  }

  throw std::out_of_range("piece must be valid");
}

constexpr const char *piece_to_color(Piece piece) {

  if (piece == Piece::Empty) {
    return ANSI_EMPTY;
  }

  if ((std::to_underlying(piece) & 1) == 0) {
    return ANSI_BLACK;
  }

  return ANSI_WHITE;
}

struct BoardRow {
  u32 bits{};

  constexpr explicit BoardRow(u32 value = 0) : bits(value) {}

  [[nodiscard]] constexpr u32 raw_bits() const { return bits; }

  [[nodiscard]] constexpr Piece piece_at(u32 index) const {
    assert(index <= 7);

    const u32 shift = 28 - (index * 4);
    return piece_from_bits(static_cast<u8>((bits >> shift) & 0xFU));
  }

  [[nodiscard]] constexpr BoardRow mirrored() const {
    u32 mirrored_bits = 0;

    for (u32 index = 0; index < 8; ++index) {
      mirrored_bits |= static_cast<u32>(std::to_underlying(piece_at(index)))
                       << (index * 4);
    }

    return BoardRow{mirrored_bits};
  }

  [[nodiscard]] constexpr BoardRow color_swapped() const {
    u32 swapped_bits = 0;

    for (u32 index = 0; index < 8; ++index) {
      const u32 shift = 28 - (index * 4);
      swapped_bits |= static_cast<u32>(std::to_underlying(
                          bit_chess_cpp::color_swapped(piece_at(index))))
                      << shift;
    }

    return BoardRow{swapped_bits};
  }
};
// trivially copyable
constexpr u32 make_row(std::array<Piece, 8> left_to_right) {
  u32 row = 0;

  for (u32 index = 0; index < 8; ++index) {
    const u32 shift = 28 - (index * 4);
    row |= static_cast<u32>(std::to_underlying(left_to_right[index])) << shift;
  }

  return row;
}

constexpr u32 BLACK_ROW_START = make_row({
    Piece::BlackCastle,
    Piece::BlackKnight,
    Piece::BlackBishop,
    Piece::BlackKing,
    Piece::BlackQueen,
    Piece::BlackBishop,
    Piece::BlackKnight,
    Piece::BlackCastle,
});

constexpr u32 WHITE_ROW_START = make_row({
    Piece::WhiteCastle,
    Piece::WhiteKnight,
    Piece::WhiteBishop,
    Piece::WhiteQueen,
    Piece::WhiteKing,
    Piece::WhiteBishop,
    Piece::WhiteKnight,
    Piece::WhiteCastle,
});

constexpr u32 NIBBLE_LO = ~0 / 0xFU; // has the LSB set of each nibble in a u32
//  dirty bit tricks...
constexpr u32 ALL_BLACK_PAWNS = 0x02020202U; // 000010001 repeated 4*
constexpr u32 ALL_WHITE_PAWNS = ALL_BLACK_PAWNS + NIBBLE_LO;

constexpr u32 EMPTY_ROW = 0;
constexpr u32 BLACK_PAWN_ROW = EMPTY_ROW + 2 * NIBBLE_LO;
constexpr u32 WHITE_PAWN_ROW = BLACK_PAWN_ROW + NIBBLE_LO;

constexpr u128 pack_rows(u32 row0, u32 row1, u32 row2, u32 row3) {
  return (static_cast<u128>(row0) << 96) | (static_cast<u128>(row1) << 64) |
         (static_cast<u128>(row2) << 32) | static_cast<u128>(row3);
}

constexpr u128 U32_MASK = static_cast<u128>(~u32{0});

constexpr u32
unpack_row(u128 rows,
           u32 index) { // TODO make index a newtype wrapper with basically
                        // wrapper semantics to ensure typesafety
  if (index > 3) {
    // TODO use unreachable

    throw std::out_of_range("packed row index must be in 0..=3");
  }

  const u32 shift = 96 - (index * 32);
  return static_cast<u32>((rows >> shift) & U32_MASK);
}

class ChessBoard {
public:
  constexpr ChessBoard()
      : rows0123_(
            pack_rows(BLACK_ROW_START, BLACK_PAWN_ROW, EMPTY_ROW, EMPTY_ROW)),
        rows4567_(
            pack_rows(EMPTY_ROW, EMPTY_ROW, WHITE_PAWN_ROW, WHITE_ROW_START)) {}

  [[nodiscard]] constexpr BoardRow row(u32 index) const & {
    if (index > 7) {
      throw std::out_of_range("row index must be in 0..=7");
    }

    const u128 packed_rows = index < 4 ? rows0123_ : rows4567_;
    return BoardRow{
        unpack_row(packed_rows, index % 4)}; // compiler will optimise modulo 4
    // eg will replace x % 4 with x & 3 (when x is unsigned or POSITIVE)
  }

  [[nodiscard]] constexpr BoardRow mirrored_row(u32 index) const & {
    return row(index).mirrored();
  }

  [[nodiscard]] constexpr BoardRow color_swapped_row(u32 index) const & {
    return row(index).color_swapped();
  }

  [[nodiscard]] constexpr std::array<BoardRow, 8> rows() const & {
    return {row(0), row(1), row(2), row(3), row(4), row(5), row(6), row(7)};
  }

  void print(std::ostream &stream = std::cout) const & {
    stream << to_ansi_string();
  }

  [[nodiscard]] std::string to_string() const {
    std::string out;
    constexpr size_t RESERVE_SIZE = 64 * 8;
    out.reserve(RESERVE_SIZE);

    for (size_t row_index = 0; row_index < 8; ++row_index) {
      const BoardRow current_row = row(static_cast<u32>(row_index));
      for (u32 cell_index = 0; cell_index < 8; ++cell_index) {
        out.push_back(piece_to_char(current_row.piece_at(cell_index)));
        if (cell_index < 7) {
          out.push_back(' ');
        }
      }

      if (row_index < 7) {
        out.push_back('\n');
      }
    }

    return out;
  }

  [[nodiscard]] std::string to_ansi_string() const {
    std::string out;
    constexpr size_t RESERVE_SIZE = 64 * 8;
    out.reserve(RESERVE_SIZE);

    for (size_t row_index = 0; row_index < 8; ++row_index) {
      const BoardRow current_row = row(static_cast<u32>(row_index));
      for (u32 cell_index = 0; cell_index < 8; ++cell_index) {
        const Piece piece = current_row.piece_at(cell_index);
        out += piece_to_color(piece);
        out.push_back(piece_to_char(piece));
        out += ANSI_RESET;

        if (cell_index < 7) {
          out.push_back(' ');
        }
      }

      if (row_index < 7) {
        out.push_back('\n');
      }
    }

    return out;
  }

private:
  u128 rows0123_{};
  u128 rows4567_{};
};

inline std::ostream &operator<<(std::ostream &stream, const ChessBoard &board) {
  return stream << board.to_string();
}

} // namespace bit_chess_cpp