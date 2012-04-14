#include <flakelib/value_modulator/object.hpp>
#include <fcppt/math/interpolation/trigonometric.hpp>
#include <fcppt/random/generator/seed_from_chrono.hpp>

flakelib::value_modulator::object::object(
	value_modulator::value_modify_callback const &_value_modify_callback,
	value_modulator::minimum const &_minimum,
	value_modulator::maximum const &_maximum,
	value_modulator::frequency const &_frequency)
:
	value_modify_callback_(
		_value_modify_callback),
	frequency_(
		_frequency),
        number_generator_(
                fcppt::random::generator::seed_from_chrono<random_generator::seed>()),
        next_control_point_rng_(
                number_generator_,
                distribution(
                        distribution::min(
                                _minimum.get()),
                        distribution::sup(
                                _maximum.get()))),
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
flakelib::value_modulator::object::update(
	flakelib::duration const &_dt)
{
	current_time_ +=
		_dt;

	if(current_time_.count() >= frequency_.get())
	{
		current_time_ =
			flakelib::duration(
				static_cast<cl_float>(
					0.0f));

		last_control_point_ =
			next_control_point_;

		next_control_point_ =
			next_control_point_rng_();
	}

	value_modify_callback_(
		fcppt::math::interpolation::trigonometric(
			current_time_.count() / frequency_.get(),
			last_control_point_,
			next_control_point_));
}

flakelib::value_modulator::object::~object()
{
}
