#include "dvc/sha3.h"

#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "dvc/file.h"
#include "dvc/hex.h"
#include "dvc/log.h"

#define EXPECT_SHA(f, input, output)                                   \
  DVC_ASSERT_EQ(dvc::ByteArrayToHexString(f(std::string_view(input))), \
                dvc::ByteArrayToHexString(dvc::HexStringToByteArray(output)))

#define EXPECT_SHAKE(f, input, output_len, output)                       \
  DVC_ASSERT_EQ(                                                         \
      dvc::ByteArrayToHexString(f(std::string_view(input), output_len)), \
      dvc::ByteArrayToHexString(dvc::HexStringToByteArray(output)))

void sha3test_empty() {
  EXPECT_SHA(
      dvc::SHA3_256, "",
      "A7FFC6F8BF1ED76651C14756A061D662F580FF4DE43B49FA82D80A4B80F8434A");

  EXPECT_SHA(dvc::SHA3_224, "",
             "6b4e03423667dbb73b6e15454f0eb1abd4597f9a1b078e3f5b5a6bc7");

  EXPECT_SHA(
      dvc::SHA3_256, "",
      "a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a");
  EXPECT_SHA(dvc::SHA3_384, "",
             "0c63a75b845e4f7d01107d852e4c2485c51a50aaaa94fc61995e71bbee983a2ac"
             "3713831264adb47fb6bd1e058d5f004");

  EXPECT_SHA(dvc::SHA3_512, "",
             "a69f73cca23a9ac5c8b567dc185a756e97c982164fe25859e0d1dcc1475c80a61"
             "5b2123af1f5f94c11e3e9402c3ac558f500199d95b6d3e301758586281dcd26");

  EXPECT_SHAKE(
      dvc::SHAKE128, "", 256 / 8,
      "7f9c2ba4e88f827d616045507605853ed73b8093f6efbc88eb1a6eacfa66ef26");

  EXPECT_SHAKE(
      dvc::SHAKE256, "", 512 / 8,
      "46b9dd2b0ba88d13233b3feb743eeb243fcd52ea62b81b82b50c27646ed5762fd75dc4dd"
      "d8c0f200cb05019d67b592f6fc821c49479ab48640292eacb3b7c4be");

  EXPECT_SHAKE(
      dvc::SHAKE128, "The quick brown fox jumps over the lazy dog", 256 / 8,
      "f4202e3c5852f9182a0430fd8144f0a74b95e7417ecae17db0f8cfeed0e3e66e");

  EXPECT_SHAKE(
      dvc::SHAKE128, "The quick brown fox jumps over the lazy dof", 256 / 8,
      "853f4538be0db9621a6cea659a06c1107b1f83f02b13d18297bd39d7411cf10c");
}

using ByteArray = std::vector<std::byte>;

void ParseTestFile(
    std::filesystem::path testfile,
    std::function<void(std::map<std::string, std::string>)> on_entry) {
  std::string testdata = dvc::load_file(testfile);
  DVC_ASSERT_NE(testdata.size(), 0u);
  std::map<std::string, std::string> current_entry;
  for (const std::string line : dvc::split("\n", testdata)) {
    if (line[0] == '#')
      return;
    else if (line.empty()) {
      if (current_entry.empty()) return;

      auto entry = current_entry;
      current_entry.clear();
      on_entry(entry);
    } else {
      size_t pos = line.find(" = ");
      DVC_ASSERT_NE(pos, std::string::npos);
      current_entry[line.substr(0, pos)] = line.substr(pos + 3);
    }
  }

  if (!current_entry.empty()) on_entry(current_entry);
}

template <typename F>
void ShakeFileTest(F shake, std::filesystem::path testfile) {
  ParseTestFile(testfile, [&](std::map<std::string, std::string> entry) {
    DVC_ASSERT_EQ(entry.size(), 3u);
    DVC_ASSERT(entry.count("Len"));
    DVC_ASSERT(entry.count("Msg"));
    DVC_ASSERT(entry.count("Squeezed"));
    int len = std::stoi(entry["Len"]);
    if (len == 0u || len % 8 != 0u) return;
    auto msg = dvc::HexStringToByteArray(entry["Msg"]);
    auto squeezed = entry["Squeezed"];

    DVC_ASSERT_EQ(dvc::ByteArrayToHexString(shake(msg, squeezed.size() / 2)),
                  squeezed);
  });
}

template <typename F>
void ShaFileTest(F sha, std::filesystem::path testfile) {
  ParseTestFile(testfile, [&](std::map<std::string, std::string> entry) {
    DVC_ASSERT_EQ(entry.size(), 3u);
    DVC_ASSERT(entry.count("Len"));
    DVC_ASSERT(entry.count("Msg"));
    DVC_ASSERT(entry.count("MD"));
    int len = std::stoi(entry["Len"]);
    if (len == 0u || len % 8 != 0u) return;
    auto msg = dvc::HexStringToByteArray(entry["Msg"]);
    DVC_ASSERT_EQ(int(msg.size()), len / 8);

    DVC_ASSERT_EQ(dvc::ByteArrayToHexString(sha(msg)), entry["MD"]);
  });
}

void sha3test_files() {
  ShakeFileTest(
      [](const ByteArray& input, int len) { return dvc::SHAKE128(input, len); },
      "dvc/testdata/ShortMsgKAT_SHAKE128.txt");
  ShakeFileTest(
      [](const ByteArray& input, int len) { return dvc::SHAKE256(input, len); },
      "dvc/testdata/ShortMsgKAT_SHAKE256.txt");
  ShaFileTest([](const ByteArray& input) { return dvc::SHA3_224(input); },
              "dvc/testdata/ShortMsgKAT_SHA3-224.txt");
  ShaFileTest([](const ByteArray& input) { return dvc::SHA3_256(input); },
              "dvc/testdata/ShortMsgKAT_SHA3-256.txt");
  ShaFileTest([](const ByteArray& input) { return dvc::SHA3_384(input); },
              "dvc/testdata/ShortMsgKAT_SHA3-384.txt");
  ShaFileTest([](const ByteArray& input) { return dvc::SHA3_512(input); },
              "dvc/testdata/ShortMsgKAT_SHA3-512.txt");
}

int main() {
  sha3test_empty();

  sha3test_files();
}
