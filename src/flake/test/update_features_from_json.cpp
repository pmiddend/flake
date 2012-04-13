#include <flake/test/update_features_from_json.hpp>
#include <sge/parse/json/array.hpp>
#include <sge/parse/json/get.hpp>
#include <sge/parse/json/member_map.hpp>
#include <sge/parse/json/object.hpp>
#include <fcppt/assert/error_message.hpp>


flake::test::feature_sequence const
flake::test::update_features_from_json(
	test::feature_sequence _features,
	sge::parse::json::object const &_json)
{
	for(
		sge::parse::json::member_map::const_iterator current_json =
			_json.members.begin();
		current_json != _json.members.end();
		++current_json)
	{
		if(current_json->first == FCPPT_TEXT("paused"))
			continue;

		bool found_feature = false;

		for(
			test::feature_sequence::iterator current_feature =
				_features.begin();
			current_feature != _features.end();
			++current_feature)
		{
			if(current_json->first != current_feature->json_identifier().get())
				continue;

			found_feature = true;

			current_feature->is_active(
				sge::parse::json::get<bool>(
					current_json->second));
		}

		FCPPT_ASSERT_ERROR_MESSAGE(
			found_feature,
			FCPPT_TEXT("Couldn't find feature in json file"));
	}

	return
		_features;
}
