#include <flake/media_path_from_string.hpp>
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
#include <sge/renderer/vertex_declaration.hpp>
#include <sge/renderer/scoped_vertex_declaration.hpp>
#include <sge/renderer/state/bool.hpp>
#include <sge/renderer/state/depth_func.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/renderer/texture/create_planar_from_path.hpp>
#include <sge/renderer/texture/planar_shared_ptr.hpp>
#include <sge/renderer/texture/scoped.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/shader/activate_bare.hpp>
#include <sge/shader/object_parameters.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/shader/vf_to_string.hpp>
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
	shader_(
		sge::shader::object_parameters(
			renderer_,
			*vertex_declaration_,
			sge::shader::vf_to_string<vf::format>(),
			fcppt::assign::make_container<sge::shader::variable_sequence>
				(sge::shader::variable(
					"mvp",
					sge::shader::variable_type::uniform,
					sge::shader::matrix(
						sge::renderer::matrix4::identity(),
						sge::shader::matrix_flags::projection)))
				(sge::shader::variable(
					"world",
					sge::shader::variable_type::uniform,
					sge::shader::matrix(
						sge::renderer::matrix4::identity(),
						sge::shader::matrix_flags::none)))
				(sge::shader::variable(
					"sun_direction",
					sge::shader::variable_type::uniform,
					_sun_direction.get())),
			fcppt::assign::make_container<sge::shader::sampler_sequence>
				(sge::shader::sampler(
					"primary_texture",
					sge::renderer::texture::planar_shared_ptr())))
			.vertex_shader(
				flake::media_path_from_string(
					FCPPT_TEXT("shaders/model/vertex.glsl")))
			.fragment_shader(
				flake::media_path_from_string(
					FCPPT_TEXT("shaders/model/fragment.glsl")))
			.name(
				FCPPT_TEXT("Model"))),
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
flake::volume::model::manager::render()
{
	sge::renderer::state::scoped scoped_state(
		renderer_,
		sge::renderer::state::list
			(sge::renderer::state::depth_func::less));

	sge::renderer::scoped_vertex_declaration scoped_vertex_declaration(
		renderer_,
		*vertex_declaration_);

	sge::shader::scoped scoped_shader(
		shader_,
		sge::shader::activate_bare());

	model::identifier previous_identifier(
		fcppt::string(
			FCPPT_TEXT("")));

	fcppt::scoped_ptr<sge::renderer::scoped_vertex_buffer> scoped_vb;
	fcppt::scoped_ptr<sge::renderer::texture::scoped> scoped_texture;
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
						renderer_),
					fcppt::ref(
						*(vertex_buffer->second))));

			scoped_texture.take(
				fcppt::make_unique_ptr<sge::renderer::texture::scoped>(
					fcppt::ref(
						renderer_),
					fcppt::ref(
						*(texture->second)),
					sge::renderer::texture::stage(
						0u)));
		}

		sge::camera::coordinate_system::object const moved_coordinate_system(
			camera_.coordinate_system().right(),
			camera_.coordinate_system().up(),
			camera_.coordinate_system().forward(),
			sge::camera::coordinate_system::position(
				camera_.coordinate_system().position().get() +
				child->position().get()));

		shader_.update_uniform(
			"mvp",
			sge::shader::matrix(
				sge::camera::matrix_conversion::world_projection(
					moved_coordinate_system,
					camera_.projection_matrix()),
				sge::shader::matrix_flags::projection));

		shader_.update_uniform(
			"world",
			sge::shader::matrix(
				sge::camera::matrix_conversion::world(
					moved_coordinate_system),
				sge::shader::matrix_flags::none));

		FCPPT_ASSERT_ERROR_MESSAGE(
			current_vertex_buffer,
			FCPPT_TEXT("We have no vertex buffer to render to!"));

		renderer_.render_nonindexed(
			sge::renderer::first_vertex(
				0u),
			sge::renderer::vertex_count(
				current_vertex_buffer->size()),
			sge::renderer::nonindexed_primitive_type::triangle);
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
