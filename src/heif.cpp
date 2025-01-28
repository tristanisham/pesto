#include <pesto.hpp>

int pesto::heif_to_png(std::string const& from, std::optional<std::filesystem::path> to) {

	heif_context* ctx = heif_context_alloc();
	auto err = heif_context_read_from_file(ctx, from.c_str(), nullptr);
	if (err.code > 0) {
		std::cerr << err.message << std::endl;
		return 1;
	}

	heif_image_handle* handle;
	err = heif_context_get_primary_image_handle(ctx, &handle);
	if (err.code > 0) {
		std::cerr << err.message << std::endl;
		return 1;
	}

	heif_image* img;
	err = heif_decode_image(handle, &img, heif_colorspace_RGB, heif_chroma_interleaved_RGB, nullptr);
	if (err.code > 0) {
		std::cerr << err.message << std::endl;
		return 1;
	}

	int stride;
	uint8_t const* data = heif_image_get_plane_readonly(img, heif_channel_interleaved, &stride);

	// Retrieve the dimensions of the image
	int width = heif_image_handle_get_width(handle);
	int height = heif_image_handle_get_height(handle);


	std::filesystem::path to_path;
	if (to.has_value()) {
		to_path = to.value();
	}
	else {
		std::filesystem::path tp_path{ from };
		tp_path.replace_extension(".png");
		to_path = tp_path;
	}

	std::vector<uint8_t> bytes{ data, data + stride * height };

	unsigned error = lodepng::encode(to_path.string(), bytes, (unsigned)width, (unsigned)height, LCT_RGB);
	if (error) {
		//std::cerr << "stride*height " << stride * height << " w: " << width << " h: " << height << std::endl;
		std::cerr << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	}

	heif_image_release(img);
	heif_image_handle_release(handle);
	heif_context_free(ctx);

	return 0;
}