#include "simulation.hpp"
#include "simulation_impl.hpp"
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/cref.hpp>

flake::simulation::simulation(
	sge::parse::json::object const &_config_file,
	sge::systems::instance const &_systems)
:
	impl_(
		fcppt::make_unique_ptr<simulation_impl>(
			fcppt::cref(
				_config_file),
			fcppt::cref(
				_systems)))
{
}

void
flake::simulation::update()
{
	impl_->update();
}

void
flake::simulation::render()
{
	impl_->render();
}

flake::simulation::~simulation()
{
}
