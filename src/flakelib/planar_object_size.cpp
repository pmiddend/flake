#include <flakelib/exception.hpp>
#include <flakelib/planar_object_size.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <fcppt/variant/apply_unary.hpp>
#include <fcppt/text.hpp>


namespace
{
struct planar_object_visitor
{
public:
	typedef
	sge::opencl::memory_object::dim2
	result_type;

	result_type
	operator()(
		flakelib::planar_buffer const &b) const
	{
		return b.size();
	}

	result_type
	operator()(
		sge::opencl::memory_object::image::planar const * const b) const
	{
		return b->size();
	}

	result_type
	operator()(
		flakelib::invalid_planar_object const &) const
	{
		throw flakelib::exception(FCPPT_TEXT("Tried to get the size of an invalid planar object"));
	}
};
}

sge::opencl::memory_object::dim2 const
flakelib::planar_object_size(
	flakelib::planar_object const &po)
{
	return
		fcppt::variant::apply_unary(
			planar_object_visitor(),
			po);
}
