#include <flake/message_box.hpp>
#include <fcppt/config/platform.hpp>
#include <fcppt/text.hpp>

#if FCPPT_CONFIG_WINDOWS_PLATFORM
#include <fcppt/config/include_windows.hpp>
#else
#include <fcppt/io/cerr.hpp>
#endif

void
flake::message_box(
	fcppt::string const &s)
{
#if FCPPT_CONFIG_WINDOWS_PLATFORM
	::MessageBox(
		NULL,
		s.c_str(),
		FCPPT_TEXT("Error"),
		MB_OK | MB_ICONERROR);
#else
	fcppt::io::cerr() << FCPPT_TEXT("Error: ") << s << FCPPT_TEXT("\n");
#endif
}
