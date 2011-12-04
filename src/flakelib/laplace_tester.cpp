#include <flakelib/laplace_tester.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/planar_object_size.hpp>
#include <flakelib/cl/apply_kernel_to_planar_image.hpp>
#include <flakelib/cl/planar_image_view_to_cl_image.hpp>
#include <flakelib/laplace_solver/base.hpp>
#include <flakelib/monitor/texture.hpp>
#include <flakelib/utility/object.hpp>
#include <sge/font/system.hpp>
#include <sge/image/colors.hpp>
#include <sge/image2d/file.hpp>
#include <sge/image2d/system.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/create_image_format.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/container/ptr/push_back_unique_ptr.hpp>


flakelib::laplace_tester::laplace_tester(
	laplace_solver::base &_solver,
	flakelib::planar_pool::object &_planar_cache,
	utility::object &_utility,
	sge::renderer::device &_renderer,
	sge::viewport::manager &_viewport_manager,
	sge::opencl::command_queue::object &_command_queue,
	sge::font::system &_font_system,
	sge::image2d::system &_image_loader)
:
	solver_(
		_solver),
	renderer_(
		_renderer),
	command_queue_(
		_command_queue),
	planar_cache_(
		_planar_cache),
	utility_(
		_utility),
	boundary_(
		cl::planar_image_view_to_cl_image(
			_image_loader.load(
				flakelib::media_path_from_string(
					FCPPT_TEXT("images/boundary_1024_black.png")))->view(),
			command_queue_)),
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
		planar_cache_,
		boundary_->size()[0]),
	rhs_(
		planar_cache_,
		boundary_->size()[0]),
	destination_(
		planar_cache_,
		boundary_->size()[0]),
	additional_data_()
{
	master_widget_.child(
		enumeration_widget_);

	utility_.null_image(
		rhs_.value());
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
		laplace_solver::rhs(
			rhs_.value()),
		laplace_solver::destination(
			destination_.value()),
		laplace_solver::initial_guess(
			initial_guess_image_.value()),
		laplace_solver::boundary(
			*boundary_));

	utility_.copy_image(
		utility::from(
			destination_.value()),
		utility::to(
			initial_guess_image_.value()),
		utility::multiplier(
			1.0f));

	solver_.solve(
		laplace_solver::rhs(
			rhs_.value()),
		laplace_solver::destination(
			destination_.value()),
		laplace_solver::initial_guess(
			initial_guess_image_.value()),
		laplace_solver::boundary(
			*boundary_));

	utility_.copy_image(
		utility::from(
			destination_.value()),
		utility::to(
			initial_guess_image_.value()),
		utility::multiplier(
			1.0f));

	solver_.solve(
		laplace_solver::rhs(
			rhs_.value()),
		laplace_solver::destination(
			destination_.value()),
		laplace_solver::initial_guess(
			initial_guess_image_.value()),
		laplace_solver::boundary(
			*boundary_));

	utility_.copy_image(
		utility::from(
			destination_.value()),
		utility::to(
			initial_guess_image_.value()),
		utility::multiplier(
			1.0f));

	solver_.solve(
		laplace_solver::rhs(
			rhs_.value()),
		laplace_solver::destination(
			destination_.value()),
		laplace_solver::initial_guess(
			initial_guess_image_.value()),
		laplace_solver::boundary(
			*boundary_));

	utility_.copy_image(
		utility::from(
			destination_.value()),
		utility::to(
			initial_guess_image_.value()),
		utility::multiplier(
			1.0f));

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

	additional_data_.back().from_planar_object(
		flakelib::planar_object(
			&initial_guess_image_.value()));

	enumeration_widget_.push_back_child(
		additional_data_.back().widget());

	for(
		flakelib::additional_planar_data::const_iterator it =
			solver_.additional_planar_data().begin();
		it != solver_.additional_planar_data().end();
		++it)
	{
		sge::opencl::memory_object::dim2 const object_size =
			flakelib::planar_object_size(
				it->value());

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

		additional_data_.back().from_planar_object(
			it->value());

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

	additional_data_.back().from_planar_object(
		flakelib::planar_object(
			&destination_.value()));

	enumeration_widget_.push_back_child(
		additional_data_.back().widget());

}

flakelib::laplace_tester::~laplace_tester()
{
}
