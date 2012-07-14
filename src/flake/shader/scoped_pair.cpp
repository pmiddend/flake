#include <flake/shader/context.hpp>
#include <flake/shader/pair.hpp>
#include <flake/shader/scoped_pair.hpp>
#include <flake/shader/parameter/planar_texture.hpp>


flake::shader::scoped_pair::scoped_pair(
	sge::renderer::context::object &_render_context,
	flake::shader::pair &_parent)
:
	parent_(
		_parent),
	scoped_vertex_program_(
		_render_context,
		parent_.loaded_vertex_program()),
	scoped_pixel_program_(
		_render_context,
		parent_.loaded_pixel_program())
{
	for(
		flake::shader::pair::planar_texture_sequence::iterator it =
			parent_.planar_textures_.begin();
		it != parent_.planar_textures_.end();
		++it)
		it->activate(
			_render_context);
}

flake::shader::scoped_pair::~scoped_pair()
{
	for(
		flake::shader::pair::planar_texture_sequence::iterator it =
			parent_.planar_textures_.begin();
		it != parent_.planar_textures_.end();
		++it)
		it->deactivate();
}
