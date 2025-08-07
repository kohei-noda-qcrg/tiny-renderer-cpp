#include <filesystem>
#include <format>
#include <source_location>
#include <string>

namespace util {
inline auto gen_test_output_name(const std::filesystem::path input_file, std::string_view suffix, const std::source_location& loc) -> std::string {
    return std::format("{}_{}{}", std::filesystem::path(loc.file_name()).stem().string(), input_file.stem().string(), suffix);
}
} // namespace util

#define GEN_TEST_OUTPUT_NAME(input_file, suffix) \
    util::gen_test_output_name(input_file, suffix, std::source_location::current())
