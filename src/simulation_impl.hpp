#ifndef FLAKE_SIMULATION_IMPL_HPP_INCLUDED
#define FLAKE_SIMULATION_IMPL_HPP_INCLUDED

#include "vector2_grid2.hpp"
#include "scalar.hpp"
#include "scalar_grid2.hpp"
#include "source.hpp"
#include "vector2.hpp"
#include "diffusion_coefficient.hpp"
#include "density_sprite/system.hpp"
#include "density_sprite/object.hpp"
#include <sge/sprite/intrusive/system_impl.hpp>
#include <sge/sprite/object_impl.hpp>
#include <sge/systems/instance_fwd.hpp>
#include <sge/parse/json/object_fwd.hpp>
#include <sge/line_drawer/object.hpp>
#include <fcppt/math/vector/basic_impl.hpp>
#include <fcppt/container/grid/object_impl.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/signal/scoped_connection.hpp>

namespace flake
{
class simulation_impl
{
FCPPT_NONCOPYABLE(
	simulation_impl);
public:
	explicit
	simulation_impl(
		sge::parse::json::object const &,
		sge::systems::instance const &);

	void
	update();

	void
	render();

	~simulation_impl();
private:
	typedef
	flake::source<flake::scalar_grid2>
	density_source;

	typedef
	std::vector<density_source>
	density_source_sequence;

	sge::systems::instance const &systems_;
	sge::line_drawer::object line_drawer_;
	flake::vector2 const cell_size_;
	flake::vector2 const field_position_;
	flake::scalar const arrow_length_;
	flake::diffusion_coefficient diffusion_coefficient_;
	flake::vector2_grid2 main_grid_;
	// A ptr_array would be very ugly.
	flake::scalar_grid2 density_grid_store_0_;
	flake::scalar_grid2 density_grid_store_1_;
	// Again, a ptr_array would be ugly
	flake::scalar_grid2 *density_grid_view_0_;
	flake::scalar_grid2 *density_grid_view_1_;
	density_source_sequence density_sources_;
	density_sprite::system density_sprite_system_;
	density_sprite::object density_sprite_;
	fcppt::signal::scoped_connection density_input_connection_;

	void
	density_callback();
};
}

#endif
