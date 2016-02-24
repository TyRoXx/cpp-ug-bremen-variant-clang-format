#include "x-www-form-urlencoded.hpp"
#include "catch.hpp"
#include <silicium/source/memory_source.hpp>

namespace
{
	template <class First, class... T>
	First expect_first(Si::variant<First, T...> &&variant)
	{
		First *const expected = Si::try_get_ptr<First>(variant);
		if (!expected)
		{
			throw std::logic_error("expected first variant element");
		}
		return std::move(*expected);
	}
}

TEST_CASE("x_www_form_urlencoded_empty_input", "")
{
	std::vector<std::pair<std::string, std::string>> const expected{};
	std::string const encoded = "";
	std::vector<std::pair<std::string, std::string>> const parsed =
	    expect_first(
	        demo::parse_x_www_form_encoded(Si::make_container_source(encoded)));
	CHECK(expected == parsed);
}

TEST_CASE("x_www_form_urlencoded_empty_value", "")
{
	std::vector<std::pair<std::string, std::string>> const expected{
	    {"key", ""}};
	std::string const encoded = "key=";
	std::vector<std::pair<std::string, std::string>> const parsed =
	    expect_first(
	        demo::parse_x_www_form_encoded(Si::make_container_source(encoded)));
	CHECK(expected == parsed);
}

TEST_CASE("x_www_form_urlencoded_empty_key", "")
{
	std::vector<std::pair<std::string, std::string>> const expected{
	    {"", "value"}};
	std::string const encoded = "=value";
	std::vector<std::pair<std::string, std::string>> const parsed =
	    expect_first(
	        demo::parse_x_www_form_encoded(Si::make_container_source(encoded)));
	CHECK(expected == parsed);
}

TEST_CASE("x_www_form_urlencoded_simple", "")
{
	std::vector<std::pair<std::string, std::string>> const expected{
	    {"key", "value"}};
	std::string const encoded = "key=value";
	std::vector<std::pair<std::string, std::string>> const parsed =
	    expect_first(
	        demo::parse_x_www_form_encoded(Si::make_container_source(encoded)));
	CHECK(expected == parsed);
}

TEST_CASE("x_www_form_urlencoded_trailing_ampersand", "")
{
	std::vector<std::pair<std::string, std::string>> const expected{
	    {"key", "value"}};
	std::string const encoded = "key=value&";
	std::vector<std::pair<std::string, std::string>> const parsed =
	    expect_first(
	        demo::parse_x_www_form_encoded(Si::make_container_source(encoded)));
	CHECK(expected == parsed);
}

TEST_CASE("x_www_form_urlencoded_space", "")
{
	std::vector<std::pair<std::string, std::string>> const expected{
	    {"key", "value "}};
	std::string const encoded = "key=value+";
	std::vector<std::pair<std::string, std::string>> const parsed =
	    expect_first(
	        demo::parse_x_www_form_encoded(Si::make_container_source(encoded)));
	CHECK(expected == parsed);
}

TEST_CASE("x_www_form_urlencoded_hex_ok", "")
{
	std::vector<std::pair<std::string, std::string>> const expected{
	    {"key", "value "}};
	std::string const encoded = "key=value%20";
	std::vector<std::pair<std::string, std::string>> const parsed =
	    expect_first(
	        demo::parse_x_www_form_encoded(Si::make_container_source(encoded)));
	CHECK(expected == parsed);
}

TEST_CASE("x_www_form_urlencoded_hex_first_non_digit", "")
{
	std::string const encoded = "key=value%x";
	Si::visit<void>(
	    demo::parse_x_www_form_encoded(Si::make_container_source(encoded)),
	    [](demo::x_www_form_key_value_pairs const &)
	    {
		    REQUIRE(!"syntax error expected");
		},
	    [](demo::x_www_form_syntax_error)
	    {
		});
}

TEST_CASE("x_www_form_urlencoded_hex_second_non_digit", "")
{
	std::string const encoded = "key=value%2x";
	Si::visit<void>(
	    demo::parse_x_www_form_encoded(Si::make_container_source(encoded)),
	    [](demo::x_www_form_key_value_pairs const &)
	    {
		    REQUIRE(!"syntax error expected");
		},
	    [](demo::x_www_form_syntax_error)
	    {
		});
}

TEST_CASE("x_www_form_urlencoded_hex_first_end_of_input", "")
{
	std::string const encoded = "key=value%";
	Si::visit<void>(
	    demo::parse_x_www_form_encoded(Si::make_container_source(encoded)),
	    [](demo::x_www_form_key_value_pairs const &)
	    {
		    REQUIRE(!"syntax error expected");
		},
	    [](demo::x_www_form_syntax_error)
	    {
		});
}

TEST_CASE("x_www_form_urlencoded_hex_second_end_of_input", "")
{
	std::string const encoded = "key=value%2";
	Si::visit<void>(
	    demo::parse_x_www_form_encoded(Si::make_container_source(encoded)),
	    [](demo::x_www_form_key_value_pairs const &)
	    {
		    REQUIRE(!"syntax error expected");
		},
	    [](demo::x_www_form_syntax_error)
	    {
		});
}

TEST_CASE("x_www_form_urlencoded_complex", "")
{
	std::vector<std::pair<std::string, std::string>> const expected{
	    {"name", "+ \" = ?"},  {"date", "2016-01-23"}, {"purchaser", "Kurt"},
	    {"total_price", "11"}, {"receiver", "on"},     {"category X", "on"}};
	std::string const encoded = "name=%2B+%22+%3D+%3F&date=2016-01-23&"
	                            "purchaser=Kurt&total_price=11&receiver="
	                            "on&category+X=on";
	std::vector<std::pair<std::string, std::string>> const parsed =
	    expect_first(
	        demo::parse_x_www_form_encoded(Si::make_container_source(encoded)));
	CHECK(expected == parsed);
}
