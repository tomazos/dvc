#pragma once

#include <string>
#include <vector>
#include <string_view>

#include "dvc/log.h"

namespace dvc {

inline std::vector<std::byte> HexStringToByteArray(std::string_view hex_string);
inline std::string ByteArrayToHexString(const std::byte* data, size_t size);
inline std::string ByteArrayToHexString(const std::vector<std::byte>& byte_array) {
  return ByteArrayToHexString(byte_array.data(), byte_array.size());
}

template <size_t size>
inline std::string ByteArrayToHexString(const std::array<std::byte, size>& byte_array) {
  return ByteArrayToHexString(byte_array.data(), size);
}

inline std::string ByteArrayToHexString(std::string_view byte_array) {
  return ByteArrayToHexString((std::byte*)byte_array.data(), byte_array.size());
}

inline int HexCharToInt(char hex_char) {
  switch (hex_char) {
    case '0':
      return 0;
    case '1':
      return 1;
    case '2':
      return 2;
    case '3':
      return 3;
    case '4':
      return 4;
    case '5':
      return 5;
    case '6':
      return 6;
    case '7':
      return 7;
    case '8':
      return 8;
    case '9':
      return 9;
    case 'A':
    case 'a':
      return 0xA;
    case 'B':
    case 'b':
      return 0xB;
    case 'C':
    case 'c':
      return 0xC;
    case 'D':
    case 'd':
      return 0xD;
    case 'E':
    case 'e':
      return 0xE;
    case 'F':
    case 'f':
      return 0xF;
    default:
      DVC_FATAL("invalid hex char #", int(hex_char));
  }
}

inline char IntToHexChar(int hex_int) {
  switch (hex_int) {
    case 0:
      return '0';
    case 1:
      return '1';
    case 2:
      return '2';
    case 3:
      return '3';
    case 4:
      return '4';
    case 5:
      return '5';
    case 6:
      return '6';
    case 7:
      return '7';
    case 8:
      return '8';
    case 9:
      return '9';
    case 0xA:
      return 'A';
    case 0xB:
      return 'B';
    case 0xC:
      return 'C';
    case 0xD:
      return 'D';
    case 0xE:
      return 'E';
    case 0xF:
      return 'F';
    default:
      DVC_FATAL("invalid hex int ", int(hex_int));
  }
}

inline std::byte HexCharPairToByte(std::pair<char, char> hex_pair) {
  return std::byte(HexCharToInt(hex_pair.first) << 4 | HexCharToInt(hex_pair.second) << 0);
}

inline std::pair<char, char> ByteToHexCharPair(std::byte byte) {
  return {IntToHexChar((uint8_t(byte) & 0xF0) >> 4), IntToHexChar((uint8_t(byte) & 0x0F) >> 0)};
}

inline std::vector<std::byte> HexStringToByteArray(std::string_view hex_string) {
  DVC_ASSERT_EQ(hex_string.size() % 2, 0u);
  const auto nbytes = hex_string.size() / 2;
  std::vector<std::byte> byte_array(nbytes);
  for (size_t i = 0; i < nbytes; i++) {
    byte_array[i] =
        HexCharPairToByte({hex_string[2 * i + 0], hex_string[2 * i + 1]});
  }
  return byte_array;
}

inline std::string ByteArrayToHexString(const std::byte* data, size_t size) {
  std::string hex_string(size * 2, '\0');
  for (size_t i = 0; i < size; ++i) {
    const auto char_pair = ByteToHexCharPair(data[i]);
    hex_string[2 * i + 0] = char_pair.first;
    hex_string[2 * i + 1] = char_pair.second;
  }
  return hex_string;
}

}  // namespace dvc

