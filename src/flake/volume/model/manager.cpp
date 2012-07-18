#include <flake/media_path_from_string.hpp>
#include <flake/shader/scoped_pair.hpp>
#include <flake/volume/model/manager.hpp>
#include <flake/volume/model/vf/format.hpp>
#include <flake/volume/model/vf/from_obj_map.hpp>
#include <sge/camera/base.hpp>
#include <sge/camera/coordinate_system/object.hpp>
#include <sge/camera/matrix_conversion/world.hpp>
#include <sge/camera/matrix_conversion/world_projection.hpp>
#include <sge/model/obj/create.hpp>
#include <sge/model/obj/loader.hpp>
#include <sge/model/obj/vb_converter/convert.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/scoped_vertex_declaration.hpp>
#include <sge/renderer/vertex_declaration.hpp>
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
#include <fcppt/cref.hpp>
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
	flake::shader::context &_shader_context,
	sge::image2d::system &_image_system,
	sge::camera::base &_camera,
	model::sun_direction const &_sun_direction,
	flake::volume::model::fog_color const &_fog_color,
	flake::volume::model::fog_density const &_fog_density)
:
	renderer_(
		_renderer),
	camera_(
		_camera),
	vertex_declaration_(
		_renderer.create_vertex_declaration(
			sge::renderer::vf::dynamic::make_format<vf::format>())),
	shader_(
		_shader_context,
		*vertex_declaration_,
		flake::shader::vertex_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/model.cg"))),
		flake::shader::pixel_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/model.cg")))),
	mvp_parameter_(
		shader_.vertex_program(),
		flake::shader::parameter::name(
			sge::cg::string(
				"mvp")),
		flake::shader::parameter::is_projection_matrix(
			true),
		sge::renderer::matrix4()),
	world_parameter_(
		shader_.vertex_program(),
		flake::shader::parameter::name(
			sge::cg::string(
				"world")),
		flake::shader::parameter::is_projection_matrix(
			false),
		sge::renderer::matrix4()),
	sun_direction_parameter_(
		shader_.pixel_program(),
		flake::shader::parameter::name(
			sge::cg::string(
				"sun_direction")),
		_sun_direction.get()),
	fog_color_parameter_(
		shader_.pixel_program(),
		flake::shader::parameter::name(
			sge::cg::string(
				"fog_color")),
		_fog_color.get()),
	fog_density_parameter_(
		shader_.vertex_program(),
		flake::shader::parameter::name(
			sge::cg::string(
				"fog_density")),
		_fog_density.get()),
	loaded_texture_(
		shader_,
		renderer_,
		shader_.pixel_program(),
		flake::shader::parameter::name(
			sge::cg::string(
				"primary_texture")),
		flake::shader::parameter::planar_texture::optional_value()),
	identifier_to_vertex_buffer_(),
	identifier_to_texture_(),
	model_loader_(
		sge::model::obj::create()),
	children_()
{
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

	flake::shader::scoped_pair scoped_shader(
		_context,
		shader_);

	model::identifier previous_identifier(
		fcppt::string(
			FCPPT_TEXT("")));

	fcppt::scoped_ptr<sge::renderer::scoped_vertex_buffer> scoped_vb;
	fcppt::optional<sge::renderer::vertex_buffer &> current_vertex_buffer;

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

			loaded_texture_.set(
				*(texture->second));
		}

		sge::camera::coordinate_system::object const moved_coordinate_system(
			camera_.coordinate_system().right(),
			camera_.coordinate_system().up(),
			camera_.coordinate_system().forward(),
			sge::camera::coordinate_system::position(
				camera_.coordinate_system().position().get() +
				child->position().get()));

		mvp_parameter_.set(
			sge::camera::matrix_conversion::world_projection(
				moved_coordinate_system,
				camera_.projection_matrix()));

		world_parameter_.set(
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
