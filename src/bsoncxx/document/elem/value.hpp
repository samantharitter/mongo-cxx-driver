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

#pragma once

#include <memory>

#include <bsoncxx/document/element/view.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

namespace document {
namespace elem {

///
/// A variant owning type that represents BSON values serialized
/// from a BSON document and owns its underlying buffer. When an
/// element::value goes out of scope, its underlying buffer is freed.
///
/// For accessors into this type and to extract the various values out
/// of it, please see document::element::view.
///
/// @relatesalso document::element::view
///
class BSONCXX_API value {
   public:
    using unique_ptr_type = std::unique_ptr<uint8_t[], deleter_type>;

    ///
    /// Constructs an element::value from an element::view. The data
    /// referenced by the element::view will be copied into a new buffer
    /// managed by the constructed element::value/
    ///
    /// @param view
    ///   A view of the element to copy.
    ///
    explicit value(elem::view view);

    value(const value&);
    value& operator=(const value&);

    value(value&&) noexcept = default;
    value& operator=(value&&) noexcept = default;

    ///
    /// Get an elem::view over the element owned by this value type.
    ///
    BSONCXX_INLINE elem::view view() const noexcept;

    ///
    /// Conversion operator that provides a view given a value.
    ///
    /// @return An elem::view over this value.
    ///
    BSONCXX_INLINE operator elem::view() const noexcept;

   private:
    unique_ptr_type _raw;

    std::uint32_t _length;
    std::uint32_t _keylen;
    std::uint32_t _offset;
};

BSONCXX_INLINE elem::view value::view() const noexcept {
    return elem::view{static_cast<uint8_t*>(_data.get()), _length, _offset, _keylen};
}

BSONCXX_INLINE value::operator document::view() const noexcept {
    return view();
}

}  // namespace elem
}  // namespace document

BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
