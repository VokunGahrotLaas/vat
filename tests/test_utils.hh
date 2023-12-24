#pragma once

// STL
#include <array>
#include <filesystem>
#include <iostream>

// vat
#include <vat/bind/binder.hh>
#include <vat/eval/ast.hh>
#include <vat/parser/parser.hh>
#include <vat/utils/error.hh>

static constexpr auto lexing_assets = std::to_array<std::string_view>({
	"test_assets/lexing",
});

static constexpr auto parsing_assets = std::to_array<std::string_view>({
	"test_assets/parsing",
});

static constexpr auto binding_assets = std::to_array<std::string_view>({
	"test_assets/binding",
});

static constexpr auto typing_assets = std::to_array<std::string_view>({
	"test_assets/typing",
});

static constexpr auto good_assets = std::to_array<std::string_view>({
	"test_assets/good",
});

enum class FailOn
{
	Lexing,
	Parsing,
	Binding,
	Typing,
	Evaluation,
	Nothing,
};

enum class SuccessUntil
{
	Lexing,
	Parsing,
	Binding,
	Typing,
	Evaluation,
	End,
};

template <size_t N>
int tests_on(std::array<std::string_view, N> assets, FailOn fail_on = FailOn::Nothing,
			 SuccessUntil success_until = SuccessUntil::End)
{
	namespace fs = std::filesystem;
	using namespace vat;

	utils::ErrorType code = utils::ErrorType::None;
	for (fs::path dir: assets)
	{
		if (!fs::is_directory(dir))
		{
			std::cerr << dir << ": not a directory" << std::endl;
			code |= utils::ErrorType::Other;
			continue;
		}
		for (fs::directory_entry const& file: fs::directory_iterator{ dir })
		{
			if (!file.is_regular_file() || file.path().extension() != ".vat") continue;
			utils::ErrorManager em;
			parser::Parser parser;
			ast::SharedAst ast = parser.parse(file.path().string());
			if (fail_on == FailOn::Lexing || fail_on == FailOn::Parsing)
			{
				if (ast)
				{
					std::cerr << file << ": parsing - should have failed but didn't" << std::endl;
					code |= vat::utils::ErrorManager::Parsing;
				}
				else
					std::cout << file << ": parsing - success" << std::endl;
				continue;
			}
			if (!ast)
			{
				std::cerr << file << ": parsing - should have succeded but didn't" << std::endl;
				code |= vat::utils::ErrorManager::Parsing;
				continue;
			}
			std::cout << file << ": parsing - success" << std::endl;
			if (success_until == SuccessUntil::Lexing || success_until == SuccessUntil::Parsing) continue;
			bind::Binder binder{ em };
			binder.bind(*ast);
			if (fail_on == FailOn::Binding)
			{
				if (!em)
				{
					std::cerr << file << ": binding - should have failed but didn't" << std::endl;
					code |= vat::utils::ErrorManager::Binding;
				}
				else
					std::cout << file << ": binding - success" << std::endl;
				continue;
			}
			if (em)
			{
				std::cerr << em;
				std::cerr << file << ": binding - should have succeded but didn't" << std::endl;
				code |= vat::utils::ErrorManager::Binding;
				continue;
			}
			std::cout << file << ": binding - success" << std::endl;
			if (success_until == SuccessUntil::Binding) continue;
			eval::AstEvaluator evaluator{ em };
			eval::ast_exp::exp_type exp = evaluator.eval(*ast);
			if (fail_on == FailOn::Typing || fail_on == FailOn::Evaluation)
			{
				if (!em)
				{
					std::cerr << file << ": evaluation - should have failed but didn't" << std::endl;
					code |= vat::utils::ErrorManager::Evaluation;
				}
				else
					std::cout << file << ": evaluation - success" << std::endl;
				continue;
			}
			if (em)
			{
				std::cerr << em;
				std::cerr << file << ": evaluation - should have succeded but didn't" << std::endl;
				code |= vat::utils::ErrorManager::Evaluation;
				continue;
			}
			std::cout << file << ": evaluation - success" << std::endl;
			if (success_until == SuccessUntil::Typing || success_until == SuccessUntil::Evaluation) continue;
		}
	}
	return static_cast<int>(code);
}

template <size_t N>
int tests_on(std::array<std::string_view, N> assets, SuccessUntil success_until = SuccessUntil::End,
			 FailOn fail_on = FailOn::Nothing)
{
	return tests_on(assets, fail_on, success_until);
}
