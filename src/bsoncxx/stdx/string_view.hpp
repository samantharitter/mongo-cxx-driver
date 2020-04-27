// Copyright 2015 MongoDB Inc.
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

#include <cstdlib>
#include <cstring>

#include <bsoncxx/config/prelude.hpp>

#if defined(BSONCXX_POLY_USE_MNMLSTC)

#if defined(MONGO_CXX_DRIVER_COMPILING) || defined(BSONCXX_POLY_USE_SYSTEM_MNMLSTC)
#include <core/string.hpp>
#else
#include <bsoncxx/third_party/mnmlstc/core/string.hpp>
#endif

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace stdx {

using ::core::basic_string_view;
using ::core::string_view;

}  // namespace stdx
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#elif defined(BSONCXX_POLY_USE_BOOST)

#include <boost/version.hpp>

#if BOOST_VERSION >= 106100

#include <boost/utility/string_view.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace stdx {

using ::boost::basic_string_view;
using ::boost::string_view;

}  // namespace stdx
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#else

#include <boost/utility/string_ref.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace stdx {

template <typename charT, typename traits = std::char_traits<charT>>
using basic_string_view = ::boost::basic_string_ref<charT, traits>;
using string_view = ::boost::string_ref;

}  // namespace stdx
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#endif

#elif defined(BSONCXX_POLY_USE_STD_EXPERIMENTAL)

#include <experimental/string_view>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace stdx {

using ::std::experimental::basic_string_view;
using ::std::experimental::string_view;

}  // namespace stdx
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#elif defined(BSONCXX_POLY_USE_STD)

#include <string_view>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace stdx {

using ::std::basic_string_view;
using ::std::string_view;

}  // namespace stdx
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#else
#error "Cannot find a valid polyfill for string_view"
#endif

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace stdx {

BSONCXX_INLINE bool null_blind_strcmp(stdx::string_view lhs, stdx::string_view rhs) {
    auto diff = std::abs((int)(lhs.length() - rhs.length()));

    if (diff == 0) {
        return lhs == rhs;
    }

    if (diff > 1) {
        return false;
    }

    stdx::string_view longer;
    stdx::string_view shorter;

    if (lhs.length() > rhs.length()) {
        longer = lhs;
        shorter = rhs;
    } else {
        longer = rhs;
        shorter = lhs;
    }

    if (longer.back() != '\0') {
        return false;
    }

    return (std::strncmp(shorter.data(), longer.data(), shorter.length()) == 0);
}
}  // namespace stdx
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
