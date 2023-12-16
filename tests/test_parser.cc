// STL
#include <filesystem>
#include <iostream>

// vat
#include <vat/parser/parser.hh>

#define ASSERT(Message, ...)                                                                                           \
	do                                                                                                                 \
	{                                                                                                                  \
		if ((__VA_ARGS__)) break;                                                                                      \
		std::cerr << Message << std::endl;                                                                             \
		std::exit(1);                                                                                                  \
	} while (false)

static constexpr std::string_view good_assets[] = {
	"test_assets/good",
};

static constexpr std::string_view bad_assets[] = {
	"test_assets/parsing",
};

int main()
{
	namespace fs = std::filesystem;
	using namespace vat::parser;
	using namespace vat::ast;

	Parser parser;

	for (fs::path dir: good_assets)
	{
		ASSERT("not a directory: " << dir, fs::is_directory(dir));
		for (fs::directory_entry const& file: fs::directory_iterator{ dir })
		{
			if (!file.is_regular_file() || file.path().extension() != ".vat") continue;
			SharedAst ast = parser.parse(file.path().string());
			ASSERT("failed to parse good asset: " << file.path(), ast);
			std::cout << "correctly parsed " << file.path() << std::endl;
		}
	}

	for (fs::path dir: bad_assets)
	{
		ASSERT("not a directory: " << dir, fs::is_directory(dir));
		for (fs::directory_entry const& file: fs::directory_iterator{ dir })
		{
			if (!file.is_regular_file() || file.path().extension() != ".vat") continue;
			SharedAst ast = parser.parse(file.path().string());
			ASSERT("parsed bad asset: " << file.path(), !ast);
			std::cout << "correctly failed to parse " << file.path() << std::endl;
		}
	}

	return 0;
}
