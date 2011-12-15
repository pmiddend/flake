#ifndef FLAKELIB_MAIN_HEAD_HPP_INCLUDED
#define FLAKELIB_MAIN_HEAD_HPP_INCLUDED

#include <fcppt/config/platform.hpp>

#ifdef FCPPT_CONFIG_WINDOWS_PLATFORM

#include <fcppt/config/include_windows.hpp>

#define FLAKELIB_MAIN_HEAD \
int CALLBACK \
WinMain(\
	HINSTANCE,\
	HINSTANCE,\
	LPSTR,\
	int)
#else

#define FLAKELIB_MAIN_HEAD \
int \
main(\
	int,\
	char *[])
#endif

#endif
