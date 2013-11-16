#include <flake/volume/snow_cover/parallel_update.hpp>
#include <flakelib/marching_cubes/cpu/object.hpp>
#include <fcppt/config/external_begin.hpp>
#include <functional>
#include <mutex>
#include <fcppt/config/external_end.hpp>

flake::volume::snow_cover::parallel_update::parallel_update(
	flakelib::marching_cubes::cpu::object &_marching_cubes,
	sge::opencl::command_queue::object &_command_queue,
	flake::volume::flakes::snow_density_view const &_buffer)
:
	marching_cubes_(
		_marching_cubes),
	command_queue_(
		_command_queue),
	buffer_(
		_buffer),
	dirty_mutex_(),
	finished_mutex_(),
	finished_(
		true),
	not_dirty_(),
	thread_(
		std::bind(
			&flake::volume::snow_cover::parallel_update::thread,
			this))
{
}

void
flake::volume::snow_cover::parallel_update::restart_if_finished()
{
	{
		std::unique_lock<std::mutex> l(
			finished_mutex_);

		if(!finished_)
			return;
	}

	/*
	boost::unique_lock<boost::mutex> lk(
		dirty_mutex_);
	*/
	//std::cout << "Updating buffers, constructing from OpenCL buffer\n";

	marching_cubes_.update_buffers();

	marching_cubes_.construct_from_cl_buffer(
		command_queue_,
		buffer_.get());

	not_dirty_.notify_all();
}

flake::volume::snow_cover::parallel_update::~parallel_update()
{
	thread_.join();
}

void
flake::volume::snow_cover::parallel_update::thread()
{
	// FIXME: finished is always true and std::thread doesn't support thread interruption
	for(;;)
	{
		{
			std::unique_lock<std::mutex> l(
				dirty_mutex_);

			//		std::cout << "Snow cover update thread: Waiting\n";

			while(!marching_cubes_.is_dirty())
				not_dirty_.wait(
					l);

			//			std::cout << "Snow cover update thread: Creating new snowcover\n";
			marching_cubes_.run();
		}

		std::unique_lock<std::mutex> l(
			finished_mutex_);

		finished_ =
			true;
	}
}
