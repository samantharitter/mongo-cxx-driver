// Copyright 2020 MongoDB Inc.
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

#include <bsoncxx/document/element/view.hpp>

#include <cstdlib>
#include <cstring>

#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/private/libbson.hh>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>

#include <bsoncxx/config/private/prelude.hh>

#define BSONCXX_CITER \
    bson_iter_t iter; \
    bson_iter_init_from_data_at_offset(&iter, _raw, _length, _offset, _keylen);

#define BSONCXX_TYPE_CHECK(name)                                              \
    do {                                                                      \
        if (type() != bsoncxx::type::name) {                                  \
            throw bsoncxx::exception{error_code::k_need_element_type_##name}; \
        }                                                                     \
    } while (0)

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace document {
namespace element {

view::view() : view(nullptr, 0, 0, 0) {}

view::view(const std::uint8_t* raw,
           std::uint32_t length,
           std::uint32_t offset,
           std::uint32_t keylen)
    : _raw(raw), _length(length), _offset(offset), _keylen(keylen) {}

const std::uint8_t* view::raw() const {
    return _raw;
}

std::uint32_t view::length() const {
    return _length;
}
std::uint32_t view::offset() const {
    return _offset;
}

std::uint32_t view::keylen() const {
    return _keylen;
}

bsoncxx::type view::type() const {
    if (_raw == nullptr) {
        throw bsoncxx::exception{error_code::k_unset_element};
    }

    BSONCXX_CITER;
    return static_cast<bsoncxx::type>(bson_iter_type(&iter));
}

stdx::string_view view::key() const {
    if (_raw == nullptr) {
        throw bsoncxx::exception{error_code::k_unset_element};
    }

    BSONCXX_CITER;

    const char* key = bson_iter_key(&iter);

    return stdx::string_view{key};
}

types::b_binary view::get_binary() const {
    BSONCXX_TYPE_CHECK(k_binary);

    BSONCXX_CITER;

    bson_subtype_t type;
    std::uint32_t len;
    const std::uint8_t* binary;

    bson_iter_binary(&iter, &type, &len, &binary);

    return types::b_binary{static_cast<binary_sub_type>(type), len, binary};
}

types::b_utf8 view::get_utf8() const {
    BSONCXX_TYPE_CHECK(k_utf8);

    BSONCXX_CITER;

    uint32_t len;
    const char* val = bson_iter_utf8(&iter, &len);

    return types::b_utf8{stdx::string_view{val, len}};
}

types::b_double view::get_double() const {
    BSONCXX_TYPE_CHECK(k_double);
    BSONCXX_CITER;
    return types::b_double{bson_iter_double(&iter)};
}
types::b_int32 view::get_int32() const {
    BSONCXX_TYPE_CHECK(k_int32);
    BSONCXX_CITER;
    return types::b_int32{bson_iter_int32(&iter)};
}
types::b_int64 view::get_int64() const {
    BSONCXX_TYPE_CHECK(k_int64);
    BSONCXX_CITER;
    return types::b_int64{bson_iter_int64(&iter)};
}
types::b_undefined view::get_undefined() const {
    BSONCXX_TYPE_CHECK(k_undefined);
    return types::b_undefined{};
}
types::b_oid view::get_oid() const {
    BSONCXX_TYPE_CHECK(k_oid);
    BSONCXX_CITER;

    const bson_oid_t* boid = bson_iter_oid(&iter);
    oid v(reinterpret_cast<const char*>(boid->bytes), sizeof(boid->bytes));

    return types::b_oid{v};
}
types::b_decimal128 view::get_decimal128() const {
    BSONCXX_TYPE_CHECK(k_decimal128);
    BSONCXX_CITER;

    bson_decimal128_t d128;
    bson_iter_decimal128(&iter, &d128);

    return types::b_decimal128{decimal128{d128.high, d128.low}};
}

types::b_bool view::get_bool() const {
    BSONCXX_TYPE_CHECK(k_bool);
    BSONCXX_CITER;
    return types::b_bool{bson_iter_bool(&iter)};
}
types::b_date view::get_date() const {
    BSONCXX_TYPE_CHECK(k_date);
    BSONCXX_CITER;
    return types::b_date{std::chrono::milliseconds{bson_iter_date_time(&iter)}};
}
types::b_null view::get_null() const {
    BSONCXX_TYPE_CHECK(k_null);
    return types::b_null{};
}

types::b_regex view::get_regex() const {
    BSONCXX_TYPE_CHECK(k_regex);
    BSONCXX_CITER;

    const char* options;
    const char* regex = bson_iter_regex(&iter, &options);

    return types::b_regex{stdx::string_view{regex}, stdx::string_view{options}};
}

types::b_dbpointer view::get_dbpointer() const {
    BSONCXX_TYPE_CHECK(k_dbpointer);
    BSONCXX_CITER;

    uint32_t collection_len;
    const char* collection;
    const bson_oid_t* boid;
    bson_iter_dbpointer(&iter, &collection_len, &collection, &boid);

    oid v{reinterpret_cast<const char*>(boid->bytes), sizeof(boid->bytes)};

    return types::b_dbpointer{stdx::string_view{collection, collection_len}, v};
}

types::b_code view::get_code() const {
    BSONCXX_TYPE_CHECK(k_code);
    BSONCXX_CITER;

    uint32_t len;
    const char* code = bson_iter_code(&iter, &len);

    return types::b_code{stdx::string_view{code, len}};
}

types::b_symbol view::get_symbol() const {
    BSONCXX_TYPE_CHECK(k_symbol);
    BSONCXX_CITER;

    uint32_t len;
    const char* symbol = bson_iter_symbol(&iter, &len);

    return types::b_symbol{stdx::string_view{symbol, len}};
}

types::b_codewscope view::get_codewscope() const {
    BSONCXX_TYPE_CHECK(k_codewscope);
    BSONCXX_CITER;

    uint32_t code_len;
    const uint8_t* scope_ptr;
    uint32_t scope_len;
    const char* code = bson_iter_codewscope(&iter, &code_len, &scope_len, &scope_ptr);
    document::view view(scope_ptr, scope_len);

    return types::b_codewscope{stdx::string_view{code, code_len}, view};
}

types::b_timestamp view::get_timestamp() const {
    BSONCXX_TYPE_CHECK(k_timestamp);
    BSONCXX_CITER;

    uint32_t timestamp;
    uint32_t increment;
    bson_iter_timestamp(&iter, &timestamp, &increment);

    return types::b_timestamp{increment, timestamp};
}

types::b_minkey view::get_minkey() const {
    BSONCXX_TYPE_CHECK(k_minkey);
    return types::b_minkey{};
}
types::b_maxkey view::get_maxkey() const {
    BSONCXX_TYPE_CHECK(k_maxkey);
    return types::b_maxkey{};
}

types::b_document view::get_document() const {
    BSONCXX_TYPE_CHECK(k_document);
    BSONCXX_CITER;

    const std::uint8_t* buf;
    std::uint32_t len;

    bson_iter_document(&iter, &len, &buf);

    return types::b_document{document::view{buf, len}};
}

types::b_array view::get_array() const {
    BSONCXX_TYPE_CHECK(k_array);
    BSONCXX_CITER;

    const std::uint8_t* buf;
    std::uint32_t len;

    bson_iter_array(&iter, &len, &buf);

    return types::b_array{array::view{buf, len}};
}

types::value view::get_value() const {
    switch (static_cast<int>(type())) {
#define BSONCXX_ENUM(type, val) \
    case val:                   \
        return types::value{get_##type()};
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
    }

    BSONCXX_UNREACHABLE;
}

view view::operator[](stdx::string_view key) const {
    if (_raw == nullptr || type() != bsoncxx::type::k_document)
        return element();
    document::view doc = get_document();
    return doc[key];
}

array::element view::operator[](std::uint32_t i) const {
    if (_raw == nullptr || type() != bsoncxx::type::k_array)
        return array::element();
    array::view arr = get_array();
    return arr[i];
}

view::operator bool() const {
    return _raw != nullptr;
}

}  // namespace element
}  // namespace document
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
