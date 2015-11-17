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
    auto empty = document{} << finalize;
    auto doc = document{} << "a" << 1 << finalize;
    auto json = to_json(doc.view());

    SECTION("can be constructed with a view") {
        view_or_value variant{doc.view()};

        SECTION("can be used as a view") {
            REQUIRE(to_json(variant) == json);
        }

        SECTION("can be copy constructed") {
            view_or_value copied{variant};
            REQUIRE(to_json(copied) == json);
        }

        SECTION("can be copy assigned") {
            view_or_value copied{empty.view()};
            {
                view_or_value temp{doc.view()};
                copied = temp;
            }
            REQUIRE(to_json(copied) == json);
        }

        SECTION("can be move constructed") {
            view_or_value moved{std::move(variant)};
            REQUIRE(to_json(moved) == json);
        }

        SECTION("can be move assigned") {
            view_or_value moved{empty.view()};
            {
                view_or_value temp{doc.view()};
                moved = std::move(temp);
            }
            REQUIRE(to_json(moved) == json);
        }
    }

    SECTION("can be constructed with a value") {
        auto move_doc = doc;
        view_or_value variant{std::move(move_doc)};

        SECTION("can be used as a view") {
            REQUIRE(to_json(variant) == json);
        }

        SECTION("can be copy constructed") {
            view_or_value copied{variant};
            REQUIRE(to_json(copied) == json);
        }

        SECTION("can be copy assigned") {
            view_or_value copied{empty};
            {
                auto temp_doc = doc;
                view_or_value temp{std::move(temp_doc)};
                copied = temp;
            }
            REQUIRE(to_json(copied) == json);
        }

        SECTION("can be move constructed") {
            view_or_value moved{std::move(variant)};
            REQUIRE(to_json(moved) == json);
        }

        SECTION("can be move assigned") {
            view_or_value moved{empty};
            {
                auto temp_doc = doc;
                view_or_value temp{std::move(temp_doc)};
                moved = std::move(temp);
            }
            REQUIRE(to_json(moved) == json);
        }
    }
}
