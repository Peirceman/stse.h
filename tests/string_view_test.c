#define STSE_IMPLEMENTATION
#define STSE_OMIT_NS
#include "../stse.h"
#include <stdio.h>

int main(void) {
	char *cstr = "  \n   \tsome text   \n  ";

	String_View sv = sv_from_cstr(cstr);
	printf("sv: `"SV_FMT"`\n", SV_ARG(sv));

	String_View trimmed = sv_trim(sv);
	printf("trimmed: `"SV_FMT"`\n", SV_ARG(trimmed));

	const char *trimmed_cstr = sv_to_tmp_cstr(trimmed);
	printf("trimmed_cstr = `%s`\n", trimmed_cstr);

	String_View trimmed_space = sv_trim_delim(sv, ' ');
	printf("trimmed_space: `"SV_FMT"`\n", SV_ARG(trimmed_space));

	String_View trimmed_space_nl = sv_trim_delim_cstr(sv, " \n");
	printf("trimmed_space_nl: `"SV_FMT"`\n", SV_ARG(trimmed_space_nl));

	cstr = "precentresuf";
	sv = sv_from_cstr(cstr);
	printf("sv: `"SV_FMT"`\n", SV_ARG(sv));
	

	String_View prefix = sv_from_cstr("pre");
	String_View suffix = sv_from_cstr("suf");
	bool ok = sv_tok_start(&sv, prefix);
	ok = ok && sv_tok_end(&sv, suffix);

	printf("sv: `"SV_FMT"`\n", SV_ARG(sv));
	if (ok) {
		printf("ok\n");
	} else {
		printf("not ok\n");
	}

	sv = sv_from_cstr("A few\twords");
	printf("space char:\n");
	for (String_View tok = sv_tok_delim(&sv, ' '); tok.len > 0; tok = sv_tok_delim(&sv, ' ')) {
		printf("`"SV_FMT"`\n", SV_ARG(tok));
	}

	sv = sv_from_cstr("A few\twords");
	printf("delims cstr:\n");
	for (String_View tok = sv_tok_delim_cstr(&sv, " \t\r\n"); tok.len > 0; tok = sv_tok_delim_cstr(&sv, " \t\r\n")) {
		printf("`"SV_FMT"`\n", SV_ARG(tok));
	}

	sv = sv_from_cstr("A few\twords");
	printf("allowed_chars cstr:\n");
	const char *allowed_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	for (String_View tok = sv_tok_chars_cstr(&sv, allowed_chars); tok.len > 0; tok = sv_tok_chars_cstr(&sv, allowed_chars)) {
		printf("`"SV_FMT"`\n", SV_ARG(tok));
	}

	sv = sv_from_cstr("A few\twords");
	printf("pred (isspace):\n");
	for (String_View tok = sv_tok(&sv); tok.len > 0; tok = sv_tok(&sv)) {
		printf("`"SV_FMT"`\n", SV_ARG(tok));
	}

	printf("done!\n");

	return 0;
}
