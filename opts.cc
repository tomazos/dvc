#include "dvc/opts.h"

#include <map>
#include <mutex>

#include "dvc/parser.h"

namespace dvc {

namespace {

struct Options {
  std::vector<option*> raw_options;
  std::mutex mu;
  option& get_option(const std::string& name) {
    option* o = options_by_name[name];
    if (o == nullptr) DVC_FAIL("unknown option '", name, "'");
    return *o;
  }

  option& get_option_letter(char c) {
    option* o = options_by_letter[c];
    if (o == nullptr) DVC_FAIL("unknown option -", c);
    return *o;
  }

  std::map<std::string, option*> options_by_name;
  std::map<char, option*> options_by_letter;

  void compile() {
    std::lock_guard lock(mu);
    for (option* o : raw_options) {
      {
        auto& obn = options_by_name[o->name];
        DVC_ASSERT(obn == nullptr, "Two options by same name '", o->name,
                   "': (1) ", obn->to_string(), "; (2) ", o->to_string());
        obn = o;
      }
      if (o->letter) {
        auto& obl = options_by_letter[o->letter.value()];
        DVC_ASSERT(obl == nullptr, "Two options by same letter '",
                   o->letter.value(), "': (1) ", obl->to_string(), "; (2) ",
                   o->to_string());
        obl = o;
      }
    }
  }

  void check_required() {
    for (option* o : raw_options) o->check_required();
  }
};

Options& options() {
  static Options o;
  return o;
}

struct Char {
  int val;

  char to_char() const { return char(uint8_t(val)); }

  bool operator==(char c) const { return val < 256 && c == to_char(); }
  bool operator!=(char c) const { return !(*this == c); }
  bool operator==(const Char& c) const { return val == c.val; }
  bool operator!=(const Char& c) const { return val != c.val; }

  bool eoa() const { return to_char() == 0; }
  bool eof() const { return val == -1; }
};

struct ArgParser : dvc::parser<Char> {
  ArgParser(std::vector<Char> tokens)
      : dvc::parser<Char>("command-line arguments", std::move(tokens)) {}

  bool literal_mode = false;

  std::string parse_arg() {
    std::ostringstream oss;
    while (true) {
      Char c = pop();
      if (c.eoa()) return oss.str();
      oss.put(c.to_char());
    }
  }

  void parse_double_dash() {
    literal_mode = true;
    incr(3);
  }

  void parse_long_option() {
    incr(2);

    bool eq;

    std::ostringstream oss;
    while (true) {
      Char c = pop();
      if (c.eoa()) {
        eq = false;
        break;
      }
      if (c == '=') {
        eq = true;
        break;
      }
      oss.put(c.to_char());
    }

    std::string option_name = oss.str();
    option& o = options().get_option(option_name);
    if (eq && o.is_bool()) DVC_FAIL("Argument given to bool option: ", o.name);

    parse_option_args(o);
  }

  void parse_short_options() {
    incr(1);
    bool done = false;
    do {
      Char c = pop();
      if (peek().eoa()) {
        incr();
        done = true;
      }
      if (!parse_option_args(options().get_option_letter(c.to_char()))) return;
    } while (!done);
  }

  bool parse_option_args(option& o) {
    if (o.is_bool()) {
      o.set_value("1");
      return true;
    } else if (o.is_scalar()) {
      if (peek().eof()) DVC_FAIL("Expected argument for: ", o.name);
      o.set_value(parse_arg());
      return false;
    } else if (o.is_vector()) {
      if (peek().eof()) DVC_FAIL("Expected arguments for: ", o.name);
      do {
        o.add_value(parse_arg());
      } while (continue_vecargs());
      return false;
    } else {
      DVC_FATAL();
    }
  }

  bool continue_vecargs() {
    if (peek(0).eof()) return false;

    if (peek(0) != '-') return true;

    if (peek(1) == '-') return false;

    if (peek(1).eoa()) return true;

    if (std::isalpha(peek(1).to_char())) return false;

    return true;
  }

  void parse() {
    while (true) {
      if (peek(0).eof()) return;

      if (literal_mode)
        dvc::args.push_back(parse_arg());
      else if (peek(0) == '-') {
        if (peek(1) == '-') {
          if (peek(2).eoa())
            parse_double_dash();
          else
            parse_long_option();
        } else if (!peek(1).eoa() && std::isalpha(peek(1).to_char())) {
          parse_short_options();
        } else {
          dvc::args.push_back(parse_arg());
        }
      } else {
        dvc::args.push_back(parse_arg());
      }
    }
  }
};

}  // namespace

void register_option(option* o) {
  std::lock_guard lock(options().mu);
  options().raw_options.push_back(o);
}

std::string program_name;
std::vector<std::string> args;

bool DVC_OPTION(help, h, false, "list program options");

void init_options(int argc, char** argv) {
  static bool called = false;
  static std::mutex mu;
  std::lock_guard lock(mu);
  DVC_ASSERT(!called, "init_options called twice");
  called = true;

  options().compile();

  program_name = argv[0];

  std::vector<Char> tokens;

  for (char** s = argv + 1; s != argv + argc; s++) {
    for (size_t i = 0; true; i++) {
      tokens.push_back({int(uint8_t((*s)[i]))});
      if ((*s)[i] == 0) break;
    }
  }
  tokens.push_back({-1});

  ArgParser arg_parser(std::move(tokens));
  arg_parser.parse();

  options().check_required();

  if (help) {
    std::cout << dvc::program_name << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    for (option* o : options().raw_options) {
      std::cout << o->help_string();
    }
    std::exit(EXIT_FAILURE);
  }
}

}  // namespace dvc
