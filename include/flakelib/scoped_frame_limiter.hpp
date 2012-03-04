#ifndef FLAKELIB_SCOPED_FRAME_LIMITER_HPP_INCLUDED
#define FLAKELIB_SCOPED_FRAME_LIMITER_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/chrono/duration_impl.hpp>
#include <fcppt/chrono/high_resolution_clock.hpp>
#include <fcppt/chrono/milliseconds.hpp>
#include <fcppt/chrono/time_point_impl.hpp>

namespace flakelib
{
class scoped_frame_limiter
{
FCPPT_NONCOPYABLE(
	scoped_frame_limiter);
public:
	typedef
	fcppt::chrono::high_resolution_clock
	clock_type;

	typedef
	clock_type::rep
	fps_type;

	FLAKELIB_SYMBOL
	explicit
	scoped_frame_limiter(
		fps_type);

	FLAKELIB_SYMBOL
	~scoped_frame_limiter();
private:
	clock_type::time_point const before_frame_;
	clock_type::duration minimum_frame_length_;
};
}

#endif
