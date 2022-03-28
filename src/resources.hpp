#pragma once

#include <filesystem>
#include <spdlog/spdlog.h>
#include <stdexcept>

class Resources
{
  public:
	static std::string find_shader(const char* name)
	{
		auto root = get_root();
		auto path = root / name;
		if (std::filesystem::exists(path)) {
			spdlog::info("Found shader: {}", path.string());
			return path.string();
		} else {
			spdlog::error("No shader '{}' found in {}", name, root.string());
			throw std::runtime_error("Cannot find font");
		}
	}

	static std::string find_font(const char* name)
	{
		auto root = get_root() / "fonts";
		auto path = root / name;
		if (std::filesystem::exists(path)) {
			spdlog::info("Found font: {}", path.string());
			return path.string();
		} else {
			spdlog::error("No font '{}' found in {}", name, root.string());
			throw std::runtime_error("Cannot find font");
		}
	}

  private:
	static std::filesystem::path get_root()
	{
		// TODO change this to use some invironment variable or something for
		// locating resource files
		return std::filesystem::current_path();
	}
};