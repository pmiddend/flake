#ifndef FLAKE_SHADER_PAIR_HPP_INCLUDED
#define FLAKE_SHADER_PAIR_HPP_INCLUDED

#include <flake/shader/context_fwd.hpp>
#include <flake/shader/pixel_program_path.hpp>
#include <flake/shader/scoped_pair_fwd.hpp>
#include <flake/shader/vertex_program_path.hpp>
#include <flake/shader/parameter/matrix.hpp>
#include <flake/shader/parameter/planar_texture_fwd.hpp>
#include <sge/cg/program/object.hpp>
#include <sge/renderer/vertex_declaration_fwd.hpp>
#include <sge/renderer/cg/loaded_program_scoped_ptr.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <fcppt/config/external_end.hpp>


namespace flake
{
namespace shader
{
class pair
{
	FCPPT_NONCOPYABLE(
pair);
public:
	pair(
		flake::shader::context &,
		sge::renderer::vertex_declaration &,
		flake::shader::vertex_program_path const &,
		flake::shader::pixel_program_path const &);

	flake::shader::context &
	context() const;

	sge::cg::program::object &
	vertex_program();

	sge::cg::program::object &
	pixel_program();

	sge::renderer::cg::loaded_program &
	loaded_vertex_program();

	sge::renderer::cg::loaded_program &
	loaded_pixel_program();

	~pair();
private:
	friend class flake::shader::parameter::planar_texture;
	friend class flake::shader::scoped_pair;

	typedef
	boost::ptr_vector
	<
		flake::shader::parameter::planar_texture,
		boost::view_clone_allocator
	>
	planar_texture_sequence;

	flake::shader::context &context_;
	sge::cg::program::object vertex_program_;
	sge::cg::program::object pixel_program_;
	sge::renderer::cg::loaded_program_scoped_ptr loaded_vertex_program_;
	sge::renderer::cg::loaded_program_scoped_ptr loaded_pixel_program_;
	planar_texture_sequence planar_textures_;

	void
	add_planar_texture(
		flake::shader::parameter::planar_texture &);

	void
	remove_planar_texture(
		flake::shader::parameter::planar_texture &);
};
}
}

#endif
