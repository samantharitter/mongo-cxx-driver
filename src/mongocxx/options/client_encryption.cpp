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

#include <mongocxx/options/client_encryption.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

client_encryption& client_encryption::key_vault_client(mongocxx::client* client) {
    _key_vault_client = client;
    return *this;
}

const stdx::optional<mongocxx::client*>& client_encryption::key_vault_client() const {
    return _key_vault_client;
}

client_encryption& client_encryption::key_vault_namespace(client_encryption::ns_pair ns) {
    _key_vault_namespace = ns;
    return *this;
}

const stdx::optional<client_encryption::ns_pair>& client_encryption::key_vault_namespace() const {
    return _key_vault_namespace;
}

client_encryption& client_encryption::kms_providers(
    bsoncxx::document::view_or_value kms_providers) {
    _kms_providers = std::move(kms_providers);
    return *this;
}

const stdx::optional<bsoncxx::document::view_or_value>& client_encryption::kms_providers() const {
    return _kms_providers;
}
}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
