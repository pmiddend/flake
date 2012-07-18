#ifndef FLAKE_SHADER_PARAMETER_PLANAR_TEXTURE_HPP_INCLUDED
#define FLAKE_SHADER_PARAMETER_PLANAR_TEXTURE_HPP_INCLUDED

#include <flake/shader/pair_fwd.hpp>
#include <flake/shader/parameter/name.hpp>
#include <sge/cg/parameter/named.hpp>
#include <sge/cg/program/object_fwd.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/cg/loaded_texture_scoped_ptr.hpp>
#include <sge/renderer/cg/scoped_texture.hpp>
#include <sge/renderer/texture/planar_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/optional.hpp>
#include <fcppt/optional_fwd.hpp>
#include <fcppt/scoped_ptr.hpp>


namespace flake
{
namespace shader
{
namespace parameter
{
class planar_texture
{
FCPPT_NONCOPYABLE(
	planar_texture);
public:
	typedef
	fcppt::optional<sge::renderer::texture::planar &>
	optional_value;

	planar_texture(
		flake::shader::pair &,
		sge::renderer::device &,
		sge::cg::program::object &,
		flake::shader::parameter::name const &,
		optional_value const &);

	void
	set(
		sge::renderer::texture::planar &);

	void
	activate(
		sge::renderer::context::object &);

	void
	deactivate();

	sge::renderer::cg::loaded_texture &
	loaded_texture();

	sge::renderer::texture::stage const
	stage() const;

	~planar_texture();
private:
	typedef
	fcppt::optional<sge::renderer::context::object &>
	optional_render_context;

	flake::shader::pair &parent_;
	sge::renderer::device &renderer_;
	sge::cg::parameter::named const parameter_;
	sge::renderer::cg::loaded_texture_scoped_ptr loaded_texture_;
	fcppt::scoped_ptr<sge::renderer::cg::scoped_texture> scoped_texture_;
	optional_render_context optional_render_context_;
	optional_value value_;
};
}
}
}

#endif
