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
	-n \
	src/flakelib \
	src/flakelib/cl \
	src/flakelib/value_modulator \
	src/flakelib/marching_cubes \
	src/flakelib/scan \
	src/flakelib/buffer_pool \
	src/flakelib/volume/conversion \
	src/flakelib/volume/simulation/stam \
	src/flakelib/splatter \
	src/flakelib/marching_cubes/cpu \
	src/flakelib/marching_cubes/gpu \
	src/flakelib/splatter/rectangle \
	src/flakelib/splatter/box \
	src/flakelib/utility \
	src/flakelib/volume

update_cmake_file \
	CMakeLists.txt \
	FLAKELIB_PLANAR_FILES \
	-n \
	src/flakelib/planar/simulation/stam \
	src/flakelib/planar/simulation/stam/buissnesq

update_cmake_file \
	CMakeLists.txt \
	FLAKE_PLANAR_FILES \
	-n \
	src/flake/planar \
	src/flake/planar/monitor \
	src/flake/planar/conversion \

update_cmake_file \
	CMakeLists.txt \
	FLAKE_FILES \
	-n \
	src/flake \
	src/flake/test \
	src/flake/test/information \
	src/flake/postprocessing \
	src/flake/skydome \
	src/flake/volume \
	src/flake/volume/arrows \
	src/flake/volume/snow_cover \
	src/flake/volume/flakes \
	src/flake/volume/density_visualization/raycaster \
	src/flake/volume/obstacles \
	src/flake/notifications \
	src/flake/time_modifier
