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

#pragma once

#include <mongocxx/config/prelude.hpp>

#include <bsoncxx/stdx/optional.hpp>

namespace mongocxx {
    MONGOCXX_INLINE_NAMESPACE_BEGIN

    class client;
    class pool;

    ///
    /// Class representing the server API version required by a given client.
    ///
    enum class server_api_version {
				   MONGOCXX_SERVER_API_V1
    };

    ///
    /// Converts the given server_api_version to a string.
    ///
    /// @return
    ///   A string represenatation of the given server api version.
    ///
    std::string server_api_version_to_string (server_api_version);

    ///
    /// Class representing the server API required by a given client.
    ///
    class MONGOCXX_API server_api {
    public:
    ///
    /// Creates a server_api object.
    ///
    /// @param version
    ///   An object representing the server api version.
    ///
    server_api(server_api_version version);

    ///
    /// Destroys a server_api object.
    ///
    ~server_api() noexcept;

    ///
    /// Set whether or not to error if a behavior that is not defined in this
    /// api version is used.
    ///
    /// @param strict
    ///   Whether to receive strict errors for non-api behaviors.
    ///
    /// @return
    ///   A reference to this object to facilitate method chaining.
    ///
    server_api& strict(bool strict);

    ///
    /// Get the current level of strictness.
    ///
    /// @return
    ///   The strict setting.
    ///
    const stdx::optional<bool>& strict() const;

    ///
    /// Set whether to receive errors when behaviors are deprecated in this api version.
    ///
    /// @param deprecation_errors
    ///   Whether or not to receive deprecation errors.
    ///
    /// @return
    ///   A reference to this object to facilitate method chaining.
    ///
    server_api& deprecation_errors(bool deprecation_errors);

    ///
    /// Get the current deprecation error setting.
    ///
    /// @return
    ///   The current deprecation error setting.
    ///
    const stdx::optional<bool>& deprecation_errors() const;

    private:
   friend class mongocxx::client;
   friend class mongocxx::pool;

   MONGOCXX_PRIVATE void* convert() const;

    server_api_version _version;
    stdx::optional<bool> _strict;
    stdx::optional<bool> _deprecation_errors;
    };

    
    MONGOCXX_INLINE_NAMESPACE_END
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
