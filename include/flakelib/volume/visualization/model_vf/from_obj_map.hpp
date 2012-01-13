#ifndef FLAKELIB_VOLUME_VISUALIZATION_MODEL_VF_FROM_OBJ_MAP_HPP_INCLUDED
#define FLAKELIB_VOLUME_VISUALIZATION_MODEL_VF_FROM_OBJ_MAP_HPP_INCLUDED

#include <flakelib/volume/visualization/model_vf/format_part.hpp>
#include <sge/model/obj/vb_converter/convert.hpp>
#include <fcppt/mpl/index_of.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/map/map10.hpp>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace volume
{
namespace visualization
{
namespace model_vf
{
typedef
boost::mpl::map3
<
	boost::mpl::pair
	<
		sge::model::obj::vb_converter::roles::position,
		fcppt::mpl::index_of
		<
			model_vf::format_part::elements,
			model_vf::position
		>
	>,
	boost::mpl::pair
	<
		sge::model::obj::vb_converter::roles::texcoord,
		fcppt::mpl::index_of
		<
			model_vf::format_part::elements,
			model_vf::texcoord
		>
	>,
	boost::mpl::pair
	<
		sge::model::obj::vb_converter::roles::normal,
		fcppt::mpl::index_of
		<
			model_vf::format_part::elements,
			model_vf::normal
		>
	>
>
from_obj_map;
}
}
}
}

#endif
