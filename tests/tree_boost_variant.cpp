#include <boost/variant.hpp>
#include <silicium/config.hpp>
#include <ostream>
#include "catch.hpp"

namespace
{
	struct add;
	struct reference;
	struct literal;

	typedef boost::variant<std::unique_ptr<add>, reference, literal> expression;

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

	void print(std::ostream &out, expression const &root);

	struct expression_printer
	{
		typedef void result_type;

		std::ostream &out;

		void operator()(std::unique_ptr<add> const &add_) const
		{
			out << '(';
			print(out, add_->left);
			out << " + ";
			print(out, add_->right);
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
		boost::apply_visitor(expression_printer{out}, root);
	}
}

TEST_CASE("tree of boost::variant", "tree")
{
	expression right = Si::make_unique<add>(reference{"b"}, literal{1});
	expression root = Si::make_unique<add>(reference{"a"}, std::move(right));
	std::ostringstream buffer;
	print(buffer, root);
	CHECK("(a + (b + 1))" == buffer.str());
}
