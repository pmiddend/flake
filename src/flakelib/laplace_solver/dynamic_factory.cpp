#include <flakelib/exception.hpp>
#include <flakelib/laplace_solver/dynamic_factory.hpp>
#include <flakelib/laplace_solver/jacobi.hpp>
#include <flakelib/laplace_solver/multigrid.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/object.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <fcppt/cref.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/string.hpp>
#include <fcppt/text.hpp>


flakelib::laplace_solver::dynamic_factory::dynamic_factory(
	flakelib::planar_pool::object &_planar_pool,
	sge::opencl::command_queue::object &_command_queue,
	sge::parse::json::object const &_config_file,
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
	laplace_solver::grid_scale::value_type const grid_scale =
		sge::parse::json::find_and_convert_member<laplace_solver::grid_scale::value_type>(
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
					_planar_pool),
				fcppt::ref(
					_command_queue),
				fcppt::cref(
					inner_json),
				fcppt::ref(
					_utility)));

		solver_.take(
			fcppt::make_unique_ptr<laplace_solver::multigrid>(
				fcppt::ref(
					_planar_pool),
				fcppt::ref(
					_utility),
				fcppt::ref(
					_command_queue),
				fcppt::ref(
					inner_factory_->value()),
				laplace_solver::grid_scale(
					grid_scale),
				laplace_solver::termination_size(
					sge::parse::json::find_and_convert_member<laplace_solver::termination_size::value_type>(
						_config_file,
						sge::parse::json::string_to_path(
							FCPPT_TEXT("termination-size")))),
				laplace_solver::debug_output(
					false)));
	}
	else if (type == FCPPT_TEXT("jacobi"))
	{
		solver_.take(
			fcppt::make_unique_ptr<laplace_solver::jacobi>(
				fcppt::ref(
					_planar_pool),
				fcppt::ref(
					_command_queue),
				laplace_solver::grid_scale(
					grid_scale),
				laplace_solver::iterations(
					sge::parse::json::find_and_convert_member<laplace_solver::iterations::value_type>(
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

flakelib::laplace_solver::base &
flakelib::laplace_solver::dynamic_factory::value() const
{
	return
		*solver_;
}

flakelib::laplace_solver::dynamic_factory::~dynamic_factory()
{
}
