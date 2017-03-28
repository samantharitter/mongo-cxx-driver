// Copyright 2017 MongoDB Inc.
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

#include <cstddef>
#include <cstdint>
#include <memory>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/types/value.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/stdx.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace gridfs {

///
/// Class used to download a GridFS file.
///
class MONGOCXX_API downloader {
   public:
    ///
    /// Default constructs a downloader object. The downloader is equivalent to the state of a moved
    /// from downloader. The only valid actions to take with a default constructed downloader are to
    /// assign to it, or destroy it.
    ///
    downloader() noexcept;

    ///
    /// Move constructs a downloader.
    ///
    downloader(downloader&&) noexcept;

    ///
    /// Move assigns a downloader.
    ///
    downloader& operator=(downloader&&) noexcept;

    downloader(const downloader&) = delete;

    downloader& operator=(const downloader&) = delete;

    ///
    /// Destroys a downloader.
    ///
    ~downloader();

    ///
    /// Returns true if the downloader is valid, meaning it was not default constructed or moved
    /// from.
    ///
    explicit operator bool() const noexcept;

    ///
    /// Reads a specified number of bytes from the GridFS file being downloaded.
    ///
    /// @param length
    ///   The number of bytes to read from the file.
    ///
    /// @param buffer
    ///   A pointer to a buffer to store the bytes read from the file.
    ///
    /// @return
    ///   The number of bytes actually read. If zero, the downloader has reached the end of the
    ///   file.
    ///
    /// @throws an exception if an invalid chunk is downloaded, if the file ends unexpectedly, or if
    /// the stream has been closed.
    ///
    std::size_t read(std::size_t length, std::uint8_t* buffer);

    ///
    /// Closes the downloader stream.
    ///
    /// @throws if the downloader stream was already closed.
    ///
    void close();

    ///
    /// Gets the chunk size of the file being downloaded.
    ///
    /// @return
    ///   The chunk size in bytes.
    ///
    std::int32_t chunk_size() const;

    ///
    /// Gets the length of the file being downloaded.
    ///
    /// @return
    ///   The length in bytes.
    ///
    std::int64_t file_length() const;

    ///
    /// Gets the files collection document of the file being downloaded.
    ///
    /// @return
    ///    A view to the files collection document of the file being downloaded.
    ///
    bsoncxx::document::view files_document() const;

   private:
    friend class bucket;

    //
    // Constructs a new downloader stream.
    //
    // @param chunks
    //   The cursor to read the chunks of the file from. It must have a value if the length of the
    //   file is non-zero.
    //
    // @param files_doc
    //   The files collection document of the file being downloaded.
    //
    MONGOCXX_PRIVATE downloader(stdx::optional<cursor> chunks, bsoncxx::document::value files_doc);

    MONGOCXX_PRIVATE void fetch_chunk();

    class MONGOCXX_PRIVATE impl;

    MONGOCXX_PRIVATE impl& _get_impl();
    MONGOCXX_PRIVATE const impl& _get_impl() const;

    std::unique_ptr<impl> _impl;
};

}  // namespace gridfs
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
