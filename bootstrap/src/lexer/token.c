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

bool token_ctor_num(struct token* token, struct loc const* loc, uint64_t u64)
{
	if (!token_of_type(token, loc, TOKEN_NUM)) return false;
	token->val.u64 = u64;
	return true;
}

void token_dtor(struct token* token)
{
	switch (token->type)
	{
	case TOKEN_WORD: str_dtor(&token->val.str); break;
	case TOKEN_NEWLINE: FALLTHROUGH;
	case TOKEN_SEMICOLUMN: FALLTHROUGH;
	case TOKEN_LPAREN: FALLTHROUGH;
	case TOKEN_RPAREN: FALLTHROUGH;
	case TOKEN_PLUS: FALLTHROUGH;
	case TOKEN_MINUS: FALLTHROUGH;
	case TOKEN_NUM: FALLTHROUGH;
	case TOKEN_EOF: break;
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
	case TOKEN_EOF: fputs("<EOF>", stream); break;
	case TOKEN_NEWLINE: fputs("<NEWLINE>", stream); break;
	case TOKEN_SEMICOLUMN: fputc(';', stream); break;
	case TOKEN_LPAREN: fputc('(', stream); break;
	case TOKEN_RPAREN: fputc(')', stream); break;
	case TOKEN_PLUS: fputc('+', stream); break;
	case TOKEN_MINUS: fputc('-', stream); break;
	case TOKEN_WORD: cv_print(cv_str(&token->val.str), stream); break;
	case TOKEN_NUM: fprintf(stream, "%" PRIu64, token->val.u64); break;
	};
}
