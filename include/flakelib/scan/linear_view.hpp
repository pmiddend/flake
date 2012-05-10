#ifndef FLAKELIB_SCAN_LINEAR_VIEW_HPP_INCLUDED
#define FLAKELIB_SCAN_LINEAR_VIEW_HPP_INCLUDED

#include <flakelib/buffer/linear_view_fwd.hpp>
#include <flakelib/scan/value_type.hpp>

namespace flakelib
{
namespace scan
{
typedef
flakelib::buffer::linear_view<flakelib::scan::value_type>
linear_view;
}
}

#endif

