#ifndef FLAKE_VOLUME_SNOW_COVER_PARALLEL_UPDATE_HPP_INCLUDED
#define FLAKE_VOLUME_SNOW_COVER_PARALLEL_UPDATE_HPP_INCLUDED

#include <flake/detail/symbol.hpp>
#include <flake/volume/flakes/snow_density_view.hpp>
#include <flakelib/buffer/volume_view_impl.hpp>
#include <flakelib/marching_cubes/cpu/object_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/config/external_begin.hpp>
#include <condition_variable>
#include <mutex>
#include <thread>
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
	FLAKE_DETAIL_SYMBOL
	parallel_update(
		flakelib::marching_cubes::cpu::object &,
		sge::opencl::command_queue::object &,
		flake::volume::flakes::snow_density_view const &);

	FLAKE_DETAIL_SYMBOL
	void
	restart_if_finished();

	FLAKE_DETAIL_SYMBOL
	~parallel_update();
private:
	flakelib::marching_cubes::cpu::object &marching_cubes_;
	sge::opencl::command_queue::object &command_queue_;
	flake::volume::flakes::snow_density_view const buffer_;
	std::mutex dirty_mutex_;
	std::mutex finished_mutex_;
	bool finished_;
	std::condition_variable not_dirty_;
	std::thread thread_;

	void
	thread();
};
}
}
}

#endif
