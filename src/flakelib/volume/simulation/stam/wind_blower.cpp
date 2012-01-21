#include <flakelib/volume/simulation/stam/wind_blower.hpp>
#include <flakelib/volume/simulation/stam/object.hpp>
#include <fcppt/chrono/high_resolution_clock.hpp>
#include <fcppt/math/interpolation/trigonometric.hpp>
#include <fcppt/random/make_inclusive_range.hpp>


flakelib::volume::simulation::stam::wind_blower::wind_blower(
	stam::object &_stam,
	stam::wind_speed_minimum const &_wind_speed_minimum,
	stam::wind_speed_maximum const &_wind_speed_maximum,
	stam::wind_change_frequency const &_wind_change_frequency)
:
	stam_(
		_stam),
	wind_change_frequency_(
		_wind_change_frequency.get()),
	number_generator_(
		static_cast<fcppt::random::default_generator::result_type>(
			fcppt::chrono::high_resolution_clock::now().time_since_epoch().count())),
	next_control_point_rng_(
		fcppt::random::make_inclusive_range(
			_wind_speed_minimum.get(),
			_wind_speed_maximum.get()),
		number_generator_),
	current_time_(
		static_cast<cl_float>(
			0.0f)),
	last_control_point_(
		next_control_point_rng_()),
	next_control_point_(
		next_control_point_rng_())
{

}

void
flakelib::volume::simulation::stam::wind_blower::update(
	flakelib::duration const &_dt)
{
	current_time_ +=
		_dt;

	if(current_time_.count() >= wind_change_frequency_)
	{
		current_time_ =
			flakelib::duration(
				static_cast<cl_float>(
					0.0f));

		last_control_point_ =
			next_control_point_;

		next_control_point_ = next_control_point_rng_();
	}

	stam_.external_force_magnitude(
		fcppt::math::interpolation::trigonometric(
			current_time_.count() / wind_change_frequency_,
			last_control_point_,
			next_control_point_));
}

flakelib::volume::simulation::stam::wind_blower::~wind_blower()
{
}
