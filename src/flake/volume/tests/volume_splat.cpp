#include <flake/catch_statements.hpp>
#include <flake/save_l8_texture_to_file.hpp>
#include <flake/volume/tests/volume_splat.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>
#include <flakelib/buffer_pool/volume_lock_impl.hpp>
#include <flakelib/splatter/box/object.hpp>
#include <flakelib/splatter/pen/object_impl.hpp>
#include <flakelib/splatter/pen/volume.hpp>
#include <flakelib/volume/conversion/planar_size_from_volume_size.hpp>
#include <sge/image/colors.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/opencl/single_device_system/object.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/state/color.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/renderer/texture/capabilities_field.hpp>
#include <sge/renderer/texture/planar.hpp>
#include <sge/renderer/texture/planar_parameters.hpp>
#include <sge/renderer/texture/planar_scoped_ptr.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/container/bitfield/object_impl.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/math/vector/object_impl.hpp>


awl::main::exit_code const
custom_main(
	awl::main::function_context const &);

awl::main::exit_code const
custom_main(
	awl::main::function_context const &_function_context)
try
{
	flake::volume::tests::volume_splat s(
		_function_context);

	return
		s.run();
}
FLAKE_CATCH_STATEMENTS

flake::volume::tests::volume_splat::volume_splat(
	awl::main::function_context const &_function_context)
:
	flake::test_base(
		_function_context,
		sge::window::title(
			FCPPT_TEXT("Volume splat test")),
		sge::systems::cursor_option_field()),
	simulation_size_(
		sge::parse::json::find_and_convert_member<sge::opencl::memory_object::dim3>(
			this->configuration(),
			sge::parse::json::string_to_path(
				FCPPT_TEXT("tests/volume/volume-splat/simulation-size")))),
	splatter_(
		this->program_context()),
	density_buffer_(
		fcppt::make_unique_ptr<flakelib::volume::float_buffer_lock>(
			fcppt::ref(
				this->buffer_pool()),
			simulation_size_)),
	conversion_object_(
		this->program_context()),
	fill_buffer_(
		this->program_context())
{
	fill_buffer_.apply(
		flakelib::buffer::linear_view<cl_float>(
			density_buffer_->value().buffer()),
		static_cast<cl_float>(
			0.0f));

	flakelib::splatter::box::size const splat_size(
		sge::parse::json::find_and_convert_member<sge::opencl::memory_object::dim3>(
			this->configuration(),
			sge::parse::json::string_to_path(
				FCPPT_TEXT("tests/volume/volume-splat/splat-size"))));

	splatter_.splat_volume_float(
		density_buffer_->value(),
		flakelib::splatter::pen::volume(
			flakelib::splatter::box::object(
				flakelib::splatter::box::position(
					sge::opencl::memory_object::vector3(
						simulation_size_.w()/2 - splat_size.get().w()/2,
						simulation_size_.h()/2 - splat_size.get().h()/2,
						simulation_size_.d()/2 - splat_size.get().d()/2)),
				splat_size),
			flakelib::splatter::pen::is_round(
				true),
			flakelib::splatter::pen::is_smooth(
				true),
			flakelib::splatter::pen::draw_mode::add,
			flakelib::splatter::pen::blend_factor(
				1.0f)),
		static_cast<cl_float>(
			1.0f));

	sge::renderer::texture::planar_scoped_ptr temporary_texture(
		this->renderer().create_planar_texture(
			sge::renderer::texture::planar_parameters(
				flakelib::volume::conversion::planar_size_from_volume_size(
					simulation_size_),
				sge::image::color::format::l8,
				sge::renderer::texture::mipmap::off(),
				sge::renderer::resource_flags_field(
					sge::renderer::resource_flags::readable),
				sge::renderer::texture::capabilities_field::null())));

	sge::opencl::memory_object::image::planar temporary_texture_image(
		this->opencl_system().context(),
		sge::opencl::memory_object::flags_field(
			sge::opencl::memory_object::flags::read) | sge::opencl::memory_object::flags::write,
		*temporary_texture);

	conversion_object_.float_view_to_flat_volume_texture(
		density_buffer_->value(),
		temporary_texture_image,
		flakelib::volume::conversion::scaling_factor(
			1.0f),
		flakelib::volume::conversion::constant_addition(
			0.0f));

	std::cout << "Saving to file...\n";
	flake::save_l8_texture_to_file(
		this->image_system(),
		*temporary_texture,
		"/tmp/slices.png");
}

awl::main::exit_code const
flake::volume::tests::volume_splat::run()
{
	return
		flake::test_base::run();
}

flake::volume::tests::volume_splat::~volume_splat()
{
}

void
flake::volume::tests::volume_splat::render()
{
	sge::renderer::state::scoped scoped_state(
		this->renderer(),
		sge::renderer::state::list
			(sge::renderer::state::color::back_buffer_clear_color = sge::image::colors::grey()));

	this->renderer().clear(
		sge::renderer::clear_flags_field(
			sge::renderer::clear_flags::back_buffer));

	test_base::render();
}

void
flake::volume::tests::volume_splat::update()
{
	test_base::update();
}
