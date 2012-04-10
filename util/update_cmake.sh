#!/bin/bash

function die()
{
	exit -1
}

function update_cmake_file()
{
	local cmakefile="$1"

	update_cmake \
		"${cmakefile}" \
		"${@:2}" \
		|| die

	mv "${cmakefile}".new "${cmakefile}" || die
}

update_cmake_file \
	CMakeLists.txt \
	FLAKELIB_FILES \
	src/flakelib \

update_cmake_file \
	CMakeLists.txt \
	FLAKE_FILES \
	-n \
	src/flake \
	src/flake/planar \
	src/flake/volume \
	src/flake/volume/arrows \
	src/flake/planar/monitor \
	src/flake/notifications \
	src/flake/time_modifier
