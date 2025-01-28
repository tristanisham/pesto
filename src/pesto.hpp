#pragma
#include <string>
#include <libheif/heif.h>
#include <lodepng/lodepng.h>
#include <optional>
#include <iostream>
#include <filesystem>

namespace pesto {
	int heif_to_png(std::string const& from, std::optional<std::filesystem::path> to);



}

namespace pesto::meta {

#ifdef NDEBUG
    inline const char* VERSION = "v0.0.2";
#else
    inline const char* VERSION = "v0.0.2 DEBUG";
#endif


}