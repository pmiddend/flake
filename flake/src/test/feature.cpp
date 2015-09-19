#include <flake/test/feature.hpp>

flake::test::feature::feature(
	test::json_identifier const &_json_identifier,
	sge::input::key::optional_code const &_optional_key_code)
:
	json_identifier_(
		_json_identifier),
	optional_key_code_(
		_optional_key_code),
	is_active_(
		false)
{
}

flake::test::json_identifier const
flake::test::feature::json_identifier() const
{
	return
		json_identifier_;
}

sge::input::key::optional_code const
flake::test::feature::optional_key_code() const
{
	return
		optional_key_code_;
}

bool
flake::test::feature::is_active() const
{
	return
		is_active_;
}

void
flake::test::feature::is_active(
	bool const _is_active)
{
	is_active_ =
		_is_active;
}
