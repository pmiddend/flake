#include <flake/test/information/manager.hpp>
#include <flake/test/information/object.hpp>


flake::test::information::object::object(
	information::manager &_manager,
	information::identifier const &_identifier,
	information::pull_function const &_pull_function)
:
	identifier_(
		_identifier),
	pull_function_(
		_pull_function)
{
	_manager.add_child(
		*this);
}

flake::test::information::identifier const &
flake::test::information::object::identifier() const
{
	return
		identifier_;
}

fcppt::string const
flake::test::information::object::value() const
{
	return
		pull_function_();
}

flake::test::information::object::~object()
{
}
