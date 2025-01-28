/**
Copyright 2025 Tristan Isham

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
#include <vector>
#include <filesystem>
#include <indicators/indicators.hpp>
#include "pesto.hpp"
#include <CLI11/CLI11.hpp>


int main(int argc, char* argv[]) {
	CLI::App app{ "Convert HEIC files to PNGs" };
	argv = app.ensure_utf8(argv);
	app.allow_windows_style_options();
	app.set_help_flag("");
	app.set_help_all_flag("-h, --help");
	app.set_version_flag("--version", pesto::meta::VERSION);

	// outdir
	std::string outdir = "out";
	app.add_option("-o,--out", outdir, "the directory to write out files");

	// input
	std::vector<std::string> files;
	app.add_option("-f,--file", files, "A list of input files")->expected(-2);

	app.require_option(1);

	CLI11_PARSE(app, argc, argv);

	if (!files.empty()) {

		if (!outdir.empty()) {
			std::filesystem::create_directories(outdir);
		}

		std::vector<std::filesystem::path> targets;

		for (const auto& f : files) {

			std::filesystem::path target{ f };
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
			if (!outdir.empty()) {
				std::filesystem::path output_dir{ outdir };
				output_path = output_dir / p.filename();
				output_path.replace_extension(".png");
			}
			else {
				output_path = p;
				output_path.replace_extension(".png");
			}
			auto result = pesto::heif_to_png(p.string(), output_path);
			if (result > 0) {
				std::cout << "Error converting: " << p << std::endl;
			}

			pbar.tick();
		}


		if (pbar.is_completed()) {
			std::cout << "Finished converting images" << std::endl;
		}

	}




	return 0;
}

