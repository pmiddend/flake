#include <flakelib/exception.hpp>
#include <flakelib/planar/laplace_solver/dynamic_factory.hpp>
#include <flakelib/planar/laplace_solver/jacobi.hpp>
#include <flakelib/planar/laplace_solver/multigrid.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/object.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <fcppt/cref.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/string.hpp>
#include <fcppt/text.hpp>


flakelib::planar::laplace_solver::dynamic_factory::dynamic_factory(
	flakelib::buffer_pool::object &_buffer_pool,
	sge::opencl::command_queue::object &_command_queue,
	sge::parse::json::object const &_config_file,
	flakelib::build_options const &_build_options,
	utility::object &_utility)
:
	solver_(),
	inner_factory_()
{
	fcppt::string const type =
		sge::parse::json::find_and_convert_member<fcppt::string>(
			_config_file,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("type")));

	// This is an attribute common to all solvers, so store it here
	planar::laplace_solver::grid_scale::value_type const grid_scale =
		sge::parse::json::find_and_convert_member<planar::laplace_solver::grid_scale::value_type>(
			_config_file,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("grid-scale")));

	if(type == FCPPT_TEXT("multigrid"))
	{
		sge::parse::json::object const &inner_json =
			sge::parse::json::find_and_convert_member<sge::parse::json::object>(
				_config_file,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("inner-solver")));

		inner_factory_.take(
			fcppt::make_unique_ptr<dynamic_factory>(
				fcppt::ref(
					_buffer_pool),
				fcppt::ref(
					_command_queue),
				fcppt::cref(
					inner_json),
				_build_options,
				fcppt::ref(
					_utility)));

		solver_.take(
			fcppt::make_unique_ptr<planar::laplace_solver::multigrid>(
				fcppt::ref(
					_buffer_pool),
				fcppt::ref(
					_utility),
				fcppt::ref(
					_command_queue),
				_build_options,
				fcppt::ref(
					inner_factory_->value()),
				planar::laplace_solver::grid_scale(
					grid_scale),
				planar::laplace_solver::termination_size(
					sge::parse::json::find_and_convert_member<planar::laplace_solver::termination_size::value_type>(
						_config_file,
						sge::parse::json::string_to_path(
							FCPPT_TEXT("termination-size")))),
				planar::laplace_solver::debug_output(
					false)));
	}
	else if (type == FCPPT_TEXT("jacobi"))
	{
		solver_.take(
			fcppt::make_unique_ptr<planar::laplace_solver::jacobi>(
				fcppt::ref(
					_buffer_pool),
				fcppt::ref(
					_command_queue),
				_build_options,
				planar::laplace_solver::grid_scale(
					grid_scale),
				planar::laplace_solver::iterations(
					sge::parse::json::find_and_convert_member<planar::laplace_solver::iterations::value_type>(
						_config_file,
						sge::parse::json::string_to_path(
							FCPPT_TEXT("iterations"))))));
	}
	else
		throw
			flakelib::exception(
				FCPPT_TEXT("Unknown solver type \"")+
				type+
				FCPPT_TEXT("\""));
}

flakelib::planar::laplace_solver::base &
flakelib::planar::laplace_solver::dynamic_factory::value() const
{
	return
		*solver_;
}

flakelib::planar::laplace_solver::dynamic_factory::~dynamic_factory()
{
}
