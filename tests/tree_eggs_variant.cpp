#include <eggs/variant.hpp>
#include <silicium/config.hpp>
#include <ostream>
#include "catch.hpp"

namespace
{
	struct add;
	struct reference;
	struct literal;

	typedef eggs::variant<add, reference, literal> expression;

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

	void print(std::ostream &out, expression const &root);

	struct expression_printer
	{
		typedef void result_type;

		std::ostream &out;

		void operator()(add const &add_) const
		{
			out << '(';
			print(out, *add_.left);
			out << " + ";
			print(out, *add_.right);
			out << ')';
		}

		void operator()(reference const &reference_) const
		{
			out << reference_.name;
		}

		void operator()(literal const &literal_) const
		{
			out << literal_.value;
		}
	};

	void print(std::ostream &out, expression const &root)
	{
		eggs::variants::apply(expression_printer{out}, root);
	}
}

TEST_CASE("tree of eggs::variant", "tree")
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
