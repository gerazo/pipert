#include "pipert/LogEventTypeRegistrar.h"

#include <cassert>

namespace pipert {

int LogEventTypeRegistrar::GetRegisterLength() { return register_length_; }

const char* LogEventTypeRegistrar::GetLogEventTypeID(int i) {
  assert(i < register_length_);
  return register_ids_[i];
}

int LogEventTypeRegistrar::register_length_ = 0;

const char*
    LogEventTypeRegistrar::register_ids_[kLogEventTypeRegisterMaxLength];

LogEventTypeRegistrar::LogEventTypeRegistrar(const char* type_id) {
  assert(register_length_ < kLogEventTypeRegisterMaxLength);
  register_ids_[register_length_] = type_id;
  register_length_++;
}

}  // namespace pipert
