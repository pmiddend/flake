#include <flake/volume/model/object.hpp>
#include <flake/volume/obstacles/manager.hpp>
#include <flakelib/buffer/volume_view_impl.hpp>
#include <flakelib/splatter/object.hpp>
#include <flakelib/splatter/box/object.hpp>
#include <flakelib/splatter/box/position.hpp>
#include <flakelib/splatter/box/size.hpp>
#include <flakelib/splatter/pen/object.hpp>
#include <flakelib/splatter/pen/volume.hpp>
#include <sge/parse/json/array.hpp>
#include <sge/parse/json/convert_from.hpp>
#include <sge/parse/json/element_vector.hpp>
#include <sge/parse/json/get.hpp>
#include <sge/parse/json/object.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assert/error_message.hpp>
#include <fcppt/container/ptr/push_back_unique_ptr.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <fcppt/math/vector/structure_cast.hpp>


flake::volume::obstacles::manager::manager(
	flake::volume::model::manager &_model_manager,
	sge::parse::json::array const &_json_obstacles,
	flakelib::volume::boundary_buffer_view const &_boundary,
	flakelib::splatter::object &_splatter)
:
	models_()
{
	for(
		sge::parse::json::element_vector::const_iterator current_obstacle =
			_json_obstacles.elements.begin();
		current_obstacle != _json_obstacles.elements.end();
		++current_obstacle)
	{
		sge::parse::json::object const &current_obstacle_object =
			sge::parse::json::get<sge::parse::json::object>(
				*current_obstacle);

		sge::parse::json::member_map::const_iterator position_member =
			current_obstacle_object.members.find(
				FCPPT_TEXT("left-bottom-back-position"));

		FCPPT_ASSERT_ERROR_MESSAGE(
			position_member != current_obstacle_object.members.end(),
			FCPPT_TEXT("An obstacle without a position found"));

		sge::parse::json::member_map::const_iterator size_member =
			current_obstacle_object.members.find(
				FCPPT_TEXT("bounding-box-size"));

		FCPPT_ASSERT_ERROR_MESSAGE(
			size_member != current_obstacle_object.members.end(),
			FCPPT_TEXT("An obstacle without a bounding-box-size found"));

		flakelib::splatter::box::position const discrete_position(
			sge::parse::json::convert_from<flakelib::splatter::box::position::value_type>(
				position_member->second));

		flakelib::splatter::box::size const discrete_size(
			sge::parse::json::convert_from<flakelib::splatter::box::size::value_type>(
				size_member->second));

		model::position const continuous_position(
			fcppt::math::vector::structure_cast<model::position::value_type>(
				discrete_position.get()) +
			fcppt::math::dim::structure_cast<model::position::value_type>(
				discrete_size.get())/
			static_cast<model::position::value_type::value_type>(
				2));

		fcppt::container::ptr::push_back_unique_ptr(
			models_,
			fcppt::make_unique_ptr<flake::volume::model::object>(
				fcppt::ref(
					_model_manager),
				model::identifier(
					FCPPT_TEXT("cube")),
				continuous_position));

		_splatter.splat_volume_float(
			_boundary.get(),
			flakelib::splatter::pen::volume(
				flakelib::splatter::box::object(
					discrete_position,
					discrete_size),
				flakelib::splatter::pen::is_round(
					false),
				flakelib::splatter::pen::is_smooth(
					false),
				flakelib::splatter::pen::draw_mode::mix,
				flakelib::splatter::pen::blend_factor(
					1.0f)),
			static_cast<cl_float>(
				1.0f));
	}
}

flake::volume::obstacles::manager::~manager()
{
}
