#include "catch.hpp"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/json.hpp>

using bsoncxx::to_json;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::document::view_or_value;

TEST_CASE("view_or_value", "[bsoncxx::document::view_or_value]") {
    auto doc = document{} << "a" << 1 << finalize;
    auto json = to_json(doc.view());

    SECTION("can be constructed with a view") {
        view_or_value variant(doc.view());

        SECTION("can be used as a view") {
            REQUIRE(to_json(variant) == json);
        }
    }

    SECTION("can be constructed with a value") {
        view_or_value variant(doc);

        SECTION("can be used as a view") {
            REQUIRE(to_json(variant) == json);
        }
    }
}
