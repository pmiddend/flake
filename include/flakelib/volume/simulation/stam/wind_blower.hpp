#ifndef FLAKELIB_VOLUME_SIMULATION_STAM_WIND_BLOWER_HPP_INCLUDED
#define FLAKELIB_VOLUME_SIMULATION_STAM_WIND_BLOWER_HPP_INCLUDED

#include <flakelib/duration.hpp>
#include <flakelib/volume/simulation/stam/wind_change_frequency.hpp>
#include <flakelib/volume/simulation/stam/object_fwd.hpp>
#include <flakelib/volume/simulation/stam/wind_speed_maximum.hpp>
#include <flakelib/volume/simulation/stam/wind_speed_minimum.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/chrono/duration.hpp>
#include <fcppt/random/default_generator.hpp>
#include <fcppt/random/uniform.hpp>


namespace flakelib
{
namespace volume
{
namespace simulation
{
namespace stam
{
class wind_blower
{
public:
	explicit
	wind_blower(
		stam::object &,
		stam::wind_speed_minimum const &,
		stam::wind_speed_maximum const &,
		stam::wind_change_frequency const &);

	void
	update(
		flakelib::duration const &);

	~wind_blower();
private:
	stam::object &stam_;
	cl_float const wind_change_frequency_;
	fcppt::random::default_generator number_generator_;
	fcppt::random::uniform<cl_float,fcppt::random::default_generator &> next_control_point_rng_;
	flakelib::duration current_time_;
	cl_float last_control_point_;
	cl_float next_control_point_;
};
}
}
}
}

#endif
