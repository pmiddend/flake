#ifndef FLAKE_VOLUME_SNOW_COVER_SCOPED_HPP_INCLUDED
#define FLAKE_VOLUME_SNOW_COVER_SCOPED_HPP_INCLUDED

#include <flake/volume/snow_cover/object_fwd.hpp>
#include <sge/renderer/context/core_fwd.hpp>
#include <sge/renderer/state/core/sampler/scoped.hpp>
#include <sge/shader/scoped_pair.hpp>
#include <fcppt/noncopyable.hpp>


namespace flake
{
namespace volume
{
namespace snow_cover
{
class scoped
{
FCPPT_NONCOPYABLE(
	scoped);
public:
	scoped(
		flake::volume::snow_cover::object &,
		sge::renderer::context::core &);

	~scoped();
private:
	sge::shader::scoped_pair scoped_shader_;
	sge::renderer::state::core::sampler::scoped sampler_state_;
};
}
}
}

#endif

