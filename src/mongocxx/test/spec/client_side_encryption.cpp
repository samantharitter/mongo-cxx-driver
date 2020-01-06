// Copyright 2019-present MongoDB Inc.
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

#include <fstream>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/test/spec/monitoring.hh>
#include <mongocxx/test/spec/operation.hh>
#include <mongocxx/test/spec/util.hh>
#include <mongocxx/test_util/client_helpers.hh>

namespace {

using namespace bsoncxx;
using namespace mongocxx;
using namespace spec;

void _set_up_key_vault(const client& client, document::view test_spec_view) {
    if (test_spec_view["key_vault_data"]) {
        write_concern wc_majority;
        wc_majority.acknowledge_level(write_concern::level::k_majority);

        auto coll = client["admin"]["datakeys"];
        coll.drop(wc_majority);

        for (auto&& doc : test_spec_view["key_vault_data"].get_array().value) {
            options::insert insert_opts;
            insert_opts.write_concern(wc_majority);
            coll.insert_one(doc.get_document().value, insert_opts);
        }
    }
}

void add_auto_encryption_opts(document::view test, options::client client_opts) {
    if (test["clientOptions"]["autoEncryptOpts"]) {
        auto test_encrypt_opts = test["clientOptions"]["autoEncryptOpts"].get_document().value;

        options::auto_encryption auto_encrypt_opts{};

        if (test_encrypt_opts["bypassAutoEncryption"]) {
            auto_encrypt_opts.bypass_auto_encryption(
                test_encrypt_opts["bypassAutoEncryption"].get_bool().value);
        }

        if (test_encrypt_opts["keyVaultNamespace"]) {
            auto ns_string =
                string::to_string(test_encrypt_opts["keyVaultNamespace"].get_utf8().value);
            auto dot = ns_string.find(".");
            std::string db = ns_string.substr(0, dot);
            std::string coll = ns_string.substr(dot);

            auto_encrypt_opts.key_vault_namespace({db.c_str(), coll.c_str()});
        } else {
            auto_encrypt_opts.key_vault_namespace({"admin", "datakeys"});
        }

        if (test_encrypt_opts["schemaMap"]) {
            auto_encrypt_opts.schema_map(test_encrypt_opts["schemaMap"].get_document().value);
        }

        // KMS providers are set through the URI in get_uri().

        client_opts.auto_encryption_opts(std::move(auto_encrypt_opts));
    }
}

void run_encryption_tests_in_file(const std::string& test_path) {
    INFO("Test path: " << test_path);

    auto test_spec = test_util::parse_test_file(test_path);
    REQUIRE(test_spec);

    auto test_spec_view = test_spec->view();
    auto db_name = test_spec_view["database_name"].get_utf8().value;
    auto coll_name = test_spec_view["collection_name"].get_utf8().value;
    auto tests = test_spec_view["tests"].get_array().value;

    /* we may not have a supported topology */
    if (should_skip_spec_test(client{uri{}}, test_spec_view)) {
        WARN("File skipped - " + test_path);
        return;
    }

    class client setup_client {
        uri {}
    };
    write_concern wc_majority;
    wc_majority.acknowledge_level(write_concern::level::k_majority);

    for (auto&& test : tests) {
        auto description = test["description"].get_utf8().value;
        INFO("Test description: " << description);
        if (should_skip_spec_test(client{uri{}}, test.get_document().value)) {
            continue;
        }

        _set_up_key_vault(setup_client, test_spec_view);
        set_up_collection(setup_client, test_spec_view);

        options::client client_opts;

        apm_checker apm_checker;
        client_opts.apm_opts(apm_checker.get_apm_opts(true /* command_started_events_only */));

        add_auto_encryption_opts(test.get_document().value, client_opts);

        class client client {
            get_uri(test.get_document().value), std::move(client_opts)
        };

        auto db = client[db_name];
        auto test_coll = db[coll_name];

        for (auto&& op : test["operations"].get_array().value) {
            run_operation_check_result(op.get_document().value,
                                       [&]() { return operation_runner{&test_coll}; });
        }

        if (test["expectations"]) {
            apm_checker.compare(test["expectations"].get_array().value, true);
        }

        if (test["outcome"] && test["outcome"]["collection"]) {
            class client plaintext_client {
                uri {}
            };

            read_preference rp;
            read_concern rc;
            rp.mode(read_preference::read_mode::k_primary);
            rc.acknowledge_level(read_concern::level::k_local);

            auto outcome_coll = plaintext_client[db_name][coll_name];
            outcome_coll.read_concern(rc);
            outcome_coll.read_preference(std::move(rp));

            test_util::check_outcome_collection(
                &outcome_coll, test["outcome"]["collection"]["data"].get_array().value);
        }
    }
}

TEST_CASE("Client side encryption spec automated tests", "[client_side_encryption_spec]") {
    try {
        instance::current();

        char* encryption_tests_path = std::getenv("ENCRYPTION_TESTS_PATH");
        REQUIRE(encryption_tests_path);

        std::string path{encryption_tests_path};
        if (path.back() == '/') {
            path.pop_back();
        }

        std::ifstream test_files{path + "/test_files.txt"};
        REQUIRE(test_files.good());

        std::string test_file;
        while (std::getline(test_files, test_file)) {
            run_encryption_tests_in_file(path + "/" + test_file);
        }
    } catch (mongocxx::exception e) {
        fprintf(stderr, "Caught exception: %s\n\n %s\n\n", e.what(), e.trace());
    }
}

}  // namespace
