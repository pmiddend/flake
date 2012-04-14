#ifndef FLAKE_VOLUME_MODEL_OBJECT_HPP_INCLUDED
#define FLAKE_VOLUME_MODEL_OBJECT_HPP_INCLUDED

#include <flake/volume/model/identifier.hpp>
#include <flake/volume/model/manager_fwd.hpp>
#include <flake/volume/model/position.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/math/vector/object_impl.hpp>
#include <fcppt/preprocessor/disable_gcc_warning.hpp>
#include <fcppt/preprocessor/pop_warning.hpp>
#include <fcppt/preprocessor/push_warning.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/intrusive/list.hpp>
#include <fcppt/config/external_end.hpp>


namespace flake
{
namespace volume
{
namespace model
{
FCPPT_PP_PUSH_WARNING
FCPPT_PP_DISABLE_GCC_WARNING(-Weffc++)

class object
:
	public boost::intrusive::list_base_hook
	<
		boost::intrusive::link_mode<boost::intrusive::auto_unlink>
	>
{
FCPPT_NONCOPYABLE(
	object);
public:
	explicit
	object(
		model::manager &,
		model::identifier const &_identifier,
		model::position const &_position);

	model::identifier const &
	identifier() const;

	model::position const &
	position() const;

	~object();
private:
	model::identifier identifier_;
	model::position position_;
};

FCPPT_PP_POP_WARNING
}
}
}

#endif

