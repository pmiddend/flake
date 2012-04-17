#ifndef FLAKE_TEST_INFORMATION_STRING_CONVERSION_ADAPTER_HPP_INCLUDED
#define FLAKE_TEST_INFORMATION_STRING_CONVERSION_ADAPTER_HPP_INCLUDED

#include <flake/test/information/pull_function.hpp>
#include <fcppt/insert_to_fcppt_string.hpp>
#include <fcppt/tr1/functional.hpp>

namespace flake
{
namespace test
{
namespace information
{
template<typename T,typename Functor>
class string_conversion_adapter_impl
{
public:
	explicit
	string_conversion_adapter_impl(
		Functor const &_impl)
	:
		impl_(
			_impl)
	{
	}

	fcppt::string const
	operator()() const
	{
		return
			fcppt::insert_to_fcppt_string(
				impl_());
	}
private:
	Functor impl_;
};

template<typename T,typename Functor>
flake::test::information::string_conversion_adapter_impl<T,Functor> const
string_conversion_adapter(
	Functor const &f)
{
	return
		flake::test::information::string_conversion_adapter_impl<T,Functor>(
			f);
}
/*
template
<
	typename T,
	typename Functor
>
flake::test::information::pull_function const
string_conversion_adapter(
	Functor const &_input)
{
	return
		flake::test::information::pull_function(
			std::tr1::bind(
				static_cast<fcppt::string const (*)(T const &,std::locale const &)>(
					&fcppt::insert_to_fcppt_string),
				_input,
				std::locale()));
}
*/
}
}
}

#endif

