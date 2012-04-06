#ifndef FLAKE_VOLUME_TESTS_VOLUME_SPLAT_HPP_INCLUDED
#define FLAKE_VOLUME_TESTS_VOLUME_SPLAT_HPP_INCLUDED

#include <flake/test_base.hpp>
#include <flakelib/splatter/object.hpp>
#include <flakelib/utility/fill_buffer.hpp>
#include <flakelib/volume/unique_float_buffer_lock.hpp>
#include <flakelib/volume/conversion/object.hpp>
#include <sge/opencl/memory_object/dim3.hpp>
#include <awl/main/exit_code.hpp>
#include <awl/main/function_context_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/math/dim/object_impl.hpp>


namespace flake
{
namespace volume
{
namespace tests
{
class volume_splat
:
	public flake::test_base
{
FCPPT_NONCOPYABLE(
	volume_splat);
public:
	explicit
	volume_splat(
		awl::main::function_context const &);

	awl::main::exit_code const
	run();

	~volume_splat();
private:
	sge::opencl::memory_object::dim3 simulation_size_;
	flakelib::splatter::object splatter_;
	flakelib::volume::unique_float_buffer_lock density_buffer_;
	flakelib::volume::conversion::object conversion_object_;
	flakelib::utility::fill_buffer fill_buffer_;

	void
	render();

	void
	update();
};
}
}
}

#endif

