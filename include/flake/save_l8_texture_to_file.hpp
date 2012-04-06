#ifndef FLAKE_SAVE_L8_TEXTURE_TO_FILE_HPP_INCLUDED
#define FLAKE_SAVE_L8_TEXTURE_TO_FILE_HPP_INCLUDED

#include <sge/image2d/system_fwd.hpp>
#include <sge/renderer/texture/planar_fwd.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/filesystem/path.hpp>
#include <fcppt/config/external_end.hpp>


namespace flake
{
void
save_l8_texture_to_file(
	sge::image2d::system &,
	sge::renderer::texture::planar &,
	boost::filesystem::path const &);
}

#endif

