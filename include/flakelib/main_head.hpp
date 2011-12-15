#ifndef FLAKELIB_MAIN_HEAD_HPP_INCLUDED
#define FLAKELIB_MAIN_HEAD_HPP_INCLUDED

#include <fcppt/config/platform.hpp>

#ifdef FCPPT_CONFIG_PLATFORM_WINDOWS

#include <fcppt/config/include_windows.hpp>

#define FLAKELIB_MAIN_HEAD \
int CALLBACK\
WinMain(\
	HINSTANCE hInstance,\
	HINSTANCE hPrevInstance,\
	LPSTR lpCmdLine,\
	int nCmdShow)
#else

#define FLAKELIB_MAIN_HEAD \
int \
main(\
	int argc,\
	char *argv[])

#endif

#endif
