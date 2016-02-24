#ifndef X_WWW_FORM_URLENCODED_HPP
#define X_WWW_FORM_URLENCODED_HPP

#include <silicium/source/source.hpp>
#include <silicium/variant.hpp>

namespace demo
{
	inline Si::optional<std::uint_least8_t> decode_hexadecimal_digit(char digit)
	{
		switch (digit)
		{
		case '0':
			return 0;
		case '1':
			return 1;
		case '2':
			return 2;
		case '3':
			return 3;
		case '4':
			return 4;
		case '5':
			return 5;
		case '6':
			return 6;
		case '7':
			return 7;
		case '8':
			return 8;
		case '9':
			return 9;
		case 'a':
		case 'A':
			return 10;
		case 'b':
		case 'B':
			return 11;
		case 'c':
		case 'C':
			return 12;
		case 'd':
		case 'D':
			return 13;
		case 'e':
		case 'E':
			return 14;
		case 'f':
		case 'F':
			return 15;
		default:
			return Si::none;
		}
	}

	struct x_www_form_string_decoder
	{
		struct need_more_input
		{
		};

		struct syntax_error
		{
		};

		typedef Si::variant<char, need_more_input, syntax_error> result;

		result put(char input)
		{
			return Si::visit<result>(
			    m_state,
			    [this, input](base) -> result
			    {
				    switch (input)
				    {
				    case '%':
					    m_state = first_digit();
					    return need_more_input();

				    case '+':
					    return ' ';

				    default:
					    return input;
				    }
				},
			    [this, input](first_digit) -> result
			    {
				    Si::optional<std::uint_least8_t> const digit =
				        decode_hexadecimal_digit(input);
				    if (digit)
				    {
					    m_state = second_digit{*digit};
					    return need_more_input();
				    }
				    else
				    {
					    m_state = base();
					    return syntax_error();
				    }
				},
			    [this, input](second_digit const second) -> result
			    {
				    Si::optional<std::uint_least8_t> const digit =
				        decode_hexadecimal_digit(input);
				    m_state = base();
				    if (digit)
				    {
					    return static_cast<char>((second.first << 4) | *digit);
				    }
				    else
				    {
					    return syntax_error();
				    }
				});
		}

	private:
		struct base
		{
		};

		struct first_digit
		{
		};

		struct second_digit
		{
			std::uint_least8_t first;
		};

		Si::variant<base, first_digit, second_digit> m_state;
	};

	struct x_www_form_submission_decoder
	{
		struct need_more_input
		{
		};

		struct syntax_error
		{
		};

		struct successful_end_of_input
		{
		};

		typedef Si::variant<std::pair<std::string, std::string>,
		                    need_more_input, successful_end_of_input,
		                    syntax_error> result;

		result put(Si::optional<char> c)
		{
			return Si::visit<result>(
			    m_state,
			    [this, c](waiting_for_key) -> result
			    {
				    if (c)
				    {
					    if (*c == '=')
					    {
						    m_state = value();
						    return need_more_input();
					    }
					    key next_state;
					    if (next_state.parsing.parse(*c))
					    {
						    return syntax_error();
					    }
					    m_state = std::move(next_state);
					    return need_more_input();
				    }
				    else
				    {
					    m_state = end_of_input();
					    return successful_end_of_input();
				    }
				},
			    [this, c](key &key) -> result
			    {
				    if (c)
				    {
					    if (*c == '=')
					    {
						    if (!key.parsing.is_complete)
						    {
							    return syntax_error();
						    }
						    value next_state;
						    next_state.key = std::move(key.parsing.decoded);
						    m_state = std::move(next_state);
						    return need_more_input();
					    }
					    if (key.parsing.parse(*c))
					    {
						    return syntax_error();
					    }
					    return need_more_input();
				    }
				    else
				    {
					    if (!key.parsing.is_complete)
					    {
						    return syntax_error();
					    }
					    std::pair<std::string, std::string> result_(
					        std::move(key.parsing.decoded), "");
					    m_state = end_of_input();
					    return std::move(result_);
				    }
				},
			    [this, c](value &value) -> result
			    {
				    if (c)
				    {
					    if (*c == '&')
					    {
						    if (!value.parsing.is_complete)
						    {
							    return syntax_error();
						    }
						    std::pair<std::string, std::string> result_(
						        std::move(value.key), value.parsing.decoded);
						    m_state = waiting_for_key();
						    return std::move(result_);
					    }
					    if (value.parsing.parse(*c))
					    {
						    return syntax_error();
					    }
					    return need_more_input();
				    }
				    else
				    {
					    if (!value.parsing.is_complete)
					    {
						    return syntax_error();
					    }

					    std::pair<std::string, std::string> result_(
					        std::move(value.key),
					        std::move(value.parsing.decoded));
					    m_state = end_of_input();
					    return std::move(result_);
				    }
				},
			    [](end_of_input) -> result
			    {
				    return successful_end_of_input();
				});
		}

	private:
		struct waiting_for_key
		{
		};

		struct parsing_string
		{
			x_www_form_string_decoder decoder;
			std::string decoded;
			bool is_complete = true;

			Si::optional<syntax_error> parse(char input)
			{
				x_www_form_string_decoder::result const last_result =
				    decoder.put(input);
				return Si::visit<Si::optional<syntax_error>>(
				    last_result,
				    [this](char decoded)
				    {
					    this->decoded += decoded;
					    is_complete = true;
					    return Si::none;
					},
				    [this](x_www_form_string_decoder::need_more_input)
				    {
					    is_complete = false;
					    return Si::none;
					},
				    [this](x_www_form_string_decoder::syntax_error)
				    {
					    is_complete = false;
					    return syntax_error();
					});
			}
		};

		struct key
		{
			parsing_string parsing;
		};

		struct value
		{
			std::string key;
			parsing_string parsing;
		};

		struct end_of_input
		{
		};

		Si::variant<waiting_for_key, key, value, end_of_input> m_state;
	};

	struct x_www_form_syntax_error
	{
	};

	typedef std::vector<std::pair<std::string, std::string>>
	    x_www_form_key_value_pairs;

	template <class CharSource>
	Si::variant<x_www_form_key_value_pairs, x_www_form_syntax_error>
	parse_x_www_form_encoded(CharSource &&input)
	{
		x_www_form_key_value_pairs results;
		x_www_form_submission_decoder decoder;
		for (;;)
		{
			Si::optional<char> c = Si::get(input);
			enum class next_step
			{
				syntax_error,
				succeed,
				continue_
			};
			switch (Si::visit<next_step>(
			    decoder.put(c),
			    [&results](std::pair<std::string, std::string> key_value)
			    {
				    results.emplace_back(std::move(key_value));
				    return next_step::continue_;
				},
			    [](x_www_form_submission_decoder::need_more_input)
			    {
				    return next_step::continue_;
				},
			    [](x_www_form_submission_decoder::successful_end_of_input)
			    {
				    return next_step::succeed;
				},
			    [](x_www_form_submission_decoder::syntax_error)
			    {
				    return next_step::syntax_error;
				}))
			{
			case next_step::syntax_error:
				return x_www_form_syntax_error();

			case next_step::succeed:
				return std::move(results);

			case next_step::continue_:
				break;
			}
		}
	}
}

#endif
