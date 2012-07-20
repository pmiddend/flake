#ifndef FLAKE_VOLUME_SNOW_COVER_PARALLEL_UPDATE_HPP_INCLUDED
#define FLAKE_VOLUME_SNOW_COVER_PARALLEL_UPDATE_HPP_INCLUDED

#include <flakelib/buffer/volume_view_impl.hpp>
#include <flakelib/marching_cubes/cpu/object_fwd.hpp>
#include <flake/volume/flakes/snow_density_view.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/thread.hpp>
#include <fcppt/config/external_end.hpp>


namespace flake
{
namespace volume
{
namespace snow_cover
{
class parallel_update
{
FCPPT_NONCOPYABLE(
	parallel_update);
public:
	parallel_update(
		flakelib::marching_cubes::cpu::object &,
		sge::opencl::command_queue::object &,
		flake::volume::flakes::snow_density_view const &);

	void
	restart_if_finished();

	~parallel_update();
private:
	flakelib::marching_cubes::cpu::object &marching_cubes_;
	sge::opencl::command_queue::object &command_queue_;
	flake::volume::flakes::snow_density_view const buffer_;
	boost::mutex dirty_mutex_;
	boost::mutex finished_mutex_;
	bool finished_;
	boost::condition_variable not_dirty_;
	boost::thread thread_;

	void
	thread();
};
}
}
}

#endif
