#include <flakelib/console_percent_bar.hpp>
#include <flakelib/volume/create_snow_volume_texture.hpp>
#include <sge/image/store.hpp>
#include <sge/image/view/const_object.hpp>
#include <sge/image3d/l8.hpp>
#include <sge/image3d/view/const_elements_wrapper.hpp>
#include <sge/image3d/view/object.hpp>
#include <sge/image3d/view/to_const.hpp>
#include <sge/noise/sample.hpp>
#include <sge/noise/sample_parameters.hpp>
#include <sge/noise/simplex/object_impl.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scalar.hpp>
#include <sge/renderer/texture/create_volume_from_view.hpp>
#include <sge/renderer/texture/volume.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <mizuiro/color/channel/luminance.hpp>
#include <mizuiro/image/store.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/config/external_begin.hpp>
#include <iostream>
#include <ostream>
#include <fcppt/config/external_end.hpp>


sge::renderer::texture::volume_unique_ptr
flakelib::volume::create_snow_volume_texture(
	sge::renderer::device &_renderer,
	sge::renderer::dim3 const &_size)
{
	typedef
	sge::image3d::l8
	store_type;

	typedef
	store_type::view_type
	view_type;

	typedef
	sge::noise::simplex::object<sge::renderer::scalar,3>
	noise_type;

	typedef
	sge::noise::sample_parameters<noise_type>
	param_type;

	noise_type noise_generator(
		sge::noise::simplex::width(
			1024u));

	typedef
	view_type::dim
	dim_type;

	typedef
	dim_type::value_type
	dim_value_type;

	store_type s(
		store_type::dim(
			_size.w(),
			_size.h(),
			_size.d()));

	view_type view(
		s.view());

	std::cout << "Creating snow volume texture...\n";

	{
	flakelib::console_percent_bar percent_bar(
		flakelib::console_percent_bar::maximum(
			static_cast<flakelib::console_percent_bar::numeric_type>(
				view.size()[2])));

	for(dim_value_type z = 0; z < view.size()[2]; ++z)
	{
		percent_bar.update(
			static_cast<flakelib::console_percent_bar::numeric_type>(
				z));
		for(dim_value_type y = 0; y < view.size()[1]; ++y)
		{
			for(dim_value_type x = 0; x < view.size()[0]; ++x)
			{
				dim_type const current_position(
					x,
					y,
					z);

				view[current_position].set(
					mizuiro::color::channel::luminance(),
					static_cast<sge::image::color::channel8>(
						256.0f *
						(0.5f + 0.5f *
						sge::noise::sample(
							noise_generator,
							param_type(
								param_type::position_type(
									noise_type::vector(
										static_cast<noise_type::value_type>(
											x),
										static_cast<noise_type::value_type>(
											y),
										static_cast<noise_type::value_type>(
											z))),
								param_type::amplitude_type(
									0.8f),
								param_type::frequency_type(
									.005f),
								param_type::octaves_type(
									6u))))));
			}
		}
	}
	}

	std::cout << "\nDone!\n";

	return
		sge::renderer::texture::create_volume_from_view(
			_renderer,
			sge::image3d::view::to_const(
				sge::image3d::view::object(
					s.wrapped_view())),
			sge::renderer::texture::mipmap::off(),
			sge::renderer::resource_flags::none);
}
