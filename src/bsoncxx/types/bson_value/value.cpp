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

#include <bsoncxx/types/bson_value/value.hpp>

#include <cstdlib>
#include <cstring>
#include <utility>

#include <bsoncxx/config/private/prelude.hh>

namespace bsoncxx {
    BSONCXX_INLINE_NAMESPACE_BEGIN
    namespace types {
	namespace bson_value {

	    
namespace {

void uint8_t_deleter(std::uint8_t* ptr) {
    delete[] ptr;
}

}  // namespace

	    value::value(bson_value::view view) : _raw(new std::uint8_t[static_cast<std::size_t>(view._length)]), _length(view._length), _offset(view._offset), _keylen(view._keylen) {
		std::copy(view._raw, view._raw + view._length, _raw.get());
	    }

value::value(const value& rhs) : value(rhs.view()) {}

value& value::operator=(const value& rhs) {
    *this = value{rhs.view()};
    return *this;
}    
	    
}  // namespace bson_value
}  // namespace types

BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
