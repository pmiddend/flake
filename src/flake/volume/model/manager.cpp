#include <sge/renderer/cg/scoped_texture.hpp>
#include <fcppt/cref.hpp>
#include <flake/media_path_from_string.hpp>
#include <flake/volume/model/manager.hpp>
#include <flake/volume/model/vf/format.hpp>
#include <flake/volume/model/vf/from_obj_map.hpp>
#include <sge/camera/base.hpp>
#include <sge/camera/coordinate_system/object.hpp>
#include <sge/camera/matrix_conversion/world.hpp>
#include <sge/camera/matrix_conversion/world_projection.hpp>
#include <sge/cg/parameter/matrix/set.hpp>
#include <sge/cg/parameter/vector/set.hpp>
#include <sge/cg/program/from_file_parameters.hpp>
#include <sge/model/obj/create.hpp>
#include <sge/model/obj/loader.hpp>
#include <sge/model/obj/vb_converter/convert.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/scoped_vertex_declaration.hpp>
#include <sge/renderer/vertex_declaration.hpp>
#include <sge/renderer/cg/loaded_program.hpp>
#include <sge/renderer/cg/loaded_texture.hpp>
#include <sge/renderer/cg/loaded_texture_scoped_ptr.hpp>
#include <sge/renderer/cg/scoped_program.hpp>
#include <sge/renderer/context/object.hpp>
#include <sge/renderer/state/bool.hpp>
#include <sge/renderer/state/depth_func.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/renderer/texture/create_planar_from_path.hpp>
#include <sge/renderer/texture/planar_shared_ptr.hpp>
#include <sge/renderer/texture/scoped.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/optional.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/scoped_ptr.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assert/error_message.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/container/ptr/insert_unique_ptr_map.hpp>
#include <fcppt/filesystem/extension_without_dot.hpp>
#include <fcppt/filesystem/stem.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/filesystem/operations.hpp>
#include <fcppt/config/external_end.hpp>


flake::volume::model::manager::manager(
	sge::renderer::device &_renderer,
	sge::cg::context::object &_cg_context,
	flake::shader::vertex_profile const &_cg_vertex_profile,
	flake::shader::pixel_profile const &_cg_pixel_profile,
	sge::image2d::system &_image_system,
	sge::camera::base &_camera,
	model::sun_direction const &_sun_direction)
:
	renderer_(
		_renderer),
	camera_(
		_camera),
	vertex_declaration_(
		_renderer.create_vertex_declaration(
			sge::renderer::vf::dynamic::make_format<vf::format>())),
	vertex_program_(
		sge::cg::program::from_file_parameters(
			_cg_context,
			sge::cg::program::source_type::text,
			_cg_vertex_profile.get(),
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/model.cg")),
			sge::cg::program::main_function(
				"vertex_main"),
			_renderer.cg_compile_options(
				_cg_context,
				_cg_vertex_profile.get()))),
	pixel_program_(
		sge::cg::program::from_file_parameters(
			_cg_context,
			sge::cg::program::source_type::text,
			_cg_pixel_profile.get(),
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/model.cg")),
			sge::cg::program::main_function(
				"pixel_main"),
			_renderer.cg_compile_options(
				_cg_context,
				_cg_pixel_profile.get()))),
	loaded_vertex_program_(
		renderer_.load_cg_program(
			vertex_program_)),
	loaded_pixel_program_(
		renderer_.load_cg_program(
			pixel_program_)),
	mvp_parameter_(
		vertex_program_.parameter(
			"mvp")),
	world_parameter_(
		vertex_program_.parameter(
			"world")),
	sun_direction_parameter_(
		pixel_program_.parameter(
			"sun_direction")),
	identifier_to_vertex_buffer_(),
	identifier_to_texture_(),
	model_loader_(
		sge::model::obj::create()),
	children_()
{
	sge::cg::parameter::vector::set(
		sun_direction_parameter_.object(),
		_sun_direction.get());

	for(
		boost::filesystem::directory_iterator current_file(
			flake::media_path_from_string(
				FCPPT_TEXT("models")));
		current_file != boost::filesystem::directory_iterator();
		++current_file)
	{
		FCPPT_ASSERT_ERROR_MESSAGE(
			boost::filesystem::is_regular_file(
				*current_file),
			FCPPT_TEXT("The model directory is supposed to contain only models"));

		FCPPT_ASSERT_ERROR_MESSAGE(
			fcppt::filesystem::extension_without_dot(
				*current_file) == FCPPT_TEXT("obj"),
			FCPPT_TEXT("A model has non-.obj extension."));

		fcppt::string const stem =
			fcppt::filesystem::stem(
				*current_file);

		fcppt::container::ptr::insert_unique_ptr_map(
			identifier_to_vertex_buffer_,
			model::identifier(
				stem),
			sge::model::obj::vb_converter::convert
			<
				vf::format_part,
				vf::from_obj_map
			>(
				renderer_,
				*vertex_declaration_,
				sge::renderer::resource_flags::none,
				*(model_loader_->load(
					*current_file))));

		boost::filesystem::path const texture_file(
			flake::media_path_from_string(
				FCPPT_TEXT("textures/models/")+
				stem+
				FCPPT_TEXT(".png")));

		FCPPT_ASSERT_ERROR_MESSAGE(
			boost::filesystem::is_regular_file(
				texture_file),
			FCPPT_TEXT("The texture corresponding to a model was not found."));

		fcppt::container::ptr::insert_unique_ptr_map(
			identifier_to_texture_,
			model::identifier(
				stem),
			sge::renderer::texture::create_planar_from_path(
				texture_file,
				renderer_,
				_image_system,
				sge::renderer::texture::mipmap::off(),
				sge::renderer::resource_flags::none));
	}
}

void
flake::volume::model::manager::render(
	sge::renderer::context::object &_context)
{
	sge::renderer::state::scoped scoped_state(
		_context,
		sge::renderer::state::list
			(sge::renderer::state::depth_func::less));

	sge::renderer::scoped_vertex_declaration scoped_vertex_declaration(
		_context,
		*vertex_declaration_);

	sge::renderer::cg::scoped_program
		scoped_vertex_program(
			_context,
			*loaded_vertex_program_),
		scoped_pixel_program(
			_context,
			*loaded_pixel_program_);

	model::identifier previous_identifier(
		fcppt::string(
			FCPPT_TEXT("")));

	fcppt::scoped_ptr<sge::renderer::scoped_vertex_buffer> scoped_vb;
	sge::renderer::cg::loaded_texture_scoped_ptr loaded_texture;
	fcppt::optional<sge::renderer::vertex_buffer &> current_vertex_buffer;
	fcppt::scoped_ptr<sge::renderer::cg::scoped_texture> scoped_texture;

	for(
		child_sequence::iterator child =
			children_.begin();
		child != children_.end();
		++child)
	{
		if(child->identifier() != previous_identifier)
		{
			vertex_buffer_map::iterator vertex_buffer =
				identifier_to_vertex_buffer_.find(
					child->identifier());

			current_vertex_buffer =
				fcppt::optional<sge::renderer::vertex_buffer &>(
					*(vertex_buffer->second));

			texture_map::iterator texture =
				identifier_to_texture_.find(
					child->identifier());

			FCPPT_ASSERT_ERROR_MESSAGE(
				vertex_buffer != identifier_to_vertex_buffer_.end() &&
				texture != identifier_to_texture_.end(),
				FCPPT_TEXT("Internal texture/vertex buffer identifier inconsistency"));

			scoped_vb.take(
				fcppt::make_unique_ptr<sge::renderer::scoped_vertex_buffer>(
					fcppt::ref(
						_context),
					fcppt::ref(
						*(vertex_buffer->second))));

			loaded_texture.take(
				renderer_.load_cg_texture(
					pixel_program_.parameter(
						"primary_texture").object(),
					*(texture->second)));

			scoped_texture.take(
				fcppt::make_unique_ptr<sge::renderer::cg::scoped_texture>(
					fcppt::ref(
						_context),
					fcppt::cref(
						*loaded_texture)));
		}

		sge::camera::coordinate_system::object const moved_coordinate_system(
			camera_.coordinate_system().right(),
			camera_.coordinate_system().up(),
			camera_.coordinate_system().forward(),
			sge::camera::coordinate_system::position(
				camera_.coordinate_system().position().get() +
				child->position().get()));

		sge::cg::parameter::matrix::set(
			mvp_parameter_.object(),
			sge::camera::matrix_conversion::world_projection(
				moved_coordinate_system,
				camera_.projection_matrix()));

		sge::cg::parameter::matrix::set(
			world_parameter_.object(),
			sge::camera::matrix_conversion::world(
				moved_coordinate_system));

		FCPPT_ASSERT_ERROR_MESSAGE(
			current_vertex_buffer,
			FCPPT_TEXT("We have no vertex buffer to render to!"));

		_context.render_nonindexed(
			sge::renderer::first_vertex(
				0u),
			sge::renderer::vertex_count(
				current_vertex_buffer->size()),
			sge::renderer::primitive_type::triangle_list);
	}
}

flake::volume::model::manager::~manager()
{
}

void
flake::volume::model::manager::add_child(
	model::object &_model)
{
	children_.push_back(
		_model);
}
