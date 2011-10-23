#ifndef FLAKELIB_VISUALIZATION_BASE_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_BASE_HPP_INCLUDED

#include <flakelib/duration.hpp>
#include <sge/renderer/state/list.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace visualization
{
class base
{
FCPPT_NONCOPYABLE(
	base);
public:
	virtual void
	update(
		flakelib::duration const &) = 0;

	virtual void
	render() = 0;

	virtual sge::renderer::state::list const
	render_states() const = 0;

	virtual ~base() = 0;
protected:
	explicit
	base();
};
}
}

#endif
