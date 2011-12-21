#ifndef FLAKELIB_PLANAR_POOL_SCOPED_LOCK_HPP_INCLUDED
#define FLAKELIB_PLANAR_POOL_SCOPED_LOCK_HPP_INCLUDED

#include <flakelib/planar_pool/object_fwd.hpp>
#include <flakelib/planar_buffer_fwd.hpp>
#include <fcppt/noncopyable.hpp>

namespace flakelib
{
namespace planar_pool
{
class scoped_lock
{
FCPPT_NONCOPYABLE(
	scoped_lock);
public:
	flakelib::planar_buffer &
	value() const;

	~scoped_lock();
private:
	friend class planar_pool::object;

	planar_pool::object &pool_;
	flakelib::planar_buffer &value_;

	explicit
	scoped_lock(
		planar_pool::object &,
		flakelib::planar_buffer &);
};
}
}

#endif
