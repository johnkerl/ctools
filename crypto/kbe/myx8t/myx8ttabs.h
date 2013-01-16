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

#ifndef MYX8TTABS_H
#define MYX8TTABS_H

// Exp table for generator g = 0x02, reductor 0x1c3:
// Contains g^n, indexed by n.
// Contents are duplicated for ease of indexing.

static unsigned char myx8t_exp_table[] = {
	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
	0xc3, 0x45, 0x8a, 0xd7, 0x6d, 0xda, 0x77, 0xee,
	0x1f, 0x3e, 0x7c, 0xf8, 0x33, 0x66, 0xcc, 0x5b,
	0xb6, 0xaf, 0x9d, 0xf9, 0x31, 0x62, 0xc4, 0x4b,
	0x96, 0xef, 0x1d, 0x3a, 0x74, 0xe8, 0x13, 0x26,
	0x4c, 0x98, 0xf3, 0x25, 0x4a, 0x94, 0xeb, 0x15,
	0x2a, 0x54, 0xa8, 0x93, 0xe5, 0x09, 0x12, 0x24,
	0x48, 0x90, 0xe3, 0x05, 0x0a, 0x14, 0x28, 0x50,
	0xa0, 0x83, 0xc5, 0x49, 0x92, 0xe7, 0x0d, 0x1a,
	0x34, 0x68, 0xd0, 0x63, 0xc6, 0x4f, 0x9e, 0xff,
	0x3d, 0x7a, 0xf4, 0x2b, 0x56, 0xac, 0x9b, 0xf5,
	0x29, 0x52, 0xa4, 0x8b, 0xd5, 0x69, 0xd2, 0x67,
	0xce, 0x5f, 0xbe, 0xbf, 0xbd, 0xb9, 0xb1, 0xa1,
	0x81, 0xc1, 0x41, 0x82, 0xc7, 0x4d, 0x9a, 0xf7,
	0x2d, 0x5a, 0xb4, 0xab, 0x95, 0xe9, 0x11, 0x22,
	0x44, 0x88, 0xd3, 0x65, 0xca, 0x57, 0xae, 0x9f,
	0xfd, 0x39, 0x72, 0xe4, 0x0b, 0x16, 0x2c, 0x58,
	0xb0, 0xa3, 0x85, 0xc9, 0x51, 0xa2, 0x87, 0xcd,
	0x59, 0xb2, 0xa7, 0x8d, 0xd9, 0x71, 0xe2, 0x07,
	0x0e, 0x1c, 0x38, 0x70, 0xe0, 0x03, 0x06, 0x0c,
	0x18, 0x30, 0x60, 0xc0, 0x43, 0x86, 0xcf, 0x5d,
	0xba, 0xb7, 0xad, 0x99, 0xf1, 0x21, 0x42, 0x84,
	0xcb, 0x55, 0xaa, 0x97, 0xed, 0x19, 0x32, 0x64,
	0xc8, 0x53, 0xa6, 0x8f, 0xdd, 0x79, 0xf2, 0x27,
	0x4e, 0x9c, 0xfb, 0x35, 0x6a, 0xd4, 0x6b, 0xd6,
	0x6f, 0xde, 0x7f, 0xfe, 0x3f, 0x7e, 0xfc, 0x3b,
	0x76, 0xec, 0x1b, 0x36, 0x6c, 0xd8, 0x73, 0xe6,
	0x0f, 0x1e, 0x3c, 0x78, 0xf0, 0x23, 0x46, 0x8c,
	0xdb, 0x75, 0xea, 0x17, 0x2e, 0x5c, 0xb8, 0xb3,
	0xa5, 0x89, 0xd1, 0x61, 0xc2, 0x47, 0x8e, 0xdf,
	0x7d, 0xfa, 0x37, 0x6e, 0xdc, 0x7b, 0xf6, 0x2f,
	0x5e, 0xbc, 0xbb, 0xb5, 0xa9, 0x91, 0xe1, 

	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
	0xc3, 0x45, 0x8a, 0xd7, 0x6d, 0xda, 0x77, 0xee,
	0x1f, 0x3e, 0x7c, 0xf8, 0x33, 0x66, 0xcc, 0x5b,
	0xb6, 0xaf, 0x9d, 0xf9, 0x31, 0x62, 0xc4, 0x4b,
	0x96, 0xef, 0x1d, 0x3a, 0x74, 0xe8, 0x13, 0x26,
	0x4c, 0x98, 0xf3, 0x25, 0x4a, 0x94, 0xeb, 0x15,
	0x2a, 0x54, 0xa8, 0x93, 0xe5, 0x09, 0x12, 0x24,
	0x48, 0x90, 0xe3, 0x05, 0x0a, 0x14, 0x28, 0x50,
	0xa0, 0x83, 0xc5, 0x49, 0x92, 0xe7, 0x0d, 0x1a,
	0x34, 0x68, 0xd0, 0x63, 0xc6, 0x4f, 0x9e, 0xff,
	0x3d, 0x7a, 0xf4, 0x2b, 0x56, 0xac, 0x9b, 0xf5,
	0x29, 0x52, 0xa4, 0x8b, 0xd5, 0x69, 0xd2, 0x67,
	0xce, 0x5f, 0xbe, 0xbf, 0xbd, 0xb9, 0xb1, 0xa1,
	0x81, 0xc1, 0x41, 0x82, 0xc7, 0x4d, 0x9a, 0xf7,
	0x2d, 0x5a, 0xb4, 0xab, 0x95, 0xe9, 0x11, 0x22,
	0x44, 0x88, 0xd3, 0x65, 0xca, 0x57, 0xae, 0x9f,
	0xfd, 0x39, 0x72, 0xe4, 0x0b, 0x16, 0x2c, 0x58,
	0xb0, 0xa3, 0x85, 0xc9, 0x51, 0xa2, 0x87, 0xcd,
	0x59, 0xb2, 0xa7, 0x8d, 0xd9, 0x71, 0xe2, 0x07,
	0x0e, 0x1c, 0x38, 0x70, 0xe0, 0x03, 0x06, 0x0c,
	0x18, 0x30, 0x60, 0xc0, 0x43, 0x86, 0xcf, 0x5d,
	0xba, 0xb7, 0xad, 0x99, 0xf1, 0x21, 0x42, 0x84,
	0xcb, 0x55, 0xaa, 0x97, 0xed, 0x19, 0x32, 0x64,
	0xc8, 0x53, 0xa6, 0x8f, 0xdd, 0x79, 0xf2, 0x27,
	0x4e, 0x9c, 0xfb, 0x35, 0x6a, 0xd4, 0x6b, 0xd6,
	0x6f, 0xde, 0x7f, 0xfe, 0x3f, 0x7e, 0xfc, 0x3b,
	0x76, 0xec, 0x1b, 0x36, 0x6c, 0xd8, 0x73, 0xe6,
	0x0f, 0x1e, 0x3c, 0x78, 0xf0, 0x23, 0x46, 0x8c,
	0xdb, 0x75, 0xea, 0x17, 0x2e, 0x5c, 0xb8, 0xb3,
	0xa5, 0x89, 0xd1, 0x61, 0xc2, 0x47, 0x8e, 0xdf,
	0x7d, 0xfa, 0x37, 0x6e, 0xdc, 0x7b, 0xf6, 0x2f,
	0x5e, 0xbc, 0xbb, 0xb5, 0xa9, 0x91, 0xe1, 
};

// Log table for generator g = 0x02, reductor 0x1c3:
// Contains log_g(x), indexed by x (treated as an
// integer rather than as a polynomial).

static unsigned char myx8t_log_table[] = {
	0xff,
	// Dummy entry, to make indexing convenient.
	// Log of zero of course doesn't exist.
	      0x00, 0x01, 0x9d, 0x02, 0x3b, 0x9e, 0x97,
	0x03, 0x35, 0x3c, 0x84, 0x9f, 0x46, 0x98, 0xd8,
	0x04, 0x76, 0x36, 0x26, 0x3d, 0x2f, 0x85, 0xe3,
	0xa0, 0xb5, 0x47, 0xd2, 0x99, 0x22, 0xd9, 0x10,
	0x05, 0xad, 0x77, 0xdd, 0x37, 0x2b, 0x27, 0xbf,
	0x3e, 0x58, 0x30, 0x53, 0x86, 0x70, 0xe4, 0xf7,
	0xa1, 0x1c, 0xb6, 0x14, 0x48, 0xc3, 0xd3, 0xf2,
	0x9a, 0x81, 0x23, 0xcf, 0xda, 0x50, 0x11, 0xcc,
	0x06, 0x6a, 0xae, 0xa4, 0x78, 0x09, 0xde, 0xed,
	0x38, 0x43, 0x2c, 0x1f, 0x28, 0x6d, 0xc0, 0x4d,
	0x3f, 0x8c, 0x59, 0xb9, 0x31, 0xb1, 0x54, 0x7d,
	0x87, 0x90, 0x71, 0x17, 0xe5, 0xa7, 0xf8, 0x61,
	0xa2, 0xeb, 0x1d, 0x4b, 0xb7, 0x7b, 0x15, 0x5f,
	0x49, 0x5d, 0xc4, 0xc6, 0xd4, 0x0c, 0xf3, 0xc8,
	0x9b, 0x95, 0x82, 0xd6, 0x24, 0xe1, 0xd0, 0x0e,
	0xdb, 0xbd, 0x51, 0xf5, 0x12, 0xf0, 0xcd, 0xca,
	0x07, 0x68, 0x6b, 0x41, 0xaf, 0x8a, 0xa5, 0x8e,
	0x79, 0xe9, 0x0a, 0x5b, 0xdf, 0x93, 0xee, 0xbb,
	0x39, 0xfd, 0x44, 0x33, 0x2d, 0x74, 0x20, 0xb3,
	0x29, 0xab, 0x6e, 0x56, 0xc1, 0x1a, 0x4e, 0x7f,
	0x40, 0x67, 0x8d, 0x89, 0x5a, 0xe8, 0xba, 0x92,
	0x32, 0xfc, 0xb2, 0x73, 0x55, 0xaa, 0x7e, 0x19,
	0x88, 0x66, 0x91, 0xe7, 0x72, 0xfb, 0x18, 0xa9,
	0xe6, 0x65, 0xa8, 0xfa, 0xf9, 0x64, 0x62, 0x63,
	0xa3, 0x69, 0xec, 0x08, 0x1e, 0x42, 0x4c, 0x6c,
	0xb8, 0x8b, 0x7c, 0xb0, 0x16, 0x8f, 0x60, 0xa6,
	0x4a, 0xea, 0x5e, 0x7a, 0xc5, 0x5c, 0xc7, 0x0b,
	0xd5, 0x94, 0x0d, 0xe0, 0xf4, 0xbc, 0xc9, 0xef,
	0x9c, 0xfe, 0x96, 0x3a, 0x83, 0x34, 0xd7, 0x45,
	0x25, 0x75, 0xe2, 0x2e, 0xd1, 0xb4, 0x0f, 0x21,
	0xdc, 0xac, 0xbe, 0x2a, 0x52, 0x57, 0xf6, 0x6f,
	0x13, 0x1b, 0xf1, 0xc2, 0xce, 0x80, 0xcb, 0x4f,
};

#endif // MYX8TTABS_H
