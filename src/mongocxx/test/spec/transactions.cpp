// Copyright 2018-present MongoDB Inc.
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
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/aggregate.hpp>
#include <mongocxx/options/count.hpp>
#include <mongocxx/options/delete.hpp>
#include <mongocxx/options/distinct.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/options/find_one_and_delete.hpp>
#include <mongocxx/options/find_one_and_replace.hpp>
#include <mongocxx/options/find_one_and_update.hpp>
#include <mongocxx/options/find_one_common_options.hpp>
#include <mongocxx/options/update.hpp>
#include <mongocxx/pipeline.hpp>
#include <mongocxx/result/delete.hpp>
#include <mongocxx/result/insert_many.hpp>
#include <mongocxx/result/insert_one.hpp>
#include <mongocxx/result/replace_one.hpp>
#include <mongocxx/result/update.hpp>
#include <mongocxx/test/spec/monitoring.hh>
#include <mongocxx/test/spec/operation.hh>
#include <mongocxx/test/spec/util.hh>
#include <mongocxx/test_util/client_helpers.hh>

namespace {
using namespace bsoncxx;
using namespace mongocxx;
using namespace spec;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
using bsoncxx::stdx::optional;
using bsoncxx::stdx::string_view;

void test_setup(document::view test, document::view test_spec) {
    // Step 1. "clean up any open transactions from previous test failures"
    client client{uri{}};
    try {
        client["admin"].run_command(make_document(kvp("killAllSessions", make_array())));
    } catch (const operation_exception& e) {
    }

    // Steps 2 - 5, set up new collection
    set_up_collection(client, test_spec);

    // Step 6. "If failPoint is specified, its value is a configureFailPoint command"
    configure_fail_point(client, test);
}

void parse_session_opts(document::view session_opts, options::client_session* out) {
    options::transaction txn_opts;
    if (session_opts["defaultTransactionOptions"]) {
        auto rc = lookup_read_concern(session_opts["defaultTransactionOptions"].get_document());
        if (rc) {
            txn_opts.read_concern(*rc);
        }

        auto wc = lookup_write_concern(session_opts["defaultTransactionOptions"].get_document());
        if (wc) {
            txn_opts.write_concern(*wc);
        }

        auto rp = lookup_read_preference(session_opts["defaultTransactionOptions"].get_document());
        if (rp) {
            txn_opts.read_preference(*rp);
        }
    }

    out->default_transaction_opts(txn_opts);
}

void run_transactions_tests_in_file(const std::string& test_path) {
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

    for (auto&& test : tests) {
        bool fail_point_enabled = (bool)test["failPoint"];
        auto description = test["description"].get_utf8().value;
        INFO("Test description: " << description);
        if (should_skip_spec_test(client{uri{}}, test.get_document().value)) {
            continue;
        }

        // Steps 1-6.
        test_setup(test.get_document().value, test_spec_view);

        // Step 7. "Create a new MongoClient client, with Command Monitoring listeners enabled."
        options::client client_opts;
        apm_checker apm_checker;
        client_opts.apm_opts(apm_checker.get_apm_opts(true /* command_started_events_only */));
        client client;
        if (test["useMultipleMongoses"]) {
            client = {uri{"mongodb://localhost:27017,localhost:27018"}, client_opts};
        } else {
            client = {get_uri(test.get_document().value), client_opts};
        }

        /* individual test may contain a skipReason */
        if (should_skip_spec_test(client, test.get_document())) {
            continue;
        }

        options::client_session session0_opts;
        options::client_session session1_opts;

        // Step 8: "Call client.startSession twice to create ClientSession objects"
        if (test["sessionOptions"] && test["sessionOptions"]["session0"]) {
            parse_session_opts(test["sessionOptions"]["session0"].get_document().value,
                               &session0_opts);
        }
        if (test["sessionOptions"] && test["sessionOptions"]["session1"]) {
            parse_session_opts(test["sessionOptions"]["session1"].get_document().value,
                               &session1_opts);
        }

        document::value session_lsid0{{}};
        document::value session_lsid1{{}};

        // We wrap this section in its own scope as a way to control when the client_session
        // objects created inside get destroyed. On destruction, client_sessions can send
        // an abortTransaction that some of the spec tests look for.
        {
            client_session session0 = client.start_session(session0_opts);
            client_session session1 = client.start_session(session1_opts);
            session_lsid0.reset(session0.id());
            session_lsid1.reset(session1.id());

            // Step 9. Perform the operations.
            apm_checker.clear();
            auto operations = test["operations"].get_array().value;
            for (auto&& op : operations) {
                fail_point_enabled =
                    fail_point_enabled || op.get_document().value["arguments"]["failPoint"];
                auto op_view = op.get_document().value;
                database db = client[db_name];
                parse_database_options(op_view, &db);
                collection coll = db[coll_name];
                parse_collection_options(op_view, &coll);
                run_operation_check_result(op_view, [&]() {
                    return operation_runner{&db, &coll, &session0, &session1, &client};
                });
            }
        }
        // Step 10. "Call session0.endSession() and session1.endSession." (done in destructors).

        // Step 11. Compare APM events.
        test_util::match_visitor visitor = [&](bsoncxx::stdx::string_view key,
                                               bsoncxx::stdx::optional<bsoncxx::types::value> main,
                                               bsoncxx::types::value pattern) {
            if (key.compare("lsid") == 0) {
                REQUIRE(pattern.type() == type::k_utf8);
                REQUIRE(main);
                REQUIRE(main->type() == type::k_document);
                auto session_name = pattern.get_utf8().value;
                if (session_name.compare("session0") == 0) {
                    REQUIRE(test_util::matches(session_lsid0, main->get_document().value));
                } else {
                    REQUIRE(test_util::matches(session_lsid1, main->get_document().value));
                }
                return test_util::match_action::k_skip;
            } else if (pattern.type() == type::k_null) {
                if (main) {
                    return test_util::match_action::k_not_equal;
                }
                return test_util::match_action::k_skip;
            }
            return test_util::match_action::k_skip;
        };

        if (test["expectations"]) {
            apm_checker.compare(test["expectations"].get_array().value, false, visitor);
        }

        // Step 12. Disable the failpoint.
        if (fail_point_enabled) {
            disable_fail_point("mongodb://localhost:27017", client_opts);
            if (test["useMultipleMongoses"]) {
                disable_fail_point("mongodb://localhost:27018", client_opts);
            }
        }

        // Step 13. Compare the collection outcomes
        if (test["outcome"] && test["outcome"]["collection"]) {
            auto coll = client[db_name][coll_name];
            test_util::check_outcome_collection(&coll,
                                                test["outcome"]["collection"].get_document().value);
        }
    }
}

TEST_CASE("Transactions spec automated tests", "[transactions_spec]") {
    instance::current();

    char* transactions_tests_path = std::getenv("TRANSACTIONS_TESTS_PATH");
    REQUIRE(transactions_tests_path);

    std::string path{transactions_tests_path};
    if (path.back() == '/') {
        path.pop_back();
    }

    std::ifstream test_files{path + "/test_files.txt"};
    REQUIRE(test_files.good());

    std::string test_file;
    while (std::getline(test_files, test_file)) {
        run_transactions_tests_in_file(path + "/" + test_file);
    }
}
}  // namespace
