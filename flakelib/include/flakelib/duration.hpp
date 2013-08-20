#ifndef FLAKELIB_DURATION_HPP_INCLUDED
#define FLAKELIB_DURATION_HPP_INCLUDED

#include <sge/renderer/scalar.hpp>
#include <fcppt/config/external_begin.hpp>
#include <chrono>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
typedef
std::chrono::duration<sge::renderer::scalar>
duration;
}

#endif
