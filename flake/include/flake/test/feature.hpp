#ifndef FLAKE_TEST_FEATURE_HPP_INCLUDED
#define FLAKE_TEST_FEATURE_HPP_INCLUDED

#include <flake/test/json_identifier.hpp>
#include <sge/input/keyboard/optional_key_code.hpp>

namespace flake
{
namespace test
{
class feature
{
public:
	feature(
		test::json_identifier const &,
		sge::input::keyboard::optional_key_code const &);

	test::json_identifier const
	json_identifier() const;

	sge::input::keyboard::optional_key_code const
	optional_key_code() const;

	bool
	is_active() const;

	void
	is_active(
		bool);
private:
	test::json_identifier json_identifier_;
	sge::input::keyboard::optional_key_code optional_key_code_;
	bool is_active_;
};
}
}

#endif

