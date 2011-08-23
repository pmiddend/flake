#ifndef FLAKE_SIMULATION_IMPL_HPP_INCLUDED
#define FLAKE_SIMULATION_IMPL_HPP_INCLUDED

#include "vector2_grid.hpp"
#include "scalar_store.hpp"
#include "scalar_view.hpp"
#include "scalar.hpp"
#include "source_sequence.hpp"
#include "vector2.hpp"
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
	sge::systems::instance const &systems_;
	sge::line_drawer::object line_drawer_;
	flake::vector2 const cell_size_;
	flake::vector2 const field_position_;
	flake::scalar const arrow_length_;
	flake::vector2_grid main_grid_;
	// A ptr_array would be very ugly.
	flake::scalar_store density_grid_store_0_;
	flake::scalar_store density_grid_store_1_;
	// Again, a ptr_array would be ugly
	flake::scalar_view density_grid_view_0_;
	flake::scalar_view density_grid_view_1_;
	flake::source_sequence external_sources_;
	density_sprite::system density_sprite_system_;
	density_sprite::object density_sprite_;
	fcppt::signal::scoped_connection density_input_connection_;

	void
	density_callback();
};
}

#endif
