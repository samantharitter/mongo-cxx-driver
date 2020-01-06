// Copyright 2019 MongoDB Inc.
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

#include <mongocxx/options/data_key.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

data_key& data_key::master_key(bsoncxx::document::view_or_value master_key) {
    _master_key = std::move(master_key);
    return *this;
}

const stdx::optional<bsoncxx::document::view_or_value>& data_key::master_key() const {
    return _master_key;
}

data_key& data_key::key_alt_names(std::vector<std::string> key_alt_names) {
    if (!_key_alt_names.empty()) {
        _key_alt_names.clear();
    }

    _key_alt_names = std::move(key_alt_names);

    return *this;
}

const std::vector<std::string>& data_key::key_alt_names() const {
    return _key_alt_names;
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
