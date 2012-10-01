#ifndef FLAKELIB_VALUE_MODULATOR_OBJECT_HPP_INCLUDED
#define FLAKELIB_VALUE_MODULATOR_OBJECT_HPP_INCLUDED

#include <flakelib/duration.hpp>
#include <flakelib/symbol.hpp>
#include <flakelib/value_modulator/frequency.hpp>
#include <flakelib/value_modulator/maximum.hpp>
#include <flakelib/value_modulator/minimum.hpp>
#include <flakelib/value_modulator/value_modify_callback.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/random/variate.hpp>
#include <fcppt/random/distribution/uniform_real.hpp>
#include <fcppt/random/generator/minstd_rand.hpp>


namespace flakelib
{
namespace value_modulator
{
class object
{
FCPPT_NONCOPYABLE(
	object);
public:
	FLAKELIB_SYMBOL
	object(
		value_modulator::value_modify_callback const &,
		value_modulator::minimum const &,
		value_modulator::maximum const &,
		value_modulator::frequency const &);

	FLAKELIB_SYMBOL
	void
	update(
		flakelib::duration const &);

	FLAKELIB_SYMBOL
	~object();
private:
        typedef
        fcppt::random::generator::minstd_rand
        random_generator;

        typedef
        fcppt::random::distribution::uniform_real<cl_float>
        distribution;

        typedef
        fcppt::random::variate<random_generator,distribution>
        variate;

	value_modulator::value_modify_callback const value_modify_callback_;
        value_modulator::frequency const frequency_;
        random_generator number_generator_;
        variate next_control_point_rng_;
        flakelib::duration current_time_;
        cl_float last_control_point_;
        cl_float next_control_point_;

};
}
}

#endif

