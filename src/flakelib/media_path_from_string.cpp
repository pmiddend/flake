#include <flakelib/media_path_from_string.hpp>
#include <flakelib/media_path.hpp>
#include <fcppt/text.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/operator.hpp>
#include <vector>
#include <numeric>

fcppt::filesystem::path const
flakelib::media_path_from_string(
	fcppt::string const &s)
{
	typedef
	std::vector<fcppt::string>
	string_sequence;

	string_sequence result;
	boost::algorithm::split(
		result,
		s,
		boost::algorithm::is_any_of(
			FCPPT_TEXT("/")));

	return
		std::accumulate(
			result.begin(),
			result.end(),
			flakelib::media_path(),
			boost::phoenix::arg_names::arg1 / boost::phoenix::arg_names::arg2);
}