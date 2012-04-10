#ifndef FLAKE_VOLUME_DENSITY_VISUALIZATION_SGEVOL_RAYCASTER_HPP_INCLUDED
#define FLAKE_VOLUME_DENSITY_VISUALIZATION_SGEVOL_RAYCASTER_HPP_INCLUDED

#include <fcppt/noncopyable.hpp>

namespace flake
{
namespace volume
{
namespace density_visualization
{
class sgevol_raycaster
{
FCPPT_NONCOPYABLE(
	sgevol_raycaster);
public:
	explicit
	sgevol_raycaster();

	void
	render();

	~sgevol_raycaster();
private:
	
};
}
}
}

#endif

