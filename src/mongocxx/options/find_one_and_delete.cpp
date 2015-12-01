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

#include <mongocxx/options/find_one_and_delete.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

void find_one_and_delete::max_time_ms(std::int64_t max_time_ms) {
    _max_time_ms = max_time_ms;
}

void find_one_and_delete::projection(bsoncxx::document::view_or_value projection) {
    _projection = projection;
}

void find_one_and_delete::sort(bsoncxx::document::view_or_value ordering) {
    _ordering = ordering;
}

const stdx::optional<bsoncxx::document::view_or_value>& find_one_and_delete::projection() const {
    return _projection;
}

const stdx::optional<bsoncxx::document::view_or_value>& find_one_and_delete::sort() const {
    return _ordering;
}

const stdx::optional<std::int64_t>& find_one_and_delete::max_time_ms() const {
    return _max_time_ms;
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
