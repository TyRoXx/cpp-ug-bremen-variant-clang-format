#include <silicium/variant.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>
#include "catch.hpp"

namespace
{
	Si::variant<boost::uintmax_t, boost::system::error_code>
	file_size(boost::filesystem::path const &file)
	{
		boost::system::error_code ec;
		boost::uintmax_t size = boost::filesystem::file_size(file, ec);
		if (!ec)
		{
			return size;
		}
		return ec;
	}
}

using boost::system::error_code;
using std::string;
using boost::uintmax_t;
using Si::variant;
using Si::visit;

TEST_CASE("return Si::variant", "")
{
	visit<void>(file_size(__FILE__),
	            [](uintmax_t size)
	            {
		            CHECK(size >= 800);
		        },
	            [](error_code ec)
	            {
		            CHECK(error_code() == ec);
		            FAIL("Expected success");
		        });
}

TEST_CASE("Si::variant nested visit", "")
{
	variant<string, error_code> maybe_formatted =
	    visit<variant<string, error_code>>(
	        file_size(__FILE__),
	        [](uintmax_t size)
	        {
		        return boost::lexical_cast<string>(size);
		    },
	        [](error_code ec)
	        {
		        return ec;
		    });

	visit<void>(maybe_formatted,
	            [](string const &formatted)
	            {
		            CHECK(formatted.size() >= 3);
		        },
	            [](error_code ec)
	            {
		            CHECK(error_code() == ec);
		            FAIL("Expected success");
		        });
}
