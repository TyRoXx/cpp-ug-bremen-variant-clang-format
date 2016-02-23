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

TEST_CASE("return Si::variant", "")
{
	Si::visit<void>(file_size(__FILE__),
	                [](boost::uintmax_t size)
	                {
		                CHECK(size >= 800);
		            },
	                [](boost::system::error_code ec)
	                {
		                CHECK(boost::system::error_code() == ec);
		                FAIL("Expected success");
		            });
}

TEST_CASE("Si::variant nested visit", "")
{
	Si::variant<std::string, boost::system::error_code> maybe_formatted =
	    Si::visit<Si::variant<std::string, boost::system::error_code>>(
	        file_size(__FILE__),
	        [](boost::uintmax_t size)
	        {
		        return boost::lexical_cast<std::string>(size);
		    },
	        [](boost::system::error_code ec)
	        {
		        return ec;
		    });

	Si::visit<void>(maybe_formatted,
	                [](std::string const &formatted)
	                {
		                CHECK(formatted.size() >= 3);
		            },
	                [](boost::system::error_code ec)
	                {
		                CHECK(boost::system::error_code() == ec);
		                FAIL("Expected success");
		            });
}
