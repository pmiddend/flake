#ifndef FLAKE_TEST_UPDATE_FEATURES_FROM_JSON_HPP_INCLUDED
#define FLAKE_TEST_UPDATE_FEATURES_FROM_JSON_HPP_INCLUDED

#include <flake/test/feature_sequence.hpp>
#include <sge/parse/json/object_fwd.hpp>

namespace flake
{
namespace test
{
test::feature_sequence const
update_features_from_json(
	test::feature_sequence,
	sge::parse::json::object const &);
}
}

#endif

