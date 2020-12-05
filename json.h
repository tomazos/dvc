#pragma once

#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>

#include <memory>

#include "dvc/log.h"

namespace dvc {

class json_writer {
 public:
  json_writer(std::ostream& o)
      : ostream_wrapper(std::make_unique<rapidjson::OStreamWrapper>(o)),
        writer(*ostream_wrapper) {}

  void write_null() { DVC_ASSERT(writer.Null()); }

  void write_bool(bool b) { DVC_ASSERT(writer.Bool(b)); }

  void write_number(double d) { DVC_ASSERT(writer.Double(d)); }

  void write_string(std::string_view sv) {
    DVC_ASSERT(writer.String(sv.data(), sv.size()));
  }

  void write_key(std::string_view sv) {
    DVC_ASSERT(writer.Key(sv.data(), sv.size()));
  }

  void start_object() { DVC_ASSERT(writer.StartObject()); }

  void end_object() { DVC_ASSERT(writer.EndObject()); }

  void start_array() { DVC_ASSERT(writer.StartArray()); }

  void end_array() { DVC_ASSERT(writer.EndArray()); }

 private:
  std::unique_ptr<rapidjson::OStreamWrapper> ostream_wrapper;
  rapidjson::Writer<rapidjson::OStreamWrapper> writer;
};

}  // namespace dvc
