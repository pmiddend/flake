#ifndef FLAKELIB_PLANAR_POOL_HPP_INCLUDED
#define FLAKELIB_PLANAR_POOL_HPP_INCLUDED

#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/context/object_fwd.hpp>
#include <sge/opencl/memory_object/dim2.hpp>
#include <sge/opencl/memory_object/image/planar_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <set>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace planar_pool
{
class object
{
FCPPT_NONCOPYABLE(
	object);
public:
	explicit
	object(
		sge::opencl::context::object &,
		cl_image_format);

	void
	lock(
		sge::opencl::memory_object::image::planar &);

	sge::opencl::memory_object::image::planar &
	get(
		sge::opencl::memory_object::size_type);

	sge::opencl::memory_object::image::planar &
	get(
		sge::opencl::memory_object::dim2 const &);

	void
	unlock(
		sge::opencl::memory_object::image::planar &);

	~object();
private:
	typedef
	boost::ptr_vector<sge::opencl::memory_object::image::planar>
	image_pool;

	typedef
	std::set<sge::opencl::memory_object::image::planar *>
	locked_textures;

	sge::opencl::context::object &context_;
	cl_image_format const image_format_;
	image_pool image_pool_;
	locked_textures locked_textures_;
};
}
}

#endif
