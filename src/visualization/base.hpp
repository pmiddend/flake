#ifndef FLAKE_VISUALIZATION_BASE_HPP_INCLUDED
#define FLAKE_VISUALIZATION_BASE_HPP_INCLUDED

#include "../duration.hpp"
#include <fcppt/noncopyable.hpp>
#include <sge/renderer/state/list.hpp>

namespace flake
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
		flake::duration const &) = 0;

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
