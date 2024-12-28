#include "lexer/token.h"

bool token_of_type(struct token* token, struct loc const* loc, enum token_type type)
{
	token->loc = loc ? *loc : LOC_INVALID;
	token->type = type;
	str_ctor(&token->val.str, 0);
	return true;
}

bool token_ctor_word(struct token* token, struct loc const* loc, struct str* str)
{
	if (!token_of_type(token, loc, TOKEN_WORD)) return false;
	token->val.str = *str;
	return true;
}

bool token_ctor_strlit(struct token* token, struct loc const* loc, struct str* str)
{
	if (!token_of_type(token, loc, TOKEN_STRLIT)) return false;
	token->val.str = *str;
	return true;
}

bool token_ctor_numlit(struct token* token, struct loc const* loc, uint64_t u64)
{
	if (!token_of_type(token, loc, TOKEN_NUMLIT)) return false;
	token->val.u64 = u64;
	return true;
}

void token_dtor(struct token* token)
{
	switch (token->type)
	{
	case TOKEN_ERROR: FALLTHROUGH;
	case TOKEN_NEWLINE: FALLTHROUGH;
	case TOKEN_WHITESPACE: FALLTHROUGH;
	case TOKEN_EOF: FALLTHROUGH;
	case TOKEN_SEMICOLON: FALLTHROUGH;
	case TOKEN_LPAREN: FALLTHROUGH;
	case TOKEN_RPAREN: FALLTHROUGH;
	case TOKEN_PLUS: FALLTHROUGH;
	case TOKEN_MINUS: FALLTHROUGH;
	case TOKEN_EQUAL: FALLTHROUGH;
	case TOKEN_COLON: FALLTHROUGH;
	case TOKEN_COMA: FALLTHROUGH;
	case TOKEN_LCURLBRA: FALLTHROUGH;
	case TOKEN_RCURLBRA: FALLTHROUGH;
	case TOKEN_ARROW: FALLTHROUGH;
	case TOKEN_DOT: FALLTHROUGH;
	case TOKEN_AT: FALLTHROUGH;
	case TOKEN_LET: FALLTHROUGH;
	case TOKEN_FN: FALLTHROUGH;
	case TOKEN_RET: FALLTHROUGH;
	case TOKEN_MODULE: FALLTHROUGH;
	case TOKEN_IMPORT: FALLTHROUGH;
	case TOKEN_AS: FALLTHROUGH;
	case TOKEN_NUMLIT: break;
	case TOKEN_WORD: str_dtor(&token->val.str); break;
	case TOKEN_STRLIT: str_dtor(&token->val.str); break;
	};
}

void token_print(struct token* token, FILE* stream)
{
	if (token->type == TOKEN_NONE)
	{
		fputs("<NOT-A-TOKEN>", stream);
		return;
	}
	switch (token->type)
	{
	case TOKEN_ERROR: fputs("<error>", stream); break;
	case TOKEN_EOF: fputs("<EOF>", stream); break;
	case TOKEN_NEWLINE: fputs("<newline>", stream); break;
	case TOKEN_WHITESPACE: fputs("<whitespace>", stream); break;
	case TOKEN_SEMICOLON: fputc(';', stream); break;
	case TOKEN_LPAREN: fputc('(', stream); break;
	case TOKEN_RPAREN: fputc(')', stream); break;
	case TOKEN_PLUS: fputc('+', stream); break;
	case TOKEN_MINUS: fputc('-', stream); break;
	case TOKEN_EQUAL: fputc('=', stream); break;
	case TOKEN_COLON: fputc(':', stream); break;
	case TOKEN_COMA: fputc(',', stream); break;
	case TOKEN_LCURLBRA: fputc('{', stream); break;
	case TOKEN_RCURLBRA: fputc('}', stream); break;
	case TOKEN_ARROW: fputs("->", stream); break;
	case TOKEN_DOT: fputc('.', stream); break;
	case TOKEN_AT: fputc('@', stream); break;
	case TOKEN_LET: fputs("let", stream); break;
	case TOKEN_FN: fputs("fn", stream); break;
	case TOKEN_RET: fputs("ret", stream); break;
	case TOKEN_MODULE: fputs("module", stream); break;
	case TOKEN_IMPORT: fputs("import", stream); break;
	case TOKEN_AS: fputs("as", stream); break;
	case TOKEN_WORD: cv_print(cv_str(&token->val.str), stream); break;
	case TOKEN_STRLIT:
		fputc('"', stream);
		cv_print(cv_str(&token->val.str), stream);
		fputc('"', stream);
		break;
	case TOKEN_NUMLIT: fprintf(stream, "%" PRIu64, token->val.u64); break;
	};
}

void token_type_print(enum token_type type, FILE* stream)
{
	switch (type)
	{
	case TOKEN_ERROR: fputs("error", stream); break;
	case TOKEN_EOF: fputs("EOF", stream); break;
	case TOKEN_NEWLINE: fputs("newline", stream); break;
	case TOKEN_WHITESPACE: fputs("whitespace", stream); break;
	case TOKEN_SEMICOLON: fputc(';', stream); break;
	case TOKEN_LPAREN: fputc('(', stream); break;
	case TOKEN_RPAREN: fputc(')', stream); break;
	case TOKEN_PLUS: fputc('+', stream); break;
	case TOKEN_MINUS: fputc('-', stream); break;
	case TOKEN_EQUAL: fputc('=', stream); break;
	case TOKEN_COLON: fputc(':', stream); break;
	case TOKEN_COMA: fputc(',', stream); break;
	case TOKEN_LCURLBRA: fputc('{', stream); break;
	case TOKEN_RCURLBRA: fputc('}', stream); break;
	case TOKEN_ARROW: fputs("->", stream); break;
	case TOKEN_DOT: fputc('.', stream); break;
	case TOKEN_AT: fputc('@', stream); break;
	case TOKEN_LET: fputs("let", stream); break;
	case TOKEN_FN: fputs("fn", stream); break;
	case TOKEN_RET: fputs("ret", stream); break;
	case TOKEN_MODULE: fputs("module", stream); break;
	case TOKEN_IMPORT: fputs("import", stream); break;
	case TOKEN_AS: fputs("as", stream); break;
	case TOKEN_WORD: fputs("word", stream); break;
	case TOKEN_STRLIT: fputs("string litteral", stream); break;
	case TOKEN_NUMLIT: fputs("number litteral", stream); break;
	};
}

void token_types_print(enum token_type type, FILE* stream)
{
	bool first = true;
	for (uint64_t u = TOKEN_MIN; u < TOKEN_MAX; u <<= 1)
	{
		if (!(type & u)) continue;
		if (first)
			first = false;
		else
			fputs(", ", stream);
		token_type_print(u, stream);
	}
}
