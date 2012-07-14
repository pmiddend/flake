#ifndef FLAKE_SHADER_SCOPED_PAIR_HPP_INCLUDED
#define FLAKE_SHADER_SCOPED_PAIR_HPP_INCLUDED

#include <sge/renderer/cg/scoped_program.hpp>
#include <flake/shader/pair_fwd.hpp>
#include <sge/renderer/context/object_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <sge/renderer/cg/scoped_texture.hpp>

namespace flake
{
namespace shader
{
class scoped_pair
{
	FCPPT_NONCOPYABLE(
		scoped_pair);
public:
	scoped_pair(
		sge::renderer::context::object &,
		flake::shader::pair &);

	~scoped_pair();
private:
	flake::shader::pair &parent_;
	sge::renderer::cg::scoped_program scoped_vertex_program_;
	sge::renderer::cg::scoped_program scoped_pixel_program_;
};
}
}

#endif
