// Copyright 2019 MongoDB Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <system_error>

#include <bsoncxx/config/prelude.hpp>

#include <bsoncxx/util/backtrace.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

///
/// Class representing any exceptions emitted from the bsoncxx library or
/// its underlying implementation.
///
class BSONCXX_API trace_exception : public std::system_error {
   public:
    trace_exception(std::error_code ec)
        : std::system_error(ec), _trace(bsoncxx::trace::backtrace()) {}
    trace_exception(std::error_code ec, std::string const& what_arg)
        : std::system_error(ec, what_arg), _trace(bsoncxx::trace::backtrace()) {}

    const char* trace() {
        return _trace.c_str();
    }

   private:
    std::string _trace;
};

BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
