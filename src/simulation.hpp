#ifndef FLAKE_SIMULATION_HPP_INCLUDED
#define FLAKE_SIMULATION_HPP_INCLUDED

#include "simulation_impl_fwd.hpp"
#include <sge/systems/instance_fwd.hpp>
#include <sge/parse/json/object_fwd.hpp>
#include <fcppt/scoped_ptr.hpp>

namespace flake
{
class simulation
{
FCPPT_NONCOPYABLE(
	simulation);
public:
	explicit
	simulation(
		sge::parse::json::object const &,
		sge::systems::instance const &);

	void
	update();

	void
	render();

	~simulation();
private:
	fcppt::scoped_ptr<simulation_impl> impl_;
};
}

#endif
