#ifndef FLAKELIB_VOLUME_VISUALIZATION_ARROWS_MANAGER_HPP_INCLUDED
#define FLAKELIB_VOLUME_VISUALIZATION_ARROWS_MANAGER_HPP_INCLUDED

#include <sge/shader/object.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/vertex_declaration_ptr.hpp>
#include <sge/renderer/vertex_declaration_fwd.hpp>
#include <fcppt/noncopyable.hpp>

namespace flakelib
{
namespace volume
{
namespace visualization
{
class arrows_manager
{
FCPPT_NONCOPYABLE(
	arrows_manager);
public:
	explicit
	arrows_manager(
		sge::renderer::device &);

	sge::renderer::vertex_declaration const &
	vertex_declaration() const;

	sge::shader::object &
	shader();

	sge::shader::object const &
	shader() const;

	sge::renderer::device &
	renderer() const;

	~arrows_manager();
private:
	sge::renderer::device &renderer_;
	sge::renderer::vertex_declaration_ptr vertex_declaration_;
	sge::shader::object shader_;
};
}
}
}

#endif
