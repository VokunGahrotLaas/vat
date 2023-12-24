#pragma once

// STL
#include <array>
#include <filesystem>
#include <iostream>

// vat
#include <vat/bind/binder.hh>
#include <vat/eval/ast.hh>
#include <vat/parse/parser.hh>
#include <vat/utils/error.hh>

static constexpr auto lexing_assets = std::to_array<std::string_view>({
	"test_assets/lex",
});

static constexpr auto parsing_assets = std::to_array<std::string_view>({
	"test_assets/parse",
});

static constexpr auto binding_assets = std::to_array<std::string_view>({
	"test_assets/bind",
});

static constexpr auto typing_assets = std::to_array<std::string_view>({
	"test_assets/type",
});

static constexpr auto eval_assets = std::to_array<std::string_view>({
	"test_assets/eval",
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
			parse::Parser parser{ em };
			ast::SharedAst ast = parser.parse(file.path().string());
			if (fail_on == FailOn::Lexing)
			{
				if (!(em.type() & utils::ErrorType::Lexing))
				{
					std::cerr << em;
					std::cerr << "lexing - should have failed but didn't: " << file << std::endl;
					code |= vat::utils::ErrorManager::Lexing;
				}
				else
					std::cout << "lexing - success: " << file << std::endl;
				continue;
			}
			if (em.type() & utils::ErrorType::Lexing)
			{
				std::cerr << em;
				std::cerr << "lexing - should have succeded but didn't: " << file << std::endl;
				code |= vat::utils::ErrorManager::Lexing;
				continue;
			}
			std::cout << "lexing - success: " << file << std::endl;
			if (success_until == SuccessUntil::Lexing) continue;
			if (fail_on == FailOn::Parsing)
			{
				if (!(em.type() & utils::ErrorType::Parsing))
				{
					std::cerr << em;
					std::cerr << "parsing - should have failed but didn't: " << file << std::endl;
					code |= vat::utils::ErrorManager::Parsing;
				}
				else
					std::cout << "parsing - success: " << file << std::endl;
				continue;
			}
			if (em.type() & utils::ErrorType::Parsing)
			{
				std::cerr << em;
				std::cerr << "parsing - should have succeded but didn't: " << file << std::endl;
				code |= vat::utils::ErrorManager::Parsing;
				continue;
			}
			std::cout << "parsing - success: " << file << std::endl;
			if (success_until == SuccessUntil::Parsing) continue;
			bind::Binder binder{ em };
			binder.bind(*ast);
			if (fail_on == FailOn::Binding)
			{
				if (!em)
				{
					std::cerr << "binding - should have failed but didn't: " << file << std::endl;
					code |= vat::utils::ErrorManager::Binding;
				}
				else
					std::cout << "binding - success: " << file << std::endl;
				continue;
			}
			if (em)
			{
				std::cerr << em;
				std::cerr << "binding - should have succeded but didn't: " << file << std::endl;
				code |= vat::utils::ErrorManager::Binding;
				continue;
			}
			std::cout << "binding - success: " << file << std::endl;
			if (success_until == SuccessUntil::Binding) continue;
			eval::AstEvaluator evaluator{ em };
			eval::ast_exp::exp_type exp = evaluator.eval(*ast);
			if (fail_on == FailOn::Typing || fail_on == FailOn::Evaluation)
			{
				if (!em)
				{
					std::cerr << "evaluation - should have failed but didn't: " << file << std::endl;
					code |= vat::utils::ErrorManager::Evaluation;
				}
				else
					std::cout << "evaluation - success: " << file << std::endl;
				continue;
			}
			if (em)
			{
				std::cerr << em;
				std::cerr << "evaluation - should have succeded but didn't: " << file << std::endl;
				code |= vat::utils::ErrorManager::Evaluation;
				continue;
			}
			std::cout << "evaluation - success: " << file << std::endl;
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
