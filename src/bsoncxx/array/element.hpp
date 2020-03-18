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

#include <cstddef>
#include <cstdint>

#include <bsoncxx/document/element/view.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

namespace array {

///
/// A variant view type that accesses values in serialized BSON arrays.
///
/// Element functions as a variant type, where the kind of the element can be
/// interrogated by calling type() and a specific value can be extracted through
/// get_X() accessors.
///
class BSONCXX_API element : private document::element::view {
   public:
    element();

    using document::elem::view::operator bool;

    using document::elem::view::type;

    using document::elem::view::get_double;
    using document::elem::view::get_utf8;
    using document::elem::view::get_document;
    using document::elem::view::get_array;
    using document::elem::view::get_binary;
    using document::elem::view::get_undefined;
    using document::elem::view::get_oid;
    using document::elem::view::get_bool;
    using document::elem::view::get_date;
    using document::elem::view::get_null;
    using document::elem::view::get_regex;
    using document::elem::view::get_dbpointer;
    using document::elem::view::get_code;
    using document::elem::view::get_symbol;
    using document::elem::view::get_codewscope;
    using document::elem::view::get_int32;
    using document::elem::view::get_timestamp;
    using document::elem::view::get_int64;
    using document::elem::view::get_decimal128;
    using document::elem::view::get_minkey;
    using document::elem::view::get_maxkey;

    using document::elem::view::get_value;

    using document::elem::view::operator[];

    using document::elem::view::raw;
    using document::elem::view::length;
    using document::elem::view::offset;
    using document::elem::view::keylen;

   private:
    friend class view;

    BSONCXX_PRIVATE explicit element(const std::uint8_t* raw,
                                     std::uint32_t length,
                                     std::uint32_t offset,
                                     std::uint32_t keylen);
};

}  // namespace array

BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
