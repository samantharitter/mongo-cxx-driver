// Copyright 2014 MongoDB Inc.
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

#include <string>
#include <system_error>

#include <mongocxx/config/prelude.hpp>

#include <bsoncxx/util/backtrace.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

using bsoncxx::trace::backtrace;

///
/// A class to be used as the base class for all mongocxx exceptions.
///
class MONGOCXX_API exception : public std::system_error {
 public:
    exception(std::error_code ec) : std::system_error(ec), _trace(backtrace()) {}
    exception(std::error_code ec, std::string const &what_arg) : std::system_error(ec, what_arg), _trace(backtrace()) {}

    const char *trace() const { return _trace.c_str(); }

 private:
    std::string _trace;
};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
