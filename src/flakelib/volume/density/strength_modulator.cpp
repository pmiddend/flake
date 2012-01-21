#include <flakelib/volume/density/advector.hpp>
#include <flakelib/volume/density/strength_modulator.hpp>
#include <fcppt/chrono/high_resolution_clock.hpp>
#include <fcppt/math/interpolation/trigonometric.hpp>
#include <fcppt/random/make_inclusive_range.hpp>


flakelib::volume::density::strength_modulator::strength_modulator(
	density::advector &_advector,
	density::strength_minimum const &_strength_minimum,
	density::strength_maximum const &_strength_maximum,
	density::change_frequency const &_change_frequency)
:
	advector_(
		_advector),
	change_frequency_(
		_change_frequency.get()),
	number_generator_(
		static_cast<fcppt::random::default_generator::result_type>(
			fcppt::chrono::high_resolution_clock::now().time_since_epoch().count())),
	next_control_point_rng_(
		fcppt::random::make_inclusive_range(
			_strength_minimum.get(),
			_strength_maximum.get()),
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
flakelib::volume::density::strength_modulator::update(
	flakelib::duration const &_dt)
{
	current_time_ +=
		_dt;

	if(current_time_.count() >= change_frequency_)
	{
		current_time_ =
			flakelib::duration(
				static_cast<cl_float>(
					0.0f));

		last_control_point_ =
			next_control_point_;

		next_control_point_ = next_control_point_rng_();
	}

	advector_.density_strength(
		fcppt::math::interpolation::trigonometric(
			current_time_.count() / change_frequency_,
			last_control_point_,
			next_control_point_));
}

flakelib::volume::density::strength_modulator::~strength_modulator()
{
}
