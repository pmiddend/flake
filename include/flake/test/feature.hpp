#ifndef FLAKE_TEST_FEATURE_HPP_INCLUDED
#define FLAKE_TEST_FEATURE_HPP_INCLUDED

#include <flake/test/json_identifier.hpp>
#include <flake/test/optional_key_code.hpp>
#include <fcppt/optional_impl.hpp>

namespace flake
{
namespace test
{
class feature
{
public:
	feature(
		test::json_identifier const &,
		test::optional_key_code const &);

	test::json_identifier const
	json_identifier() const;

	test::optional_key_code const
	optional_key_code() const;

	bool
	is_active() const;

	void
	is_active(
		bool);
private:
	test::json_identifier json_identifier_;
	test::optional_key_code optional_key_code_;
	bool is_active_;
};
}
}

#endif

