// Copyright 2021 MongoDB Inc.
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

#include <mongocxx/config/private/prelude.hh>

#include <mongocxx/private/libbson.hh>
#include <mongocxx/private/libmongoc.hh>
#include <mongocxx/server_api.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

server_api::server_api(server_api_version version) : _version(std::move(version)) {}

    server_api& server_api::strict(bool strict) {
	_strict = strict;
	return *this;
    }
    
    const stdx::optional<bool>& server_api::strict() const {
	return _strict;
    }

    server_api& server_api::deprecation_errors(bool deprecation_errors) {
	_deprecation_errors = deprecation_errors;
	return *this;
    }

    const stdx::optional<bool>& server_api::deprecation_errors() const {
	return _deprecation_errors;
    }

    // namespace {
    // 	mongoc_server_api_version_t _convert_version(server_api_version v) {
	    
    // 	}
    // } // namespace

    void* server_api::convert() const {
	//auto mongoc_version = _convert_version(this->_version);

	auto mongoc_server_api = libmongoc::server_api_new(_version);
	if (!mongoc_server_api) {
	    throw std::logic_error{"could not get object from libmongoc"};
	}

	if (_strict) {
	    libmongoc::server_api_set_strict(mongoc_server_api, _strict);
	}

	if (_deprecation_errors) {
	    libmongoc::server_api_set_deprecation_errors(mongoc_server_api, _deprecation_errors);
	}

	return mongoc_server_api;
    }

    MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
