#ifndef FLAKELIB_VOLUME_DENSITY_STRENGTH_MODULATOR_HPP_INCLUDED
#define FLAKELIB_VOLUME_DENSITY_STRENGTH_MODULATOR_HPP_INCLUDED

#include <flakelib/duration.hpp>
#include <flakelib/volume/density/advector_fwd.hpp>
#include <flakelib/volume/density/change_frequency.hpp>
#include <flakelib/volume/density/strength_maximum.hpp>
#include <flakelib/volume/density/strength_minimum.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/chrono/duration.hpp>
#include <fcppt/random/default_generator.hpp>
#include <fcppt/random/uniform.hpp>


namespace flakelib
{
namespace volume
{
namespace density
{
class strength_modulator
{
public:
	explicit
	strength_modulator(
		density::advector &,
		density::strength_minimum const &,
		density::strength_maximum const &,
		density::change_frequency const &);

	void
	update(
		flakelib::duration const &);

	~strength_modulator();
private:
	density::advector &advector_;
	cl_float const change_frequency_;
	fcppt::random::default_generator number_generator_;
	fcppt::random::uniform<cl_float,fcppt::random::default_generator &> next_control_point_rng_;
	flakelib::duration current_time_;
	cl_float last_control_point_;
	cl_float next_control_point_;
};
}
}
}

#endif
