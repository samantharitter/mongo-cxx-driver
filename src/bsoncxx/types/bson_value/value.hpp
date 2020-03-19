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

#include <bsoncxx/types/bson_value/view.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

namespace types {
    namespace bson_value {

	///
	/// A variant owning type that represents any BSON type. Owns its underlying
	/// buffer. When a bson_value::value goes out of scope, its underlying
	/// buffer is freed.
	///
	/// For accessors into this type and to extract the various BSON types out,
	/// please use bson_value::view.
	///
	/// @relatesalso bson_value::view
	///
	class BSONCXX_API value {
	public:
	using deleter_type = void (*)(std::uint8_t*);
	using unique_ptr_type = std::unique_ptr<uint8_t[], deleter_type>;

	///
	/// Constructs a value from a bson_value::view. The data referenced by the
	/// view will be copied into a new buffer managed by this newly-constructed value.
	///
	/// @param view
	///   A view of a bson_value to copy.
	///	
	explicit value(bson_value::view view);

	value(const value&);
	value& operator=(const value&);

	value(value&&) noexcept = default;
	value& operator=(value&&) noexcept = default;

	///
	/// Get a view over the bson_value owned by this object.
	///
	BSONCXX_INLINE bson_value::view view() const noexcept;

	///
	/// Conversion operator that provides a bson_value::view given a bson_value::value.
	///
	BSONCXX_INLINE operator bson_value::view() const noexcept;

	private:
	unique_ptr_type _raw;

	std::uint32_t _length;
	std::uint32_t _offset;
	std::uint32_t _keylen;
	};
	
	BSONCXX_INLINE bson_value::view value::view() const noexcept {
	    return bson_value::view{static_cast<const uint8_t*>(_data.get()), _length, _offset, _keylen};
	}

	BSONCXX_INLINE value::operator bson_value::view() const noexcept {
	    return view();
	}
	
///
/// @{
///
/// Compares values for (in)-equality.
///
/// @relates bson_value::value
///
BSONCXX_INLINE bool operator==(const value& lhs, const value& rhs) {
    return (lhs.view() == rhs.view());
}

BSONCXX_INLINE bool operator!=(const value& lhs, const value& rhs) {
    return !(lhs == rhs);
}

///
/// @}
///


}  // namespace bson_value
}  // namespace types

BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
