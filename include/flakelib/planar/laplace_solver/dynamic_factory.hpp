#ifndef FLAKELIB_PLANAR_LAPLACE_SOLVER_DYNAMIC_FACTORY_HPP_INCLUDED
#define FLAKELIB_PLANAR_LAPLACE_SOLVER_DYNAMIC_FACTORY_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/planar/laplace_solver/base_fwd.hpp>
#include <flakelib/utility/object_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/parse/json/object_fwd.hpp>
#include <fcppt/scoped_ptr.hpp>


namespace flakelib
{
namespace planar
{
namespace laplace_solver
{
/**
\brief A factory creating a solver based on a json object

The class gets all the data necessary to create each available solver
(multigrid, jacobi) and decides on the basis of the given json object which
solver to actually instantiate.

With multigrid, it of course needs to generate an "outer" solver and an inner
solver. The inner solver should be specified inside the outer solver in json,
too. So we nest the factories inside each other (optionally).
*/
class dynamic_factory
{
FCPPT_NONCOPYABLE(
	dynamic_factory);
public:
	/**
	\brief Constructs the appropriate solvers
	\param _buffer_pool The buffer pool needed for planar::laplace_solver::jacobi and planar::laplace_solver::multigrid
	\param _command_queue The command queue needed for all the solvers
	\param _config_file The part of the configuration file specifying the "root" solver.
	\param _utility The utility object needed for most of the solvers

	\warning
	The \p _config_file does <em>not</em> designate the whole config file, just the solver section!
	*/
	explicit
	dynamic_factory(
		flakelib::buffer_pool::object &_buffer_pool,
		sge::opencl::command_queue::object &_command_queue,
		sge::parse::json::object const &_config_file,
		flakelib::build_options const &,
		utility::object &_utility);

	/**
	\brief Returns the solver that was created from the json file
	*/
	planar::laplace_solver::base &
	value() const;

	/** \brief noncopyable classes need a destructor */
	~dynamic_factory();
private:
	fcppt::scoped_ptr<planar::laplace_solver::base> solver_;
	fcppt::scoped_ptr<dynamic_factory> inner_factory_;
};
}
}
}

#endif
