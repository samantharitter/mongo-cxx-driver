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

#include <bsoncxx/config/prelude.hpp>

#ifdef BSONCXX_ENABLE_STACK_TRACING
#define UNW_LOCAL_ONLY
#include <cxxabi.h>
#include <libunwind.h>
#endif  // BSONCXX_ENABLE_STACK_TRACING

#include <stdio.h>
#include <sstream>

#include <mongocxx/private/libbson.hh>

namespace bsoncxx {
namespace trace {

///
/// Returns a string representing the stack trace up from this point.
///
inline std::string backtrace() {
#ifdef BSONCXX_ENABLE_STACK_TRACING
    std::ostringstream stream;
    unw_cursor_t cursor;
    unw_context_t ctx;
    unw_word_t ip, sp, offset;
    int frame = 0;

    unw_getcontext(&ctx);
    unw_init_local(&cursor, &ctx);

    while (unw_step(&cursor) > 0) {
        unw_get_reg(&cursor, UNW_REG_IP, &ip);
        unw_get_reg(&cursor, UNW_REG_SP, &sp);

        if (ip == 0) {
            break;
        }

        stream << "frame " << frame << ")\t" << ip << ": ";

        char name[256];
        if (unw_get_proc_name(&cursor, name, sizeof(name), &offset) == 0) {
            char* nameptr = name;
            int status;
            char* demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
            if (status == 0) {
                nameptr = demangled;
            }

            stream << nameptr << "+0x" << offset << "\n";

            // TODO: get files and line numbers.

            bson_free(demangled);

        } else {
            stream << "unknown symbol name";
        }

        frame++;
    }

    return stream.str();

#else   // BSONCXX_ENABLE_STACK_TRACING
    return "Error: stack tracing not enabled\n";
#endif  // BSONCXX_ENABLE_STACK_TRACING
}

///
/// Prints a string representing the stack trace up from this point.
///
inline void print_backtrace() {
    auto trace = backtrace();
    printf("%s\n", trace.c_str());
}

}  // namespace trace
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
