#include <silicium/variant.hpp>
#include <ostream>
#include "catch.hpp"

namespace
{
	struct add;
	struct reference;
	struct literal;

	typedef Si::variant<add, reference, literal> expression;

	struct add
	{
		std::unique_ptr<expression> left;
		std::unique_ptr<expression> right;
	};

	struct reference
	{
		std::string name;
	};

	struct literal
	{
		std::uint64_t value;
	};

	void print(std::ostream &out, expression const &root)
	{
		Si::visit<void>(root,
		                [&out](add const &add_)
		                {
			                out << '(';
			                print(out, *add_.left);
			                out << " + ";
			                print(out, *add_.right);
			                out << ')';
			            },
		                [&out](reference const &reference_)
		                {
			                out << reference_.name;
			            },
		                [&out](literal const &literal_)
		                {
			                out << literal_.value;
			            });
	}
}

TEST_CASE("tree of Si::variant", "tree")
{
	std::unique_ptr<expression> right = Si::make_unique<expression>(
	    add{Si::make_unique<expression>(reference{"b"}),
	        Si::make_unique<expression>(literal{1})});
	expression root =
	    add{Si::make_unique<expression>(reference{"a"}), std::move(right)};
	std::ostringstream buffer;
	print(buffer, root);
	CHECK("(a + (b + 1))" == buffer.str());
}
