#pragma once

#include <optional>
#include <string>
#include <type_traits>
#include <vector>

#include "dvc/log.h"
#include "dvc/string.h"

// A namespace-scope definition of the form:
//
//    T DVC_OPTION(name, letter, default_value, description);
//
// where:
//
// - `T` is a type.  The type of the option.
//
// - `name` is an identifier.  The long name of the option.
//
// - `letter` is either: (a) a single-character identifier that is not
// underscore, in which case it is the short name of the option; or (b) `-`
// (hyphen/minus), in which case the option has no short name.
//
// - `default_value` is either: (a) an expression of type `T`, in which case it
// is the value of the option if not set on the command-line; or (b)
// `dvc::required` in which case the option must be set from the command-line
// and has no default value.
//
// - `description` is a string, the name of the option.
//
// creates a definition:
//
//    T name;
//
// and associates it with the command-line options:
//
//    --name
//    -letter
//
// If `T` is `bool`, then it is a bool option, and has no argument.  Bool
// options must have default_value false. Otherwise, if `T` is not a
// std::vector, then it is a scalar option, and has one argument. Otherwise, `T`
// is a std::vector, then it is a vector option, and has multiple arguments.
//

#define DVC_OPTION(name, letter, default_value, description)
#undef DVC_OPTION
#define DVC_OPTION(name, letter, ... /* default_value, description */) \
  name;                                                                \
  ::dvc::typed_option<decltype(name)> DVC_OPTION_##name(               \
      name, #name, #letter, __VA_ARGS__, __FILE__, __LINE__)

namespace dvc {

extern std::string program_name;
extern std::vector<std::string> args;

void init_options(int argc, char** argv);

constexpr struct required_t {
} required;

struct option;
void register_option(option*);

// BASE
struct option {
  std::string name;
  std::optional<char> letter;
  std::string description;
  bool required;
  std::string file;
  int line;
  bool set = false;

  option(const std::string& name, const std::string& letter,
         const std::string& description, const std::string& file, int line)
      : name(name),
        letter(translate_letter(letter)),
        description(description),
        file(file),
        line(line) {}

  virtual void set_value(const std::string&) = 0;
  virtual void add_value(const std::string&) = 0;
  virtual bool is_bool() const { return false; }
  virtual bool is_vector() const { return false; }
  virtual bool is_scalar() const { return false; }

  virtual std::string to_string() const = 0;

  std::optional<char> translate_letter(const std::string& letter) {
    DVC_ASSERT(letter.size() == 1, "Invalid letter in option '", name, "': '",
               letter, "', must be one character long.");
    char c = letter[0];
    if (c == '-')
      return std::nullopt;
    else
      return c;
  }

  void check_required() {
    if (required && !set)
      DVC_FAIL("Required option --", name, " not set: ", description);
  }

  virtual void write_help_args(std::ostream&) const = 0;
  virtual void write_default(std::ostream&) const = 0;

  std::string help_string() {
    std::ostringstream o;
    o << "  --" << name << " ";
    write_help_args(o);
    o << std::endl;
    if (letter) {
      o << "  -" << letter.value() << " ";
      write_help_args(o);
      o << std::endl;
    }
    o << std::endl;
    o << "    " << description;
    if (is_bool()) {
      o << std::endl;
    } else if (required)
      o << " (required)" << std::endl;
    else {
      o << " (default: ";
      write_default(o);
      o << ")" << std::endl;
    }
    o << std::endl;
    return o.str();
  }
};

template <typename T>
struct typed_option;

// BOOL
template <>
struct typed_option<bool> : option {
  bool is_bool() const override { return true; }

  bool& ref;
  typed_option(bool& ref, const std::string& name, const std::string& letter,
               bool default_value, const std::string& description,
               const std::string& file, int line)
      : option(name, letter, description, file, line), ref(ref) {
    DVC_ASSERT(default_value == false,
               "The default value of a bool option must be false: ", name);
    ref = false;
    required = false;
    register_option(this);
  }

  typed_option(bool& ref, const std::string& name, const std::string& letter,
               dvc::required_t, const std::string& description,
               const std::string& file, int line)
      : option(name, letter, description, file, line), ref(ref) {
    DVC_FATAL("bool options cannot be required: ", name);
  }

  void set_value(const std::string& string) override {
    if (string != "1")
      DVC_FATAL("set_value of not '1' called for bool option: ", name);
    if (set) DVC_FAIL("option ", name, " passed multiple times");
    set = true;
    ref = true;
  }

  void add_value(const std::string&) override {
    DVC_FATAL("unexpected add_value on scalar option: ", name);
  }

  std::string to_string() const override {
    return dvc::concat("[option ", name, " of type ", typeid(ref).name(),
                       " defined at ", file, ":", line, "]");
  }

  void write_help_args(std::ostream&) const override{};
  void write_default(std::ostream&) const override{};
};

// SCALAR
template <typename T>
struct typed_option : option {
  bool is_scalar() const override { return true; }

  using type = T;
  type& ref;
  type default_value;
  typed_option(type& ref, const std::string& name, const std::string& letter,
               dvc::required_t, const std::string& description,
               const std::string& file, int line)
      : option(name, letter, description, file, line), ref(ref) {
    required = true;
    register_option(this);
  }

  typed_option(type& ref, const std::string& name, const std::string& letter,
               type default_value, const std::string& description,
               const std::string& file, int line)
      : option(name, letter, description, file, line),
        ref(ref),
        default_value(default_value) {
    ref = default_value;
    required = false;
    register_option(this);
  }

  void set_value(const std::string& string) override {
    if (set) DVC_FAIL("option ", name, " passed multiple times");

    set = true;
    if (!dvc::destring(string, ref))
      DVC_FAIL("Unable to parse option '", name, "' of type '",
               typeid(type).name(), "' with input '", string, "'.");
  }

  void add_value(const std::string&) override {
    DVC_FATAL("unexpected add_value on scalar option: ", name);
  }

  std::string to_string() const override {
    return dvc::concat("[option ", name, " of type ", typeid(ref).name(),
                       " defined at ", file, ":", line, "]");
  }

  void write_help_args(std::ostream& o) const override { o << "<arg>"; };
  void write_default(std::ostream& o) const override { o << default_value; };
};

// VECTOR
template <typename E>
struct typed_option<std::vector<E>> : option {
  bool is_vector() const override { return true; }

  using type = std::vector<E>;
  type& ref;
  type default_value;

  typed_option(type& ref, const std::string& name, const std::string& letter,
               dvc::required_t, const std::string& description,
               const std::string& file, int line)
      : option(name, letter, description, file, line), ref(ref) {
    required = true;
    register_option(this);
  }

  typed_option(type& ref, const std::string& name, const std::string& letter,
               type default_value, const std::string& description,
               const std::string& file, int line)
      : option(name, letter, description, file, line),
        ref(ref),
        default_value(default_value) {
    ref = default_value;
    required = false;
    register_option(this);
  }

  void set_value(const std::string& string) override {
    DVC_FATAL("unexpected set_value on vector option: ", name, " = ", string);
  }

  void add_value(const std::string& string) override {
    if (!set) {
      ref.clear();
      set = true;
    }
    E e;
    if (!dvc::destring(string, e))
      DVC_FAIL("Unable to parse option '", name, "' of type '",
               typeid(E).name(), "' with input '", string, "'.");

    ref.push_back(e);
  }

  std::string to_string() const override {
    return dvc::concat("[option ", name, " of type ", typeid(ref).name(),
                       " defined at ", file, ":", line, "]");
  }

  void write_help_args(std::ostream& o) const override {
    o << "<arg1> <arg2> ... <argn>";
  };
  void write_default(std::ostream& o) const override {
    o << "[ ";
    for (auto v : default_value) o << v << " ";
    o << "]";
  }
};

}  // namespace dvc
