#pragma once

#ifdef _MSC_VER
#error "Not supported on MSVC"
#endif

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#warning "Not sure how this will work on big endian"
#endif

#if !defined(__GNUC__) && !defined(__clang__)
#error "This compiler is not supported (expected GCC or Clang)"
#endif

#include <array>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>

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

constexpr u32 NIBBLE_LO = ~0 / 0xFU;
inline constexpr char ANSI_RESET[] = "\x1b[0m";
inline constexpr char ANSI_WHITE[] = "\x1b[38;2;255;255;255m";
inline constexpr char ANSI_BLACK[] = "\x1b[38;2;0;0;0m";
inline constexpr char ANSI_EMPTY[] = "\x1b[38;2;50;50;50m";
constexpr u32 ALL_BLACK_PAWNS = 0x02020202U;
constexpr u32 ALL_WHITE_PAWNS = ALL_BLACK_PAWNS + NIBBLE_LO;

constexpr u8 piece_bits(Piece piece) { return static_cast<u8>(piece); }

constexpr Piece piece_from_bits(u8 bits) {
  switch (bits) {
  case 0:
    return Piece::Empty;
  case 2:
    return Piece::BlackPawn;
  case 4:
    return Piece::BlackBishop;
  case 6:
    return Piece::BlackKnight;
  case 8:
    return Piece::BlackCastle;
  case 10:
    return Piece::BlackQueen;
  case 12:
    return Piece::BlackKing;
  case 3:
    return Piece::WhitePawn;
  case 5:
    return Piece::WhiteBishop;
  case 7:
    return Piece::WhiteKnight;
  case 9:
    return Piece::WhiteCastle;
  case 11:
    return Piece::WhiteQueen;
  case 13:
    return Piece::WhiteKing;
  default:
    // TODO std::unreachable
    throw std::out_of_range("piece bits must encode a valid piece");
  }
}

constexpr Piece color_swapped(Piece piece) {
  switch (piece) {
  case Piece::Empty:
    return Piece::Empty;
  case Piece::BlackPawn:
    return Piece::WhitePawn;
  case Piece::BlackBishop:
    return Piece::WhiteBishop;
  case Piece::BlackKnight:
    return Piece::WhiteKnight;
  case Piece::BlackCastle:
    return Piece::WhiteCastle;
  case Piece::BlackQueen:
    return Piece::WhiteQueen;
  case Piece::BlackKing:
    return Piece::WhiteKing;
  case Piece::WhitePawn:
    return Piece::BlackPawn;
  case Piece::WhiteBishop:
    return Piece::BlackBishop;
  case Piece::WhiteKnight:
    return Piece::BlackKnight;
  case Piece::WhiteCastle:
    return Piece::BlackCastle;
  case Piece::WhiteQueen:
    return Piece::BlackQueen;
  case Piece::WhiteKing:
    return Piece::BlackKing;
  }

  throw std::out_of_range("piece must be valid");
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
  switch (piece) {
  case Piece::Empty:
    return ANSI_EMPTY;
  case Piece::BlackPawn:
  case Piece::BlackBishop:
  case Piece::BlackKnight:
  case Piece::BlackCastle:
  case Piece::BlackQueen:
  case Piece::BlackKing:
    return ANSI_BLACK;
  case Piece::WhitePawn:
  case Piece::WhiteBishop:
  case Piece::WhiteKnight:
  case Piece::WhiteCastle:
  case Piece::WhiteQueen:
  case Piece::WhiteKing:
    return ANSI_WHITE;
  }

  throw std::out_of_range("piece must be valid");
}

struct BoardRow {
  u32 bits{};

  constexpr explicit BoardRow(u32 value = 0) : bits(value) {}

  [[nodiscard]] constexpr u32 raw_bits() const { return bits; }

  [[nodiscard]] constexpr Piece piece_at(u32 index) const {
    if (index > 7) {
      throw std::out_of_range("row index must be in 0..=7");
    }

    const u32 shift = 28 - (index * 4);
    return piece_from_bits(static_cast<u8>((bits >> shift) & 0xFU));
  }

  [[nodiscard]] constexpr BoardRow mirrored() const {
    u32 mirrored_bits = 0;

    for (u32 index = 0; index < 8; ++index) {
      mirrored_bits |= static_cast<u32>(piece_bits(piece_at(index)))
                       << (index * 4);
    }

    return BoardRow{mirrored_bits};
  }

  [[nodiscard]] constexpr BoardRow color_swapped() const {
    u32 swapped_bits = 0;

    for (u32 index = 0; index < 8; ++index) {
      const u32 shift = 28 - (index * 4);
      swapped_bits |= static_cast<u32>(piece_bits(
                          bit_chess_cpp::color_swapped(piece_at(index))))
                      << shift;
    }

    return BoardRow{swapped_bits};
  }
};

constexpr u32 make_row(std::array<Piece, 8> left_to_right) {
  u32 row = 0;

  for (u32 index = 0; index < 8; ++index) {
    const u32 shift = 28 - (index * 4);
    row |= static_cast<u32>(piece_bits(left_to_right[index])) << shift;
  }

  return row;
}

constexpr u32 BLACK_ROW_START = make_row({
    Piece::BlackCastle,
    Piece::BlackKnight,
    Piece::BlackBishop,
    Piece::BlackQueen,
    Piece::BlackKing,
    Piece::BlackBishop,
    Piece::BlackKnight,
    Piece::BlackCastle,
});
//  dirty bit tricks...
constexpr u32 WHITE_ROW_START = BLACK_ROW_START + NIBBLE_LO;
constexpr u32 EMPTY_ROW = 0;
constexpr u32 BLACK_PAWN_ROW = EMPTY_ROW + 2 * NIBBLE_LO;
constexpr u32 WHITE_PAWN_ROW = BLACK_PAWN_ROW + NIBBLE_LO;

constexpr u128 pack_rows(u32 row0, u32 row1, u32 row2, u32 row3) {
  return (static_cast<u128>(row0) << 96) | (static_cast<u128>(row1) << 64) |
         (static_cast<u128>(row2) << 32) | static_cast<u128>(row3);
}

constexpr u32 unpack_row(u128 rows, u32 index) {
  if (index > 3) {
    // TODO use unreachable
    throw std::out_of_range("packed row index must be in 0..=3");
  }

  const u32 shift = 96 - (index * 32);
  return static_cast<u32>((rows >> shift) & static_cast<u128>(UINT32_MAX));
}

class ChessBoard {
public:
  constexpr ChessBoard()
      : rows0123_(
            pack_rows(BLACK_ROW_START, BLACK_PAWN_ROW, EMPTY_ROW, EMPTY_ROW)),
        rows4567_(
            pack_rows(EMPTY_ROW, EMPTY_ROW, WHITE_PAWN_ROW, WHITE_ROW_START)) {}

  [[nodiscard]] constexpr BoardRow row(u32 index) const {
    switch (index) {
    case 0:
      return BoardRow{unpack_row(rows0123_, 0)};
    case 1:
      return BoardRow{unpack_row(rows0123_, 1)};
    case 2:
      return BoardRow{unpack_row(rows0123_, 2)};
    case 3:
      return BoardRow{unpack_row(rows0123_, 3)};
    case 4:
      return BoardRow{unpack_row(rows4567_, 0)};
    case 5:
      return BoardRow{unpack_row(rows4567_, 1)};
    case 6:
      return BoardRow{unpack_row(rows4567_, 2)};
    case 7:
      return BoardRow{unpack_row(rows4567_, 3)};
    default:
      throw std::out_of_range("row index must be in 0..=7");
    }
  }

  [[nodiscard]] constexpr BoardRow mirrored_row(u32 index) const {
    return row(index).mirrored();
  }

  [[nodiscard]] constexpr BoardRow color_swapped_row(u32 index) const {
    return row(index).color_swapped();
  }

  [[nodiscard]] constexpr std::array<BoardRow, 8> rows() const {
    return {row(0), row(1), row(2), row(3), row(4), row(5), row(6), row(7)};
  }

  void print(std::ostream &stream = std::cout) const {
    stream << to_ansi_string();
  }

  [[nodiscard]] std::string to_string() const {
    std::string out;

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