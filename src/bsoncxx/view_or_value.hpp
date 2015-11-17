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

#include <bsoncxx/config/prelude.hpp>

#include <type_traits>

#include <bsoncxx/stdx/optional.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

///
/// Class representing a view-or-value variant type.
///
template <typename View, typename Value>
class BSONCXX_API view_or_value {
   public:
    ///
    /// Construct a view_or_value from a View. When constructed with a View,
    /// this object is non-owning. The Value underneath the given View must outlive this object.
    ///
    /// @param view
    ///   A non-owning View.
    ///
    BSONCXX_INLINE view_or_value(View view) : _view{view} {
    }

    ///
    /// Constructs a view_or_value from a Value type. This object owns the passed-in Value.
    ///
    /// @param value
    ///   A Value type.
    ///
    BSONCXX_INLINE view_or_value(Value&& value) : _value{std::move(value)}, _view{*_value} {
    }

    ///
    /// Class View must be constructible from an instance of class Value.
    ///
    static_assert(std::is_constructible<View, Value>::value,
                  "View type must be constructible from a Value");

    ///
    /// Construct a view_or_value from a moved-in view_or_value.
    ///
    BSONCXX_INLINE view_or_value(view_or_value&& other)
        : _value{std::move(other._value)}, _view{_value ? *_value : std::move(other._view)} {
    }

    ///
    /// Assign to this view_or_value from a moved-in view_or_value.
    ///
    BSONCXX_INLINE view_or_value& operator=(view_or_value&& other) {
        _value = std::move(other._value);
        _view = _value ? *_value : std::move(other._view);
    }

    ///
    /// Construct a view_or_value from a copied view_or_value.
    ///
    BSONCXX_INLINE view_or_value(const view_or_value& other)
        : _value{other._value}, _view{_value ? *_value : other._view} {
    }

    ///
    /// Assign to this view_or_value from a copied view_or_value.
    ///
    BSONCXX_INLINE view_or_value& operator=(const view_or_value& other) {
        _value = other._value;
        _view = _value ? *_value : other._view;
    }

    ///
    /// This type may be used as a View.
    ///
    /// @return a View into this view_or_value.
    ///
    BSONCXX_INLINE operator View() const;

   private:
    stdx::optional<Value> _value;
    View _view;
};

template <typename View, typename Value>
BSONCXX_INLINE view_or_value<View, Value>::operator View() const {
    return _view;
}

BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
