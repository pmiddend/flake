#if 0
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/pde_tester.hpp>
#include <flakelib/visualization/monitor/parent.hpp>
#include <flakelib/visualization/monitor/texture.hpp>
#include <sge/font/system.hpp>
#include <sge/image/colors.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/create_image_format.hpp>
#include <sge/opencl/memory_object/flags_field.hpp>
#include <sge/opencl/memory_object/image/planar_pitch.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/source_string_sequence.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/renderer/device.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/io/cifstream.hpp>
#include <fcppt/io/stream_to_string.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/config/external_begin.hpp>
#include <algorithm>
#include <iostream>
#include <fcppt/config/external_end.hpp>


flakelib::pde_tester::pde_tester(
	sge::opencl::context::object &_context,
	sge::opencl::command_queue::object &_command_queue,
	sge::renderer::device &_renderer,
	sge::font::system &_font_system,
	sge::parse::json::object const &_config_file,
	sge::opencl::memory_object::dim2 const &_image_size)
:
	command_queue_(
		_command_queue),
	p0_(
		_context,
		sge::opencl::memory_object::flags_field(sge::opencl::memory_object::flags::read) | sge::opencl::memory_object::flags::write,
		sge::opencl::memory_object::create_image_format(
			CL_R,
			CL_FLOAT),
		fcppt::math::dim::structure_cast<sge::opencl::memory_object::dim2>(
			_image_size),
		sge::opencl::memory_object::image::planar_pitch(
			0)),
	p1_(
		_context,
		sge::opencl::memory_object::flags_field(sge::opencl::memory_object::flags::read) | sge::opencl::memory_object::flags::write,
		sge::opencl::memory_object::create_image_format(
			CL_R,
			CL_FLOAT),
		fcppt::math::dim::structure_cast<sge::opencl::memory_object::dim2>(
			_image_size),
		sge::opencl::memory_object::image::planar_pitch(
			0)),
	guess_generation_program_(
		_context,
		fcppt::assign::make_container<sge::opencl::program::source_string_sequence>(
			fcppt::io::stream_to_string(
				*fcppt::make_unique_ptr<fcppt::io::cifstream>(
					flakelib::media_path_from_string(
						FCPPT_TEXT("kernels/pde_guess.cl"))))),
		sge::opencl::program::build_parameters()),
	generate_guess_kernel_(
		guess_generation_program_,
		sge::opencl::kernel::name(
			"generate_guess")),
	multigrid_program_(
		_context,
		fcppt::assign::make_container<sge::opencl::program::source_string_sequence>(
			fcppt::io::stream_to_string(
				*fcppt::make_unique_ptr<fcppt::io::cifstream>(
					flakelib::media_path_from_string(
						FCPPT_TEXT("kernels/multigrid.cl"))))),
		sge::opencl::program::build_parameters()),
	jacobi_kernel_(
		jacobi_program_,
		sge::opencl::kernel::name(
			"jacobi")),
	monitor_parent_(
		fcppt::make_unique_ptr<visualization::monitor::parent>(
			fcppt::ref(
				_renderer),
			fcppt::ref(
				_context),
			fcppt::ref(
				_command_queue),
			_font_system.create_font(
				flakelib::media_path_from_string(
					FCPPT_TEXT("fonts/main.ttf")),
				sge::parse::json::find_and_convert_member<sge::font::size_type>(
					_config_file,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("visualization/font-size")))),
			visualization::monitor::border_size(
				sge::parse::json::find_and_convert_member<visualization::monitor::border_size::value_type>(
					_config_file,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("visualization/border-size")))),
			visualization::monitor::font_color(
				sge::image::colors::black()),
		visualization::monitor::name(
				FCPPT_TEXT("solution guess")))),
	master_monitor_(
		fcppt::make_unique_ptr<visualization::monitor::texture>(
			fcppt::ref(
				*monitor_parent_),
			visualization::monitor::name(
				FCPPT_TEXT("solution guess")),
			visualization::monitor::grid_dimensions(
				fcppt::math::dim::structure_cast<visualization::monitor::grid_dimensions::value_type>(
					_image_size)),
			visualization::monitor::rect(
				visualization::monitor::rect::vector::null(),
				static_cast<visualization::monitor::rect::value_type>(8) *
				fcppt::math::dim::structure_cast<visualization::monitor::rect::dim>(
					_image_size)),
			visualization::monitor::scaling_factor(
				1.0f)))
{
	generate_guess_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		p0_);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		generate_guess_kernel_,
		fcppt::assign::make_array<std::size_t>
			(v1_.size()[0])
			(v1_.size()[1]).container(),
		fcppt::assign::make_array<std::size_t>
			(1)
			(1).container());

	monitor_parent_->update();
	this->update();
}

void
flakelib::pde_tester::render()
{
	monitor_parent_->render();
}

void
flakelib::pde_tester::update()
{
	cl_float const grid_scale = 1.0f;

	sge::opencl::memory_object::image::planar &
		null_image =
			unpool_image(
				64)
		destination =
			unpool_image(
				64);

	this->null_image();

	this->lock(
		destination);

	this->multigrid(
		);

	p1_->from_planar_object(
		flakelib::planar_object(
			current_dest));
}

flakelib::pde_tester::~pde_tester()
{
}

void
flakelib::pde_tester::multigrid(
	sge::opencl::memory_object::image::planar &rhs,
	sge::opencl::memory_object::image::planar &destination)
{
	FCPPT_ASSERT_PRE(
		destination.size()[0] == destination.size()[1]);

	FCPPT_ASSERT_PRE(
		is_power_of_two(destination.size()[0]));

	std::size_t const size =
		static_cast<std::size_t>(
			destination.size()[0]);

	sge::opencl::memory_object::image::planar &
		p0 =
			unpool_image(
				size),
		p1 =
			unpool_image(
				size);

	this->null_image(
		p0);

	this->jacobi(
		p0,
		p1,
		rhs);

	if(size == 8u)
	{
		this->jacobi(
			destination,
			p0,
			rhs);
		return;
	}

	this->laplacian_residual_rhs_from_to(
		rhs,
		p1,
		p0);

	sge::opencl::memory_object::image::planar &
		rd =
			get_image(
				size/2u),
		coarse_rhs =
			get_image(
				size/2u);

	this->downsample_from_to(
		p0,
		rd);

	this->multigrid(
		rd,
		coarse_rhs);

	this->upsample_from_to(
		coarse_rhs,
		p0);

	sge::opencl::memory_object::image::planar &
		h =
			get_image(
				size);

	this->add_to(
		p0,
		p1,
		h);

	this->jacobi(
		h,
		destination);
}
#endif
