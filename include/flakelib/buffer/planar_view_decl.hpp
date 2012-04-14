#ifndef FLAKELIB_BUFFER_PLANAR_VIEW_DECL_HPP_INCLUDED
#define FLAKELIB_BUFFER_PLANAR_VIEW_DECL_HPP_INCLUDED

#include <flakelib/buffer/planar_view_fwd.hpp>
#include <sge/opencl/memory_object/buffer_fwd.hpp>
#include <sge/opencl/memory_object/dim2.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/preprocessor/disable_gcc_warning.hpp>
#include <fcppt/preprocessor/pop_warning.hpp>
#include <fcppt/preprocessor/push_warning.hpp>

namespace flakelib
{
namespace buffer
{
FCPPT_PP_PUSH_WARNING
FCPPT_PP_DISABLE_GCC_WARNING(-Weffc++)

template<typename T>
class planar_view
{
public:
	typedef
	T
	value_type;

	planar_view(
		sge::opencl::memory_object::buffer &,
		sge::opencl::memory_object::dim2 const &);

	sge::opencl::memory_object::buffer &
	buffer() const;

	sge::opencl::memory_object::dim2 const &
	size() const;

	~planar_view();
private:
	sge::opencl::memory_object::buffer *buffer_;
	sge::opencl::memory_object::dim2 size_;
};

FCPPT_PP_POP_WARNING
}
}

#endif
