#include <flakelib/laplace_tester.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/linear_view.hpp>
#include <flakelib/cl/apply_kernel_to_planar_image.hpp>
#include <flakelib/cl/planar_image_view_to_float_buffer.hpp>
#include <flakelib/laplace_solver/planar/base.hpp>
#include <flakelib/monitor/texture.hpp>
#include <flakelib/utility/object.hpp>
#include <sge/font/system.hpp>
#include <sge/image/colors.hpp>
#include <sge/image2d/file.hpp>
#include <sge/image2d/system.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/image2d/view/size.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/container/ptr/push_back_unique_ptr.hpp>
#include <fcppt/math/dim/structure_cast.hpp>


flakelib::laplace_tester::laplace_tester(
	laplace_solver::planar::base &_solver,
	flakelib::buffer_pool::object &_buffer_cache,
	utility::object &_utility,
	sge::renderer::device &_renderer,
	sge::viewport::manager &_viewport_manager,
	sge::opencl::command_queue::object &_command_queue,
	sge::font::system &_font_system,
	sge::image2d::system &_image_loader,
	flakelib::build_options const &_build_options)
:
	solver_(
		_solver),
	command_queue_(
		_command_queue),
	utility_(
		_utility),
	boundary_image_file_(
		_image_loader.load(
			flakelib::media_path_from_string(
				FCPPT_TEXT("images/boundary_jagged_256.png")))),
	boundary_(
		_buffer_cache,
		fcppt::math::dim::structure_cast<sge::opencl::memory_object::dim2>(
			sge::image2d::view::size(
				boundary_image_file_->view()))),
	monitor_parent_(
		_renderer,
		command_queue_,
		_font_system.create_font(
			flakelib::media_path_from_string(
				FCPPT_TEXT("fonts/main.ttf")),
			static_cast<sge::font::size_type>(
				15)),
		monitor::font_color(
			sge::image::colors::black())),
	planar_converter_(
		command_queue_,
		_build_options),
	master_widget_(
		_viewport_manager,
		_renderer),
	enumeration_widget_(
		rucksack::padding(
			3),
		rucksack::aspect(
			1,
			1)),
	initial_guess_image_(
		_buffer_cache,
		boundary_.value().size()),
	rhs_(
		_buffer_cache,
		boundary_.value().size()),
	destination_(
		_buffer_cache,
		boundary_.value().size()),
	additional_data_()
{
	master_widget_.child(
		enumeration_widget_);

	utility_.null_buffer(
		buffer::linear_view<cl_float>(
			rhs_.value().buffer()));

	cl::planar_image_view_to_float_buffer(
		command_queue_,
		boundary_image_file_->view(),
		boundary_.value());
}

void
flakelib::laplace_tester::render(
	monitor::optional_projection const &_projection)
{
	monitor_parent_.update();
	monitor_parent_.render(
		_projection);
}

void
flakelib::laplace_tester::update()
{
	utility_.generate_oscillation(
		initial_guess_image_.value());

	// Solve the homogenous problem
	solver_.solve(
		laplace_solver::planar::rhs(
			rhs_.value()),
		laplace_solver::planar::destination(
			destination_.value()),
		laplace_solver::planar::initial_guess(
			initial_guess_image_.value()),
		laplace_solver::planar::boundary(
			boundary_.value()));

	additional_data_.clear();

	fcppt::container::ptr::push_back_unique_ptr(
		additional_data_,
		fcppt::make_unique_ptr<monitor::texture>(
			fcppt::ref(
				monitor_parent_),
			monitor::name(
				FCPPT_TEXT("initial guess")),
			monitor::grid_dimensions(
				fcppt::math::dim::structure_cast<monitor::grid_dimensions::value_type>(
					initial_guess_image_.value().size())),
			fcppt::math::dim::structure_cast<monitor::dim>(
				initial_guess_image_.value().size()),
			monitor::scaling_factor(
				1.0f)));

	planar_converter_.scalar_to_texture(
		initial_guess_image_.value(),
		additional_data_.back().cl_texture(),
		monitor::scaling_factor(
			1.0f));

	enumeration_widget_.push_back_child(
		additional_data_.back().widget());

	for(
		flakelib::additional_planar_data::const_iterator it =
			solver_.additional_planar_data().begin();
		it != solver_.additional_planar_data().end();
		++it)
	{
		sge::opencl::memory_object::dim2 const object_size =
			it->value()->size();

		fcppt::container::ptr::push_back_unique_ptr(
			additional_data_,
			fcppt::make_unique_ptr<monitor::texture>(
				fcppt::ref(
					monitor_parent_),
				monitor::name(
					it->key()),
				monitor::grid_dimensions(
					fcppt::math::dim::structure_cast<monitor::grid_dimensions::value_type>(
						object_size)),
				fcppt::math::dim::structure_cast<monitor::dim>(
					object_size),
				monitor::scaling_factor(
					1.0f)));

		planar_converter_.scalar_to_texture(
			*(it->value()),
			additional_data_.back().cl_texture(),
			monitor::scaling_factor(
				1.0f));

		enumeration_widget_.push_back_child(
			additional_data_.back().widget());
	}

	fcppt::container::ptr::push_back_unique_ptr(
		additional_data_,
		fcppt::make_unique_ptr<monitor::texture>(
			fcppt::ref(
				monitor_parent_),
			monitor::name(
				FCPPT_TEXT("destination")),
			monitor::grid_dimensions(
				fcppt::math::dim::structure_cast<monitor::grid_dimensions::value_type>(
					destination_.value().size())),
			fcppt::math::dim::structure_cast<monitor::dim>(
				destination_.value().size()),
			monitor::scaling_factor(
				1.0f)));

	planar_converter_.scalar_to_texture(
		destination_.value(),
		additional_data_.back().cl_texture(),
		monitor::scaling_factor(
			1.0f));

	enumeration_widget_.push_back_child(
		additional_data_.back().widget());

}

flakelib::laplace_tester::~laplace_tester()
{
}
