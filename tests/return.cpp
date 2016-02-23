#include <silicium/variant.hpp>
#include <boost/filesystem/operations.hpp>
#include "catch.hpp"

namespace
{
	Si::variant<boost::uintmax_t, boost::system::error_code> file_size(boost::filesystem::path const &file)
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

TEST_CASE("return Si::variant", "file_size")
{
	Si::visit<void>(file_size(__FILE__),
	                [](boost::uintmax_t size)
	                {
		                CHECK(783 == size);
		            },
	                [](boost::system::error_code ec)
	                {
		                CHECK(boost::system::error_code() == ec);
		                FAIL("Expected success");
		            });
}
