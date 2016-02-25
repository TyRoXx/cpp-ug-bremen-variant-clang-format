#include <silicium/variant.hpp>
#include <ostream>
#include "catch.hpp"

namespace
{
	struct add;
	struct reference;
	struct literal;

	typedef Si::variant<std::unique_ptr<add>, reference, literal> expression;

	struct reference
	{
		std::string name;
	};

	struct literal
	{
		std::uint64_t value;
	};

	struct add
	{
		expression left;
		expression right;

		add(expression left, expression right)
		    : left(std::move(left))
		    , right(std::move(right))
		{
		}
	};

	void print(std::ostream &out, expression const &root)
	{
		Si::visit<void>(root,
		                [&out](std::unique_ptr<add> const &add_)
		                {
			                out << '(';
			                print(out, add_->left);
			                out << " + ";
			                print(out, add_->right);
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
	expression right = Si::make_unique<add>(reference{"b"}, literal{1});
	expression root = Si::make_unique<add>(reference{"a"}, std::move(right));
	std::ostringstream buffer;
	print(buffer, root);
	CHECK("(a + (b + 1))" == buffer.str());
}
