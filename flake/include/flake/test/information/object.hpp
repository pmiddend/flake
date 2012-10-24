#ifndef FLAKE_TEST_INFORMATION_OBJECT_HPP_INCLUDED
#define FLAKE_TEST_INFORMATION_OBJECT_HPP_INCLUDED

#include <flake/test/information/identifier.hpp>
#include <flake/test/information/manager_fwd.hpp>
#include <flake/test/information/pull_function.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/string.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/intrusive/list.hpp>
#include <fcppt/config/external_end.hpp>


namespace flake
{
namespace test
{
namespace information
{
FCPPT_PP_PUSH_WARNING
FCPPT_PP_DISABLE_GCC_WARNING(-Weffc++)

class object
:
	public boost::intrusive::list_base_hook
	<
		boost::intrusive::link_mode<boost::intrusive::auto_unlink>
	>
{
FCPPT_NONCOPYABLE(
	object);
public:
	explicit
	object(
		information::manager &,
		information::identifier const &,
		information::pull_function const &);

	information::identifier const &
	identifier() const;

	fcppt::string const
	value() const;

	~object();
private:
	information::identifier identifier_;
	information::pull_function pull_function_;
};

FCPPT_PP_POP_WARNING
}
}
}

#endif

