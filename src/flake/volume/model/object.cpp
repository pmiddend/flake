#include <flake/volume/model/manager.hpp>
#include <flake/volume/model/manager.hpp>

flake::volume::model::object::object(
	model::manager &_manager,
	model::identifier const &_identifier,
	model::position const &_position)
:
	identifier_(
		_identifier),
	position_(
		_position)
{
	_manager.add_child(
		*this);
}

flake::volume::model::identifier const &
flake::volume::model::object::identifier() const
{
	return
		identifier_;
}

flake::volume::model::position const &
flake::volume::model::object::position() const
{
	return
		position_;
}

flake::volume::model::object::~object()
{
}
