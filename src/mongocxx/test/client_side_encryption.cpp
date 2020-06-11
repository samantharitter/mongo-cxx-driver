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

#include <helpers.hpp>

//#include <mongocxx/config/private/prelude.hh>

#include <fstream>
#include <sstream>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/client_encryption.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/client.hpp>
#include <mongocxx/options/client_encryption.hpp>
#include <mongocxx/options/data_key.hpp>
#include <mongocxx/read_concern.hpp>
#include <mongocxx/test/spec/monitoring.hh>
#include <mongocxx/test_util/client_helpers.hh>
#include <mongocxx/uri.hpp>
#include <mongocxx/write_concern.hpp>

namespace {
const auto kLocalMasterKey =
    "\x32\x78\x34\x34\x2b\x78\x64\x75\x54\x61\x42\x42\x6b\x59\x31\x36\x45\x72"
    "\x35\x44\x75\x41\x44\x61\x67\x68\x76\x53\x34\x76\x77\x64\x6b\x67\x38\x74"
    "\x70\x50\x70\x33\x74\x7a\x36\x67\x56\x30\x31\x41\x31\x43\x77\x62\x44\x39"
    "\x69\x74\x51\x32\x48\x46\x44\x67\x50\x57\x4f\x70\x38\x65\x4d\x61\x43\x31"
    "\x4f\x69\x37\x36\x36\x4a\x7a\x58\x5a\x42\x64\x42\x64\x62\x64\x4d\x75\x72"
    "\x64\x6f\x6e\x4a\x31\x64";

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

using bsoncxx::builder::basic::sub_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::finalize;

using namespace mongocxx;

    // Takes a path relative to the ENCRYPTION_TESTS_PATH variable, with leading '/'.
    bsoncxx::document::value _doc_from_file(std::string_view sub_path) {
	char* encryption_tests_path = std::getenv("ENCRYPTION_TESTS_PATH");
	std::string path{encryption_tests_path};
	if (path.back() == '/') {
	    path.pop_back();
	}

	std::stringstream stream;
	std::ifstream file{path + sub_path};
	REQUIRE(!file.bad());
	stream << file.rdbuf();

	return bsoncxx::from_json(stream.str());
    }

void _setup_drop_collections(const client& client) {
    write_concern wc_majority;
    wc_majority.acknowledge_level(write_concern::level::k_majority);

    auto datakeys = client["keyvault"]["datakeys"];
    datakeys.drop(wc_majority);

    auto coll = client["db"]["coll"];
    coll.drop(wc_majority);
}

bsoncxx::document::value _make_kms_doc() {
    // set both local and aws kms providers
    auto kms_doc = bsoncxx::builder::basic::document{};

    // AWS from environment
    auto access_key = std::getenv("MONGOCXX_TEST_AWS_SECRET_ACCESS_KEY");
    auto key_id = std::getenv("MONGOCXX_TEST_AWS_ACCESS_KEY_ID");
    if (!access_key || !key_id) {
        FAIL(
            "Please set environment variables for client side encryption tests:\n"
            "\tMONGOCXX_TEST_AWS_SECRET_ACCESS_KEY\n"
            "\tMONGOCXX_TEST_AWS_ACCESS_KEY_ID\n\n");
    }

    char key_storage[96];
    memcpy(&(key_storage[0]), kLocalMasterKey, 96);

    bsoncxx::types::b_binary local_master_key{
        bsoncxx::binary_sub_type::k_binary, 96, (const uint8_t*)&key_storage};

    kms_doc.append(
        kvp("local", [&](sub_document subdoc) { subdoc.append(kvp("key", local_master_key)); }));
    kms_doc.append(kvp("aws", [&](sub_document subdoc) {
        subdoc.append(kvp("secretAccessKey", access_key));
        subdoc.append(kvp("accessKeyId", key_id));
    }));

    return {kms_doc.extract()};
}

    void _add_client_encrypted_opts(options::client* client_opts,
				    bsoncxx::document::view_or_value schema_map,
				    bsoncxx::document::view_or_value kms_doc,
				    class client* key_vault_client = nullptr) {
    options::auto_encryption auto_encrypt_opts{};

    // KMS
    auto_encrypt_opts.kms_providers(std::move(kms_doc));
    auto_encrypt_opts.key_vault_namespace({"keyvault", "datakeys"});
    auto_encrypt_opts.schema_map({std::move(schema_map)});

    // For evergreen testing
    char* bypass_spawn = std::getenv("ENCRYPTION_TESTS_BYPASS_SPAWN");
    char* mongocryptd_path = std::getenv("MONGOCRYPTD_PATH");

    if (bypass_spawn || mongocryptd_path) {
        auto cmd = bsoncxx::builder::basic::document{};

        if (bypass_spawn && strcmp(bypass_spawn, "TRUE") == 0) {
            cmd.append(bsoncxx::builder::basic::kvp("mongocryptdBypassSpawn", true));
        }

        if (mongocryptd_path) {
            cmd.append(bsoncxx::builder::basic::kvp("mongocryptdSpawnPath", mongocryptd_path));
        }

        auto_encrypt_opts.extra_options({cmd.extract()});
    }

    if (key_vault_client) {
	auto_encrypt_opts.key_vault_client(key_vault_client);
    }

    client_opts->auto_encryption_opts(std::move(auto_encrypt_opts));
}

void _add_cse_opts(options::client_encryption* opts, class client* client = nullptr) {
    // KMS providers
    auto kms = _make_kms_doc();
    opts->kms_providers(std::move(kms));

    // Key vault client
    if (client) {
	opts->key_vault_client(client);
    }

    // Key vault namespace
    opts->key_vault_namespace({"keyvault", "datakeys"});
}

template <typename Callable>
void run_datakey_and_double_encryption(Callable create_data_key,
                                       stdx::string_view provider,
                                       client* setup_client,
                                       client* client_encrypted,
                                       client_encryption* client_encryption,
                                       mongocxx::spec::apm_checker* apm_checker) {
    // Test creating and using data keys:

    // 1. Call client_encryption.createDataKey()
    auto datakey_id = create_data_key();

    // Expect a BSON binary with subtype 4 to be returned, referred to as datakey_id.
    REQUIRE(datakey_id.view().type() == bsoncxx::type::k_binary);
    REQUIRE(datakey_id.view().get_binary().sub_type == bsoncxx::binary_sub_type::k_uuid);

    // Use client to run a find on keyvault.datakeys by querying with the _id
    // set to the datakey_id.
    auto datakeys = setup_client->database("keyvault").collection("datakeys");
    auto query = make_document(kvp("_id", datakey_id));
    auto cursor = datakeys.find(query.view());

    // Expect that exactly one document is returned with the correct "masterKey.provider"
    std::size_t i = 0;
    for (auto&& doc : cursor) {
        REQUIRE(doc["masterKey"]["provider"].get_utf8().value == provider);
        i++;
    }

    REQUIRE(i == 1);

    // Check that client captured a command_started event for the insert command containing a
    // majority writeConcern.
    auto event = document{} << "command_started_event" << open_document << "command"
                            << open_document << "insert"
                            << "datakeys"
                            << "writeConcern" << open_document << "w"
                            << "majority" << close_document << close_document << close_document
                            << finalize;

    auto arr = bsoncxx::builder::basic::array{};
    arr.append(event);
    apm_checker->has(arr.view());
    apm_checker->clear();

    // 2. Call client_encryption.encrypt() with the value "hello there", the algorithm
    // AEAD_AES_256_CBC_HMAC_SHA_512-Deterministic, and the key_id of datakey_id
    auto to_encrypt_doc = make_document(kvp("v", "hello there"));
    auto to_encrypt = to_encrypt_doc.view();

    options::encrypt opts{};
    opts.algorithm(options::encrypt::encryption_algorithm::k_deterministic);
    opts.key_id(datakey_id.view().get_binary());

    auto encrypted_val = client_encryption->encrypt(to_encrypt["v"].get_value(), opts);

    // Expect the return value to be a BSON binary subtype 6, referred to as encrypted
    auto encrypted = encrypted_val.view();
    REQUIRE(encrypted.type() == bsoncxx::type::k_binary);
    REQUIRE(encrypted.get_binary().sub_type == bsoncxx::binary_sub_type::k_encrypted);

    // Use client_encrypted to insert { _id: provider, "value": <encrypted> } into db.coll
    auto insert_doc = make_document(kvp("_id", provider), kvp("value", encrypted));
    client_encrypted->database("db").collection("coll").insert_one(insert_doc.view());

    // Use client_encrypted to run a find querying with _id of provider and expect value
    // to be "hello there"
    auto filter = make_document(kvp("_id", provider));
    auto res = client_encrypted->database("db").collection("coll").find_one(filter.view());
    REQUIRE(res);
    auto decrypted_bson_val = res->view()["value"];
    REQUIRE(decrypted_bson_val.type() == bsoncxx::type::k_utf8);
    REQUIRE(decrypted_bson_val.get_utf8().value == stdx::string_view{"hello there"});

    // 3. Call client_encryption.encrypt() with the value "hello there", the algorithm
    // AEAD_AES_256_CBC_HMAC_SHA_512-Deterministic, and the key_alt_name of provider_altname
    options::encrypt opts2{};
    opts2.algorithm(options::encrypt::encryption_algorithm::k_deterministic);
    std::string altname{provider};
    altname += "_altname";
    opts2.key_alt_name(altname);

    auto encrypted_val2 = client_encryption->encrypt(to_encrypt["v"].get_value(), opts2);

    // Expect the return value to be a BSON binary subtype 6. Expect the value to exactly
    // match the value of "encrypted"
    auto encrypted2 = encrypted_val2.view();
    REQUIRE(encrypted2.type() == bsoncxx::type::k_binary);
    REQUIRE(encrypted2.get_binary().sub_type == bsoncxx::binary_sub_type::k_encrypted);
    REQUIRE(encrypted == encrypted2);

    // Then, test explicit encryption on an auto encrypted field:
    // Use client_encrypted to attempt to insert { "encrypted_placeholder": (encrypted2) }
    // Expect an exception to be thrown, since this is an attempt to auto encrypt an already
    // encrypted value.
    auto double_encrypted = make_document(kvp("encrypted_placeholder", encrypted2));
    REQUIRE_THROWS(
        client_encrypted->database("db").collection("coll").insert_one(double_encrypted.view()));
}

TEST_CASE("Datakey and double encryption", "[client_side_encryption]") {
    // Setup
    // 1. Create a mongoclient without encryption
    options::client client_opts;
    spec::apm_checker apm_checker;
    client_opts.apm_opts(apm_checker.get_apm_opts(true /* command_started_events_only */));

    class client setup_client {
        uri{}, std::move(client_opts)
    };

    // 2. Drop keyvault.datakeys and db.coll
    _setup_drop_collections(setup_client);

    // 3. Create and configure client_encrypted, client_encryption.
    auto schema_map = document{} << "db.coll" << open_document << "bsonType"
                                 << "object"
                                 << "properties" << open_document << "encrypted_placeholder"
                                 << open_document << "encrypt" << open_document << "keyId"
                                 << "/placeholder"
                                 << "bsonType"
                                 << "string"
                                 << "algorithm"
                                 << "AEAD_AES_256_CBC_HMAC_SHA_512-Random" << close_document
                                 << close_document << close_document << close_document << finalize;    

    options::client encrypted_client_opts;
    _add_client_encrypted_opts(&encrypted_client_opts, _make_kms_doc(), std::move(schema_map));
    class client client_encrypted {
        uri{}, std::move(encrypted_client_opts)
    };

    // Configure both with aws and local KMS providers, and schema map
    options::client_encryption cse_opts;
    _add_cse_opts(&cse_opts, &setup_client);
    client_encryption client_encryption{std::move(cse_opts)};

    // Run test with local
    run_datakey_and_double_encryption(
        [&]() {
            // 1. Call client_encryption.createDataKey() with the local KMS provider
            // and keyAltNames set to ["local_altname"].
            options::data_key data_key_opts;
            data_key_opts.key_alt_names({"local_altname"});
            return client_encryption.create_data_key("local", data_key_opts);

        },
        "local",
        &setup_client,
        &client_encrypted,
        &client_encryption,
        &apm_checker);

    // Run with AWS
    run_datakey_and_double_encryption(
        [&]() {
            // 1. Call client_encryption.createDataKey() with the local KMS provider
            // and keyAltNames set to ["aws_altname"].
            options::data_key data_key_opts;
            data_key_opts.key_alt_names({"aws_altname"});

            auto doc = make_document(
                kvp("region", "us-east-1"),
                kvp("key",
                    "arn:aws:kms:us-east-1:579766882180:key/89fcc2c4-08b0-4bd9-9f25-e30687b580d0"));
            data_key_opts.master_key(doc.view());

            return client_encryption.create_data_key("aws", data_key_opts);

        },
        "aws",
        &setup_client,
        &client_encrypted,
        &client_encryption,
        &apm_checker);
}

    void run_external_key_vault_test(bool with_external_key_vault) {
	// Create a MongoClient without encryption enabled (referred to as client).
	class client client{uri{}};

	// Using client, drop the collections keyvault.datakeys and db.coll.
	_setup_drop_collections(client);

	// Insert the document external/external-key.json into keyvault.datakeys.
	auto external_key = _doc_from_file("/external/external-key.json");
	auto external_schema = _doc_from_file("/external/external-schema.json");

	auto datakeys = client["keyvault"]["datakeys"];
	datakeys.insert_one(external_key.view());
	auto schema_map = document{} << "db"
			             << open_document
				     << "coll"
				     << external_schema
				     << close_document
				     << finalize;
	
	// Create the following:
	//
	// - A MongoClient configured with auto encryption (referred to as client_encrypted)
	// - A ClientEncryption object (referred to as client_encryption)
	//
	// If withExternalKeyVault == true, configure both objects with an external
	// key vault client. The external client MUST connect to the same MongoDB
	// cluster that is being tested against, except it MUST use the username
	// fake-user and password fake-pwd.
	class client external_key_vault_client{uri{"mongodb://fake-user:fake-pwd@localhost:27017"}};

	// A MongoClient configured with auto encryption (referred to as client_encrypted)
	options::client encrypted_client_opts;
	if (with_external_key_vault) {
	    _add_client_encrypted_opts(&encrypted_client_opts,
				       std::move(schema_map),
				       _make_kms_doc(),
				       &external_key_vault_client);
	} else {
	    _add_client_encrypted_opts(&encrypted_client_opts, std::move(schema_map));
	}
	class client client_encrypted {
				       uri{}, std::move(encrypted_client_opts)
	};

	// A ClientEncryption object (referred to as client_encryption)
	options::client_encryption cse_opts;
	if (with_external_key_vault) {
	    _add_cse_opts(&cse_opts, &external_key_vault_client);
	} else {
	    _add_cse_opts(&cse_opts);
	}

	client_encryption client_encryption{std::move(cse_opts)};

	// Use client_encrypted to insert the document {"encrypted": "test"} into db.coll.
	auto doc = make_document(kvp("encrypted", "test"));
	auto coll = client_encrypted["db"]["coll"];


	// If withExternalKeyVault == true, expect an authentication exception to be thrown.
	// Otherwise, expect the insert to succeed.
	if (with_external_key_vault) {
	    REQUIRE_THROWS(coll.insert_one(doc.view()));
	} else {
	    coll.insert_one(doc.view());
	}

	// Use client_encryption to explicitly encrypt the string "test" with key ID
	// LOCALAAAAAAAAAAAAAAAAA== and deterministic algorithm. If withExternalKeyVault == true,
	// expect an authentication exception to be thrown. Otherwise, expect the insert to succeed.

	options::encrypt opts;
	auto key_id = external_key.view()["_id"].get_binary();
	opts.key_id(key_id);
	
	auto test_doc = make_document(kvp("v", "test"));
	auto value = test_doc.view()["v"].get_value();
	if (with_external_key_vault) {
	    REQUIRE_THROWS(client_encryption.encrypt(value, opts));
	} else {
	    auto res = client_encryption.encrypt(value, opts);
	}
    }

TEST_CASE("External key vault", "[client_side_encryption]") {
    run_external_key_vault_test(false);
    run_external_key_vault_test(true);
}

TEST_CASE("BSON size limits and batch splitting", "[client_side_encryption]") {
    // Create a MongoClient without encryption enabled (referred to as client).
    class client client {uri{}};

    // Load in json schema limits/limits-schema.json and limits/limits-key.json
    auto limits_schema = _doc_from_file("/limits/limits-schema.json");
    auto limits_key = _doc_from_file("limits/limits-key.json");

    // Using client, drop db.coll and keyvault.datakeys.
    _setup_drop_collections(&client);

    // Create the collection db.coll configured with the JSON schema limits/limits-schema.json.
    auto db = client["db"];
    options::create_collection create_opts;
    validation_criteria validation{};
    auto cmd = document{} << "create" << "coll"
			  << "validator" << open_document
			  << "$jsonSchema" << limits_schema.view()
			  << close_document << finalize;
    db.run_command(cmd.view());
    
    // Insert the document limits/limits-key.json into keyvault.datakeys.
    auto datakeys = client["keyvault"]["datakeys"];
    datakeys.insert_one(limits_key.view());

    // Create a MongoClient configured with auto encryption (referred to as client_encrypted),
    // with local KMS provider as follows:
    //     { "local": { "key": <base64 decoding of LOCAL_MASTERKEY> } }
    // and with the keyVaultNamespace set to keyvault.datakeys.
    char key_storage[96];
    memcpy(&(key_storage[0]), kLocalMasterKey, 96);
    bsoncxx::types::b_binary local_master_key{bsoncxx::binary_sub_type::k_binary, 96, (const uint8_t*)&key_storage};

    auto kms_doc = document{} << "local" << open_document
			      << "key" << local_master_key
			      << close_document << finalize;

    options::client client_encrypted_opts;
    _add_client_encrypted_opts(&client_encrypted_opts, std::move(kms_doc), limits_schema);
    class client client_encrypted{uri{}, std::move(client_encrypted_opts)};

    // Using client_encrypted perform the following operations:
    // Insert { "_id": "over_2mib_under_16mib",
    //          "unencrypted": <the string "a" repeated 2097152 times> }.
    std::string over_2mib_under_16mib{2097152, "a"};
    auto over_2mib_under_16mib_doc = make_document(kvp("_id", "over_2mib_under_16mib"),
						   kvp("unencrypted", over_2mib_under_16mib));

    // Expect this to succeed since this is still under the maxBsonObjectSize limit.
    auto coll = client_encrypted["db"]["coll"];
    coll.insert_one(over_2mib_under_16mib_doc.view());

    // Insert the document limits/limits-doc.json concatenated with
    // { "_id": "encryption_exceeds_2mib",
    //   "unencrypted": < the string "a" repeated (2097152 - 2000) times > }
    // Note: limits-doc.json is a 1005 byte BSON document that encrypts to a ~10,000 byte document.
    std::string encryption_exceeds_2mib{2097152 - 2000, "a"};
    auto limits_doc = _doc_from_file("/limits/limits-doc.json");
    auto encryption_exceeds_2mib_doc = document{} << "_id" << "encryption_exceeds_2mib"
						  << "unencrypted" << encryption_exceeds_2mib
						  << concatenate(limits_doc.view())
						  << finalize;

    // Expect this to succeed since after encryption this still is below the normal
    // maximum BSON document size. Note, before auto encryption this document is under
    // the 2 MiB limit. After encryption it exceeds the 2 MiB limit, but does NOT exceed
    // the 16 MiB limit.
    coll.insert_one(encryption_exceeds_2mib_doc.view());

    // Bulk insert the following:
    // { "_id": "over_2mib_1", "unencrypted": <the string "a" repeated (2097152) times> }
    // { "_id": "over_2mib_2", "unencrypted": <the string "a" repeated (2097152) times> }
    auto over_2mib_1 = document{} << "_id" << "over_2mib_1"
				  << "unencrypted" << over_2mib_under_16mib
				  << finalize;
    auto over_2mib_2 = document{} << "_id" << "over_2mib_2"
				  << "unencrypted" << over_2mib_under_16mib
				  << finalize;

    std::vector<bsoncxx::document::view> docs;
    docs.push_back(std::move(over_2mib_1));
    docs.push_back(std::move(over_2mib_2));

    // Expect the bulk write to succeed and split after first doc (i.e. two inserts occur).
    // Note: C++ driver does not verify using command monitoring, as splitting logic is handled by
    // the C driver.
    auto res = coll.insert_many(docs);
    REQUIRE(res.inserted_count() == 2);

    // Bulk insert the following:

    // The document limits/limits-doc.json concatenated with
    // { "_id": "encryption_exceeds_2mib_1",
    //   "unencrypted": < the string "a" repeated (2097152 - 2000) times > }
    // The document limits/limits-doc.json concatenated with
    // { "_id": "encryption_exceeds_2mib_2",
    //   "unencrypted": < the string "a" repeated (2097152 - 2000) times > }
    docs.clear();
    auto concat_1 = document{} << "_id" << "encryption_exceeds_2mib_1"
			       << "unencrypted" << encryption_exceeds_2mib
			       << concatenate(limits_doc.view())
			       << finalize;
    auto concat_2 = document{} << "_id" << "encryption_exceeds_2mib_2"
			       << "unencrypted" << encryption_exceeds_2mib
			       << concatenate(limits_doc.view())
			       << finalize;
    
    // Expect the bulk write to succeed.
    res = coll.insert_many(docs);
    REQUIRE(res.inserted_count() == 2);

    // Insert { "_id": "under_16mib", "unencrypted": <the string "a" repeated 16777216 - 2000 times> }
    std::string under_16mib{16777216 - 2000, "a"};
    auto under_16mib_doc = document{} << "_id" << "under_16mib"
				      << "unencrypted" << under_16mib << finalize;

    // Expect this to succeed since this is still (just) under the maxBsonObjectSize limit.
    coll.insert_one(under_16mib_doc.view());

    // Insert the document limits/limits-doc.json concatenated with
    // { "_id": "encryption_exceeds_16mib",
    //   "unencrypted": < the string "a" repeated (16777216 - 2000) times > }
    auto encryption_exceeds_16mib << document{} << "_id" << "encryption_exceeds_16mib"
				  << "unencrypted" << under_16mib
				  << concatenate(limits_doc.view())
				  << finalize;

    // Expect this to fail since encryption results in a document exceeding
    // the maxBsonObjectSize limit.
    REQUIRE_THROWS(coll.insert_one(encryption_exceeds_16mib));

    // Optionally, if it is possible to mock the maxWriteBatchSize (i.e. the maximum number of documents in a batch) test that setting maxWriteBatchSize=1 and inserting the two documents { "_id": "a" }, { "_id": "b" } with client_encrypted splits the operation into two inserts. */
}

TEST_CASE("Views are prohibited", "[client_side_encryption]") {
    // TODO CXX-2021
}

TEST_CASE("Corpus", "[client_side_encryption]") {
    // TODO CXX-2021
}

TEST_CASE("Custom endpoint", "[client_side_encryption]") {
    // TODO CXX-2021
}

TEST_CASE("Bypass spawning mongocryptd", "[client_side_encryption]") {
    // TODO CXX-2021
}

}  // namespace
