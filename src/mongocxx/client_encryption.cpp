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

#include <mongocxx/client_encryption.hpp>

#include <bsoncxx/private/libbson.hh>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/exception/private/mongoc_error.hh>
#include <mongocxx/options/client_encryption.hpp>
#include <mongocxx/private/client.hh>
#include <mongocxx/private/libbson.hh>
#include <mongocxx/private/libmongoc.hh>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

using mongocxx::libbson::scoped_bson_t;

client_encryption::client_encryption(options::client_encryption opts) : _opts(std::move(opts)) {}

using client_cb = std::function<mongoc_client_t*(const options::client_encryption&)>;

static mongoc_client_encryption_opts_t* _convert_encryption_opts(
    const options::client_encryption& options, client_cb get_client) {
    mongoc_client_encryption_opts_t* opts_t = libmongoc::client_encryption_opts_new();

    if (options.key_vault_client()) {
        mongoc_client_t* client_t = get_client(options);
        libmongoc::client_encryption_opts_set_keyvault_client(opts_t, client_t);
    }

    if (options.key_vault_namespace()) {
        auto ns = *options.key_vault_namespace();
        libmongoc::client_encryption_opts_set_keyvault_namespace(
            opts_t, ns.first.c_str(), ns.second.c_str());
    }

    if (options.kms_providers()) {
        scoped_bson_t kms_providers{*options.kms_providers()};
        libmongoc::client_encryption_opts_set_kms_providers(opts_t, kms_providers.bson());
    }

    return opts_t;
}

static mongoc_client_encryption_datakey_opts_t* _convert_datakey_opts(
    const options::data_key& options) {
    mongoc_client_encryption_datakey_opts_t* opts_t =
        libmongoc::client_encryption_datakey_opts_new();
    if (options.master_key()) {
        scoped_bson_t master_key{*options.master_key()};
        libmongoc::client_encryption_datakey_opts_set_masterkey(opts_t, master_key.bson());
    }

    if (!options.key_alt_names().empty()) {
        auto altnames = options.key_alt_names();
        // char* names[altnames.size()];
        char** names = (char**)bson_malloc(sizeof(char*) * altnames.size());
        int i = 0;

        for (auto&& name : altnames) {
            names[i++] = const_cast<char*>(name.data());
        }

        libmongoc::client_encryption_datakey_opts_set_keyaltnames(opts_t, names, i);
    }

    return opts_t;
}

bsoncxx::types::b_binary client_encryption::create_data_key(std::string kms_provider,
                                                            const options::data_key& opts) {
    bsoncxx::types::b_binary uuid;
    bson_value_t keyid;
    bson_error_t error;

    auto get_client = [this](const options::client_encryption& options) {
        return (*options.key_vault_client())->_get_impl().client_t;
    };

    auto encryption_opts = _convert_encryption_opts(_opts, get_client);
    auto datakey_opts = _convert_datakey_opts(opts);

    auto client_encryption_t = libmongoc::client_encryption_new(encryption_opts, &error);
    if (client_encryption_t == nullptr) {
        throw_exception<operation_exception>(error);
    }

    if (!libmongoc::client_encryption_create_datakey(
            client_encryption_t, kms_provider.c_str(), datakey_opts, &keyid, &error)) {
        throw_exception<operation_exception>(error);
    }

    // TODO, will need to pull this out.
    // Convert the bson_value_t to a bsoncxx b_binary value
    uuid.sub_type = bsoncxx::binary_sub_type::k_uuid;
    uuid.size = keyid.value.v_binary.data_len;
    uuid.bytes = keyid.value.v_binary.data;

    bson_value_destroy(&keyid);

    return uuid;
}

// bsoncxx::types::value client_encryption::encrypt(const bsoncxx::types::value& value, const
// options::encrypt& opts) {
// 	return {};
// }

// bsoncxx::types::value client_encryption::decrypt(const bsoncxx::types::value& value) {
// 	return {};
// }

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
