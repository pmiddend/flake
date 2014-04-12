#include <flakelib/value_modulator/object.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/math/interpolation/trigonometric.hpp>
#include <fcppt/random/generator/seed_from_chrono.hpp>


flakelib::value_modulator::object::object(
	flakelib::value_modulator::value_modify_callback const &_value_modify_callback,
	flakelib::value_modulator::mean const &_mean,
	flakelib::value_modulator::variance const &_variance,
	flakelib::value_modulator::frequency const &_frequency)
:
	value_modify_callback_(
		_value_modify_callback),
	mean_(
		0.0f),
	variance_(
		_variance),
	frequency_(
		_frequency),
	number_generator_(
		fcppt::random::generator::seed_from_chrono<random_generator::seed>()),
	next_control_point_rng_(
		fcppt::make_unique_ptr<variate>(
			number_generator_,
			distribution(
				distribution::param_type::min(
					_mean.get() - _variance.get()),
				distribution::param_type::sup(
					_mean.get() + _variance.get())))),
	current_time_(
		static_cast<cl_float>(
			0.0f)),
	last_control_point_(),
	next_control_point_()
{
	this->mean(
		_mean);

	last_control_point_ =
		(*next_control_point_rng_)();

	next_control_point_ =
		(*next_control_point_rng_)();
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
			(*next_control_point_rng_)();
	}

	value_modify_callback_(
		fcppt::math::interpolation::trigonometric(
			current_time_.count() / frequency_.get(),
			last_control_point_,
			next_control_point_));
}

void
flakelib::value_modulator::object::mean(
	flakelib::value_modulator::mean const &_mean)
{
	mean_ =
		_mean;

	next_control_point_rng_ =
		fcppt::make_unique_ptr<variate>(
			number_generator_,
			distribution(
				distribution::param_type::min(
					_mean.get() - variance_.get()),
				distribution::param_type::sup(
					_mean.get() + variance_.get())));
}

flakelib::value_modulator::mean const
flakelib::value_modulator::object::mean() const
{
	return
		mean_;
}

flakelib::value_modulator::object::~object()
{
}
