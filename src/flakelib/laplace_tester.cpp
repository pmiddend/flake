#include <flakelib/laplace_tester.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/planar_object_size.hpp>
#include <flakelib/cl/apply_kernel_to_planar_image.hpp>
#include <flakelib/laplace_solver/base.hpp>
#include <flakelib/visualization/monitor/texture.hpp>
#include <flakelib/cl/planar_image_view_to_cl_image.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/font/system.hpp>
#include <sge/image/colors.hpp>
#include <sge/image2d/multi_loader.hpp>
#include <sge/image2d/file.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/create_image_format.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/container/ptr/push_back_unique_ptr.hpp>


flakelib::laplace_tester::laplace_tester(
	laplace_solver::base &_solver,
	flakelib::planar_cache &_planar_cache,
	sge::renderer::device &_renderer,
	sge::viewport::manager &_viewport_manager,
	sge::opencl::command_queue::object &_command_queue,
	sge::font::system &_font_system,
	sge::image2d::multi_loader &_image_loader)
:
	solver_(
		_solver),
	renderer_(
		_renderer),
	command_queue_(
		_command_queue),
	planar_cache_(
		_planar_cache),
	boundary_(
		cl::planar_image_view_to_cl_image(
			_image_loader.load(
				flakelib::media_path_from_string(
					FCPPT_TEXT("images/boundary_256_black.png")))->view(),
			command_queue_)),
	monitor_parent_(
		_viewport_manager,
		_renderer,
		command_queue_,
		_font_system.create_font(
			flakelib::media_path_from_string(
				FCPPT_TEXT("fonts/main.ttf")),
			static_cast<sge::font::size_type>(
				15)),
		visualization::monitor::font_color(
			sge::image::colors::black())),
	master_widget_(
		rucksack::padding(
			3),
		rucksack::aspect(
			1,
			1)),
	initial_guess_image_(
		planar_cache_,
		boundary_->size()[0]),
	rhs_(
		planar_cache_,
		boundary_->size()[0]),
	destination_(
		planar_cache_,
		boundary_->size()[0]),
	utility_program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/utility.cl"))),
		sge::opencl::program::build_parameters()),
	initial_guess_kernel_(
		utility_program_,
		sge::opencl::kernel::name("generate_guess")),
	null_image_kernel_(
		utility_program_,
		sge::opencl::kernel::name("null_image")),
	additional_data_()
{
	monitor_parent_.viewport_widget().child(
		master_widget_);

	// Generate the initial guess
	null_image_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		rhs_.value());

	flakelib::cl::apply_kernel_to_planar_image(
		null_image_kernel_,
		command_queue_,
		rhs_.value());
}

void
flakelib::laplace_tester::render()
{
	monitor_parent_.update();
	monitor_parent_.render();
}

void
flakelib::laplace_tester::update()
{
	// Generate the initial guess
	initial_guess_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		initial_guess_image_.value());

	flakelib::cl::apply_kernel_to_planar_image(
		initial_guess_kernel_,
		command_queue_,
		initial_guess_image_.value());

	// Solve the homogenous problem
	solver_.solve(
		laplace_solver::rhs(
			rhs_.value()),
		laplace_solver::destination(
			destination_.value()),
		laplace_solver::initial_guess(
			initial_guess_image_.value()),
		laplace_solver::boundary(
			*boundary_));

	additional_data_.clear();

	fcppt::container::ptr::push_back_unique_ptr(
		additional_data_,
		fcppt::make_unique_ptr<visualization::monitor::texture>(
			fcppt::ref(
				monitor_parent_),
			visualization::monitor::name(
				FCPPT_TEXT("initial guess")),
			visualization::monitor::grid_dimensions(
				fcppt::math::dim::structure_cast<visualization::monitor::grid_dimensions::value_type>(
					initial_guess_image_.value().size())),
			fcppt::math::dim::structure_cast<visualization::monitor::dim>(
				initial_guess_image_.value().size()),
			visualization::monitor::scaling_factor(
				1.0f)));

	additional_data_.back().from_planar_object(
		flakelib::planar_object(
			&initial_guess_image_.value()));

	master_widget_.push_back_child(
		additional_data_.back().widget());

	for(
		flakelib::additional_planar_data::const_iterator it =
			solver_.additional_planar_data().begin();
		it != solver_.additional_planar_data().end();
		++it)
	{
		sge::opencl::memory_object::dim2 const object_size =
			flakelib::planar_object_size(
				it->second);

		fcppt::container::ptr::push_back_unique_ptr(
			additional_data_,
			fcppt::make_unique_ptr<visualization::monitor::texture>(
				fcppt::ref(
					monitor_parent_),
				visualization::monitor::name(
					it->first),
				visualization::monitor::grid_dimensions(
					fcppt::math::dim::structure_cast<visualization::monitor::grid_dimensions::value_type>(
						object_size)),
				fcppt::math::dim::structure_cast<visualization::monitor::dim>(
					object_size),
				visualization::monitor::scaling_factor(
					1.0f)));

		additional_data_.back().from_planar_object(
			it->second);

		master_widget_.push_back_child(
			additional_data_.back().widget());
	}

	fcppt::container::ptr::push_back_unique_ptr(
		additional_data_,
		fcppt::make_unique_ptr<visualization::monitor::texture>(
			fcppt::ref(
				monitor_parent_),
			visualization::monitor::name(
				FCPPT_TEXT("destination")),
			visualization::monitor::grid_dimensions(
				fcppt::math::dim::structure_cast<visualization::monitor::grid_dimensions::value_type>(
					destination_.value().size())),
			fcppt::math::dim::structure_cast<visualization::monitor::dim>(
				destination_.value().size()),
			visualization::monitor::scaling_factor(
				1.0f)));

	additional_data_.back().from_planar_object(
		flakelib::planar_object(
			&destination_.value()));

	master_widget_.push_back_child(
		additional_data_.back().widget());

}

flakelib::laplace_tester::~laplace_tester()
{
}
