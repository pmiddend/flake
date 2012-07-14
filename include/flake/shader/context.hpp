#ifndef FLAKE_SHADER_CONTEXT_HPP_INCLUDED
#define FLAKE_SHADER_CONTEXT_HPP_INCLUDED

#include <sge/renderer/device_fwd.hpp>
#include <sge/cg/context/object.hpp>
#include <sge/cg/profile/object.hpp>
#include <sge/cg/program/compile_options.hpp>
#include <fcppt/noncopyable.hpp>

namespace flake
{
namespace shader
{
class context
{
FCPPT_NONCOPYABLE(
	context);
public:
	explicit
	context(
		sge::renderer::device &);

	sge::renderer::device &
	renderer() const;

	sge::cg::context::object &
	cg_context();

	sge::cg::context::object const &
	cg_context() const;

	sge::cg::profile::object const &
	vertex_profile() const;

	sge::cg::profile::object const &
	pixel_profile() const;

	sge::cg::program::compile_options const
	vertex_compile_options() const;

	sge::cg::program::compile_options const
	pixel_compile_options() const;

	~context();
private:
	sge::renderer::device &renderer_;
	sge::cg::context::object cg_context_;
	sge::cg::profile::object const vertex_profile_;
	sge::cg::profile::object const pixel_profile_;
};
}
}

#endif
