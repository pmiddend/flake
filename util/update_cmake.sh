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
	src/flake/test \
	src/flake/test/information \
	src/flake/volume \
	src/flake/volume/arrows \
	src/flake/volume/flakes \
	src/flake/volume/density_visualization/raycaster \
	src/flake/volume/model \
	src/flake/volume/obstacles \
	src/flake/planar/monitor \
	src/flake/planar/conversion \
	src/flake/notifications \
	src/flake/time_modifier
