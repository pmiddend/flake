#ifndef FLAKE_TEST_FEATURE_HPP_INCLUDED
#define FLAKE_TEST_FEATURE_HPP_INCLUDED

#include <flake/detail/symbol.hpp>
#include <flake/test/json_identifier.hpp>
#include <sge/input/key/optional_code.hpp>

namespace flake
{
namespace test
{
class feature
{
public:
	FLAKE_DETAIL_SYMBOL
	feature(
		test::json_identifier const &,
		sge::input::key::optional_code const &);

	test::json_identifier const
	json_identifier() const;

	sge::input::key::optional_code const
	optional_key_code() const;

	bool
	is_active() const;

	void
	is_active(
		bool);
private:
	test::json_identifier json_identifier_;
	sge::input::key::optional_code optional_key_code_;
	bool is_active_;
};
}
}

#endif

