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

#ifndef BLKCIPHT_H
#define BLKCIPHT_H

#define  KBE_MAX_KL    64
#define  KBE_MAX_BL    64

typedef struct _block_cipher_instance_t {
	int                        bl;
	int                        kl;
	char                       user_IV[KBE_MAX_BL];
	int                        have_user_IV;
	char                       user_key_mat[KBE_MAX_KL];
	int                        have_user_key_mat;
	void                     * pkey_schedule;
	int                        do_cbc;
	int                        alg_debug;
} block_cipher_instance_t;

typedef int block_cipher_encryptor_t(
	char * bytes_in,
	char * bytes_out,
	block_cipher_instance_t * pciphinst);

typedef int block_cipher_decryptor_t(
	char * bytes_in,
	char * bytes_out,
	block_cipher_instance_t * pciphinst);

typedef int block_cipher_key_maker_t(
	char  * key_material,
	block_cipher_instance_t * pciphinst);

typedef void block_cipher_key_freer_t(
	char  * key_material,
	block_cipher_instance_t * pciphinst);

typedef int block_cipher_bl_checker_t(
	int  bl);

typedef int block_cipher_kl_checker_t(
	int  kl);

typedef char * block_cipher_error_explainer_t(
	int    error_code);

typedef struct _block_cipher_t {
	char                      * cipher_name;
	int                         default_bl;
	int                         default_kl;
	block_cipher_encryptor_t  * pencryptor;
	block_cipher_decryptor_t  * pdecryptor;
	block_cipher_key_maker_t  * pkey_maker;
	block_cipher_key_freer_t  * pkey_freer;
	block_cipher_bl_checker_t * pbl_checker;
	block_cipher_kl_checker_t * pkl_checker;
	block_cipher_error_explainer_t * perror_explainer;
} block_cipher_t;

#endif // BLKCIPHT_H
