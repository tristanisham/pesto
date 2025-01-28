/**
Copyright 2023 Tristan Isham

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the �Software�),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED �AS IS�, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <iostream>
#include <libheif/heif.h>
#include <lodepng/lodepng.h>
#include <vector>
#include <filesystem>
#include <indicators/indicators.hpp>

int heif_to_png(std::string const& from, std::optional<std::filesystem::path> to) {

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

int main(int argc, char* argv[]) {
	std::vector<std::filesystem::path> targets;
	std::optional<std::string> outDir;

	for (int i = 1; i < argc; i++) {
		std::string arg{ argv[i] };

		// Command line handler
		if (arg == "--outdir") {
			if (argc > i + 1) {
				outDir = std::string{ argv[i + 1] };
				i += 1;
			}
			continue;
		}


		std::filesystem::path target{ arg };
		if (std::filesystem::exists(target)) {
			if (std::filesystem::is_directory(target)) {
				for (auto const& entry : std::filesystem::recursive_directory_iterator(target)) {
					std::filesystem::path entry_path{ entry };
					if (entry_path.extension() == ".heic") {
						targets.push_back(entry_path);
					}
				}
			}
			else if (std::filesystem::is_regular_file(target) && target.extension() == ".heic") {
				targets.push_back(target);
			}
		}
	}

	if (outDir.has_value()) {
		std::filesystem::create_directories(outDir.value());
	}

	indicators::ProgressBar pbar{
	   indicators::option::BarWidth{50},
	   indicators::option::Start{"["},
	   indicators::option::Fill{"="},
	   indicators::option::Lead{">"},
	   indicators::option::Remainder{" "},
	   indicators::option::End{"]"},
	   indicators::option::PostfixText{"HEIF -> PNG"},
	   indicators::option::ForegroundColor{indicators::Color::yellow},
	   indicators::option::ShowElapsedTime{true},
	   indicators::option::ShowRemainingTime{true},
	   indicators::option::ShowPercentage{true},
	   indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}},
	   indicators::option::MaxProgress{targets.size()}
	};

	
	for (std::filesystem::path const& p : targets) {
		std::filesystem::path output_path;
		if (outDir) {
			std::filesystem::path output_dir{ outDir.value() };
			output_path = output_dir / p.filename();
			output_path.replace_extension(".png");
		}
		else {
			output_path = p;
			output_path.replace_extension(".png");
		}
		auto result = heif_to_png(p.string(), output_path);
		if (result > 0) {
			std::cout << "Error converting: " << p << std::endl;
		}

		pbar.tick();
	}


	if (pbar.is_completed()) {
		std::cout << "Finished converting images" << std::endl;
	}

	return 0;
}

