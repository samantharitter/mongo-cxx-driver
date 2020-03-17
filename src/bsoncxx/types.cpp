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

#include <bsoncxx/types.hpp>

#include <bsoncxx/json.hpp>
#include <bsoncxx/private/libbson.hh>

#include <bsoncxx/config/private/prelude.hh>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

#define BSONCXX_ENUM(name, val) constexpr type types::b_##name::type_id;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM

types::b_binary::b_binary() {}

types::b_binary::b_binary(binary_sub_type type, uint32_t len, const uint8_t* data)
    : sub_type(type), size(len), bytes(const_cast<uint8_t*>(data)) {}

types::b_binary::~b_binary() {
    if (this->_is_owned) {
        bson_free(this->bytes);
    }
}

types::b_binary::b_binary(const b_binary& rhs) noexcept {
    this->size = rhs.size;
    this->_is_owned = rhs._is_owned;

    if (rhs._is_owned) {
        this->bytes = (uint8_t*)bson_malloc(this->size);
        memcpy(this->bytes, rhs.bytes, this->size);
    } else {
        this->bytes = rhs.bytes;
    }

    this->sub_type = rhs.sub_type;
}

types::b_binary& types::b_binary::operator=(const b_binary& rhs) noexcept {
    if (this->_is_owned) {
        bson_free(this->bytes);
    }

    this->size = rhs.size;
    this->_is_owned = rhs._is_owned;

    if (rhs._is_owned) {
        this->bytes = (uint8_t*)bson_malloc(this->size);
        memcpy(this->bytes, rhs.bytes, this->size);
    } else {
        this->bytes = rhs.bytes;
    }

    this->sub_type = rhs.sub_type;

    return *this;
}

types::b_binary::b_binary(b_binary&& rhs) noexcept {
    this->bytes = rhs.bytes;
    this->_is_owned = rhs._is_owned;

    if (rhs._is_owned) {
        rhs._is_owned = false;
        rhs.bytes = nullptr;
    }

    this->size = rhs.size;
    this->sub_type = rhs.sub_type;
}

types::b_binary& types::b_binary::operator=(b_binary&& rhs) noexcept {
    if (this->_is_owned) {
        bson_free(this->bytes);
    }

    this->bytes = rhs.bytes;
    this->_is_owned = rhs._is_owned;

    if (rhs._is_owned) {
        rhs._is_owned = false;
        rhs.bytes = nullptr;
    }

    this->size = rhs.size;
    this->sub_type = rhs.sub_type;

    return *this;
}

void types::b_binary::set_owned_buffer(uint8_t* data, uint32_t data_size) {
    if (this->_is_owned) {
        bson_free(this->bytes);
    }

    this->_is_owned = true;
    this->bytes = data;
    this->size = data_size;
}

bool types::b_binary::is_owned() const {
    return _is_owned;
}

std::string BSONCXX_CALL to_string(type rhs) {
    switch (static_cast<uint8_t>(rhs)) {
#define BSONCXX_ENUM(name, val) \
    case val:                   \
        return (#name);         \
        break;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
        default:
            return "?";
    }
}

std::string BSONCXX_CALL to_string(binary_sub_type rhs) {
    switch (static_cast<uint8_t>(rhs)) {
#define BSONCXX_ENUM(name, val) \
    case val:                   \
        return (#name);         \
        break;
#include <bsoncxx/enums/binary_sub_type.hpp>
#undef BSONCXX_ENUM
        default:
            return "?";
    }
}

namespace types {}  // namespace types
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
