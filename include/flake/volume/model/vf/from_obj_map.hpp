#ifndef FLAKE_VOLUME_MODEL_VF_FROM_OBJ_MAP_HPP_INCLUDED
#define FLAKE_VOLUME_MODEL_VF_FROM_OBJ_MAP_HPP_INCLUDED

#include <flake/volume/model/vf/format_part.hpp>
	//#include <sge/model/obj/vb_converter/convert.hpp>
#include <fcppt/mpl/index_of.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/map/map10.hpp>
#include <fcppt/config/external_end.hpp>

namespace flake
{
namespace volume
{
namespace model
{
namespace vf
{
	/*
typedef
boost::mpl::map3
<
	boost::mpl::pair
	<
		sge::model::obj::vb_converter::roles::position,
		fcppt::mpl::index_of
		<
			vf::format_part::elements,
			vf::position
		>
	>,
	boost::mpl::pair
	<
		sge::model::obj::vb_converter::roles::texcoord,
		fcppt::mpl::index_of
		<
			vf::format_part::elements,
			vf::texcoord
		>
	>,
	boost::mpl::pair
	<
		sge::model::obj::vb_converter::roles::normal,
		fcppt::mpl::index_of
		<
			vf::format_part::elements,
			vf::normal
		>
	>
>
from_obj_map;
	*/
}
}
}
}

#endif

