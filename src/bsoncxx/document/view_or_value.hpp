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

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace document {

///
/// Class representing either a bsoncxx::document::view or a bsoncxx::document::value.
///
class BSONCXX_API view_or_value {
   public:
    ///
    /// Constructs a view_or_value from a bsoncxx::document::view. When constructed with a view,
    /// this object is non-owning. The value underneath the given view must outlive this object.
    ///
    /// @param view
    ///   A view into a document::value.
    ///
    explicit view_or_value(document::view view);

    ///
    /// Constructs a view_or_value from a bsoncxx::document::value. This object owns the passed-in
    /// value.
    ///
    /// @param value
    ///   A bsoncxx::document::value.
    ///
    explicit view_or_value(document::value value);

    ///
    /// Support conversion from this type to a bsoncxx::document::view.
    ///
    /// @return a view into this view_or_value.
    ///
    BSONCXX_INLINE operator document::view() const;

   private:
    document::value _value;
    document::view _view;
};

BSONCXX_INLINE view_or_value::operator document::view() const {
    return _view;
}

}  // namespace document
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
