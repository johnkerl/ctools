// ================================================================
// Copyright (c) 2004 John Kerl.
// kerl.john.r@gmail.com
//
// This code and information is provided as is without warranty of
// any kind, either expressed or implied, including but not limited to
// the implied warranties of merchantability and/or fitness for a
// particular purpose.
//
// No restrictions are placed on copy or reuse of this code, as long
// as these paragraphs are included in the code.
// ================================================================
//
// kbe (Kerl's block encryptor) is a simple little C program to encrypt and
// unencrypt files using various algorithms, coupled with cipher-block chaining.
// There are already various programs for this.  Mainly, writing this was just
// good clean fun, and a way to teach myself various concepts I was
// curious about.
// ================================================================

// ----------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>

#include "util.h"
#include "blkciphs.h"

// ----------------------------------------------------------------
static void usage(
	char      * argv0);

static void set_defaults(
	opts_t    * popts);

static int parse_command_line(
	int         argc,
	char     ** argv,
	opts_t    * popts);

static int get_arg_int(
	char ** argv,
	int     argi,
	int     argc,
	int   * pval,
	char  * desc);

// ================================================================
int main(int argc, char **argv)
{
	opts_t opts;
	int    argi;
	int    rv = 0;
	int    make_key_rc;

	set_defaults(&opts);
	if (!parse_command_line(argc, argv, &opts))
		usage(argv[0]);

	if (opts.operation == OP_ENCRYPT || opts.operation == OP_DECRYPT) {
		make_key_rc = opts.pcipher->pkey_maker(opts.key_text,
		&opts.ciphinst);
		if (make_key_rc != 1) {
			fprintf(stderr, "%s:  %s while scheduling key.\n",
				opts.pcipher->cipher_name,
				opts.pcipher->perror_explainer(make_key_rc));
			exit(1);
		}
	}

	if (opts.first_file_argi == argc) {
#ifdef WINDOWS
		setmode(fileno(stdin),  O_BINARY);
		setmode(fileno(stdout), O_BINARY);
#endif // WINDOWS
		if (!bc_handle_fp_pair(stdin, stdout, "stdin", "stdout", &opts))
			rv++;
	}

	for (argi = opts.first_file_argi; argi < argc; argi++) {
		if (!bc_handle_file_name(argv[argi], &opts))
			rv++;
	}

	opts.pcipher->pkey_freer(opts.key_text, &opts.ciphinst);

	return rv;
}

// ----------------------------------------------------------------
static void usage(char *argv0)
{
	fprintf(stderr,
		"  Usage: %s [options] {verb} {file name(s) ... }\n",
		argv0);
	fprintf(stderr, "  Options:\n");
	fprintf(stderr, "    -kl {n}:  Key   length in bytes.\n");
	fprintf(stderr, "    -bl {n}:  Block length in bytes.\n");
	fprintf(stderr, "  Verb:\n");
	fprintf(stderr, "    +{key text} for encrypt\n");
	fprintf(stderr, "    -{key text} for decrypt\n");
	fprintf(stderr,
	"    check       to see if file was encrypted with this program\n");
	exit(1);
}

// ----------------------------------------------------------------
static void set_defaults(
	opts_t * popts)
{
	memset(popts, 0, sizeof(*popts));
	popts->key_text = 0;
	popts->operation = OP_UNDEF;
	popts->first_file_argi = 1;
	block_cipher_select_default(&popts->pcipher, &popts->ciphinst);
	popts->ciphinst.do_cbc = 1;
	memset(popts->ciphinst.user_IV, 0, sizeof(popts->ciphinst.user_IV));
	popts->ciphinst.have_user_IV = 0;

	popts->app_debug = 0;
	popts->ciphinst.alg_debug = 0;
	popts->do_crc  = 1;
	popts->do_spin = 1;
}

// ----------------------------------------------------------------
static int parse_command_line(
	int      argc,
	char  ** argv,
	opts_t * popts)
{
	int argi;
	int bl = -1;
	int kl = -1;
	int dump = 0;
	char * IV_file_name = 0;

	for (argi = 1; argi < argc; argi++) {

		if (strcmp(argv[argi], "-kl") == 0) {
			if (!get_arg_int(argv, argi, argc, &bl,
					"block length"))
				return 0;
			argi++;
		}

		else if (strcmp(argv[argi], "-kl") == 0) {
			if (!get_arg_int(argv, argi, argc, &kl,
					"key length"))
				return 0;
			argi++;
		}

		else if (strcmp(argv[argi], "-bkl") == 0) {
			if (!get_arg_int(argv, argi, argc, &bl,
					"block & key length"))
				return 0;
			kl = bl;
			argi++;
		}

		else if (strcmp(argv[argi], "-nocbc") == 0) {
			popts->ciphinst.do_cbc = 0;
		}

		else if (strcmp(argv[argi], "-ecb") == 0) {
			popts->ciphinst.do_cbc = 0;
		}

		else if (strcmp(argv[argi], "-c") == 0) {
			argi++;
			if (!block_cipher_select(argv[argi],
				&popts->pcipher, &popts->ciphinst))
			{
				fprintf(stderr,
					"Couldn't find cipher \"%s\".\n",
					argv[argi]);
				return 0;
			}
		}

		else if (strcmp(argv[argi], "-iv") == 0) {
			popts->ciphinst.have_user_IV = 1;
			argi++;
			IV_file_name = argv[argi];
		}

		else if (strcmp(argv[argi], "-app") == 0) {
			popts->app_debug++;
		}

		else if (strcmp(argv[argi], "-alg") == 0) {
			popts->ciphinst.alg_debug++;
		}

		else if (strcmp(argv[argi], "-list") == 0) {
			block_cipher_list();
			exit(0);
		}

		else if (strcmp(argv[argi], "-nospin") == 0) {
			popts->do_spin = 0;
		}

		else if (strcmp(argv[argi], "-crc") == 0) {
			popts->do_crc = 1;
		}

		else if (strcmp(argv[argi], "-nocrc") == 0) {
			popts->do_crc = 0;
		}

		else if (strcmp(argv[argi], "-dump") == 0) {
			dump = 1;
		}

		else if (strcmp(argv[argi], "--help") == 0) {
			return 0;
		}

		else {
			break;
		}
	}

	popts->first_file_argi = argi + 1;

	if (bl == -1)
		popts->ciphinst.bl =
			popts->pcipher->default_bl;
	else
		popts->ciphinst.bl = bl;

	if (kl == -1)
		popts->ciphinst.kl =
			popts->pcipher->default_kl;
	else
		popts->ciphinst.kl = kl;



	if ((popts->ciphinst.kl < 1) || (popts->ciphinst.kl > KBE_MAX_KL)) {
		fprintf(stderr,
			"%s:  Key length %d out of range for application.\n",
			argv[0], popts->ciphinst.kl);
		return 0;
	}

	// xxx min is trailer size
	if ((popts->ciphinst.bl < 1) || (popts->ciphinst.bl > KBE_MAX_BL)) {
		fprintf(stderr,
			"%s:  Block length %d out of range for application.\n",
			argv[0], popts->ciphinst.bl);
		return 0;
	}

	if ((popts->ciphinst.kl < 1) || (popts->ciphinst.kl > KBE_MAX_KL)) {
		fprintf(stderr,
			"%s:  Key length %d out of range for application.\n",
			argv[0], popts->ciphinst.kl);
		return 0;
	}

	if ((popts->ciphinst.bl < 1) || (popts->ciphinst.bl > KBE_MAX_BL)) {
		fprintf(stderr,
			"%s:  Block length %d out of range for application.\n",
			argv[0], popts->ciphinst.bl);
		return 0;
	}


	if (popts->ciphinst.have_user_IV) {
		if (!bc_read_block_from_file(IV_file_name,
		popts->ciphinst.user_IV,
		popts->ciphinst.bl))
		{
			// Error message already printed out.
			exit(1);
		}

	}

	if (dump) {
		fprintf(stderr, "Cipher:       %s\n",
			popts->pcipher->cipher_name);
		fprintf(stderr, "Block length: %d\n",
			popts->ciphinst.bl);
		fprintf(stderr, "Key   length: %d\n",
			popts->ciphinst.kl);
		fprintf(stderr, "CBC:          %s\n",
			popts->ciphinst.do_cbc ? "yes" : "no");
		exit(0);
	}

	// argi should now be the index in argv[] of the verb
	// (+key, -key or check).
	if (argi >= argc) {
		fprintf(stderr,
			"%s:  Missing verb (check, +key or -key).\n",
			argv[0]);
		return 0;
	}

	if (strcmp(argv[argi], "check") == 0) {
		popts->operation = OP_CHECK;
	}
	else {
		popts->key_text = &argv[argi][1];
		if (argv[argi][0] == '+')
			popts->operation = OP_ENCRYPT;
		else if (argv[argi][0] == '-')
			popts->operation = OP_DECRYPT;
		else
			return 0;

		if (popts->key_text[0] == '@') {
			if (!bc_read_block_from_file(&popts->key_text[1],
			popts->ciphinst.user_key_mat,
			popts->ciphinst.kl))
				// Error message already printed out.
				exit(1);
			popts->ciphinst.have_user_key_mat = 1;
		}
	}

	if (popts->app_debug || popts->ciphinst.alg_debug)
		popts->do_spin = 0;

	return 1;
}

// ----------------------------------------------------------------
static int get_arg_int(
	char ** argv,
	int     argi,
	int     argc,
	int   * pval,
	char  * desc)
{
	if (argc <= argi + 1) {
		fprintf(stderr, "%s:  Missing argument to %s.\n",
			argv[0], argv[argi]);
		return 0;
	}
	if (sscanf(argv[argi+1], "%d", pval) != 1) {
		fprintf(stderr, "%s: Couldn't scan \"%s\" as %s.\n",
			argv[0], argv[argi+1], desc);
		return 0;
	}

	return 1;
}
