#pragma once

#include <filesystem>
#include <fstream>

namespace dvc {

class file_reader {
 public:
  file_reader(const std::filesystem::path& fspath) {
    ifs.exceptions(std::ios::badbit | std::ios::failbit | std::ios::eofbit);
    ifs.open(fspath.string().c_str(), std::ios::binary | std::ios::in);
  }

  size_t size() {
    size_t pos = tell();
    ifs.seekg(0, std::ios::end);
    size_t len = tell();
    seek(pos);
    return len;
  }

  void read(void* buf, size_t n) { ifs.read((char*)buf, n); }

  std::string read_string(size_t n) {
    std::string s(n, '\0');
    read(s.data(), n);
    return s;
  }

  template <typename T>
  T rread() {
    T t;
    read(&t, sizeof(T));
    return t;
  }

  size_t vread() {
    size_t s = 0;
    for (int i = 0; true; i += 7) {
      uint8_t x = rread<uint8_t>();
      s |= size_t(x & 127u) << i;
      if (!(x & 128u)) break;
    }
    return s;
  }

  void seek(size_t pos) { ifs.seekg(pos); }

  size_t tell() { return ifs.tellg(); }

  std::ifstream& istream() { return ifs; }

 private:
  std::ifstream ifs;
};

constexpr struct append_t {
} append;
constexpr struct truncate_t {
} truncate;

class file_writer {
 public:
  file_writer(const std::filesystem::path& fspath, append_t) {
    open(fspath, std::ios::ate | std::ios::binary);
  }
  file_writer(const std::filesystem::path& fspath, truncate_t) {
    open(fspath, std::ios::trunc | std::ios::binary);
  }

  size_t tell() { return ofs.tellp(); }
  void seek(size_t pos) { ofs.seekp(pos, std::ios_base::beg); }

  template <typename T>
  void rwrite(T t) {
    write(&t, sizeof(T));
  }
  void vwrite(size_t s) {
    while (s > 127) {
      rwrite<uint8_t>((s & 127u) | 128u);
      s >>= 7;
    }
    rwrite<uint8_t>(s);
  }

  template <typename T>
  size_t prepare_backpatch() {
    size_t backpatch = tell();
    rwrite(T());
    return backpatch;
  }

  template <typename T>
  void write_backpatch(size_t backpatch, T t) {
    size_t pos = tell();
    seek(backpatch);
    rwrite(t);
    seek(pos);
  }

  void append_file(std::filesystem::path path) {
    file_reader reader(path);
    size_t remaining = reader.size();
    constexpr size_t bufsize = 1 << 14;
    char buf[bufsize];
    while (remaining > bufsize) {
      reader.read(buf, bufsize);
      write(buf, bufsize);
      remaining -= bufsize;
    }
    reader.read(buf, remaining);
    write(buf, remaining);
  }

  void write(const void* buf, size_t n) { ofs.write((const char*)buf, n); }
  void write(std::string_view sv) { write(sv.data(), sv.size()); }
  template <typename... Args>
  void print(Args&&... args) {
    (ofs << ... << std::forward<Args>(args));
  }
  void println() { ofs << std::endl; }
  template <typename... Args>
  void println(Args&&... args) {
    print(std::forward<Args>(args)...);
    ofs << std::endl;
  }

  std::ostream& ostream() { return ofs; }

 private:
  void open(const std::filesystem::path& fspath,
            std::ios::openmode mode_extra) {
    ofs.exceptions(std::ios::badbit | std::ios::failbit | std::ios::eofbit);
    ofs.open(fspath.string().c_str(),
             std::ios::binary | std::ios::out | mode_extra);
  }
  std::ofstream ofs;
};

inline void save_file(const std::filesystem::path& filename,
                      std::string_view data) {
  file_writer(filename, truncate).write(data);
}

inline std::string load_file(const std::filesystem::path& filename) {
  file_reader reader(filename);
  std::string s;
  s.resize(reader.size());
  reader.read(&s[0], s.size());
  return s;
}

inline void touch_file(const std::filesystem::path& filename) {
  file_writer(filename, append);
}

}  // namespace dvc
