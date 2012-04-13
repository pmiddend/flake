#ifndef FLAKE_VOLUME_OBSTACLES_MANAGER_HPP_INCLUDED
#define FLAKE_VOLUME_OBSTACLES_MANAGER_HPP_INCLUDED

#include <flake/volume/model/manager_fwd.hpp>
#include <flake/volume/model/object_fwd.hpp>
#include <flakelib/splatter/object_fwd.hpp>
#include <flakelib/volume/boundary_buffer_view.hpp>
#include <sge/parse/json/array_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <fcppt/config/external_end.hpp>


namespace flake
{
namespace volume
{
namespace obstacles
{
// Tasks:
//
// - Read obstacles from json file (type/position).
// - Create models from it
// - Update the boundary accordingly
// 	- This has to be flexible: With the cube models, we can use the splatter.
// 	- If we want arbitrary models, we can ask the model::manager for the model data and update the boundary
// 	  using that.
class manager
{
FCPPT_NONCOPYABLE(
	manager);
public:
	explicit
	manager(
		flake::volume::model::manager &,
		sge::parse::json::array const &,
		flakelib::volume::boundary_buffer_view const &,
		flakelib::splatter::object &);

	~manager();
private:
	typedef
	boost::ptr_vector<flake::volume::model::object>
	model_sequence;

	model_sequence models_;
};
}
}
}

#endif

