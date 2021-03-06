#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

bool u_method_0(int den, int sh_post, int bits) {
    int max = (1 << bits) - 1;
    for (int num = 0; num <= max; num++) {	
	uint64_t result = num;
	result >>= sh_post;
	if (num / den != result) return false;
    }
    return true;
}

bool u_method_1(int den, int mul, int sh_post, int bits) {
    int max = (1 << bits) - 1;
    for (int num = 0; num <= max; num++) {
	uint64_t result = num;
	result *= mul;
	result >>= bits;
	if (result > max) return false;
	result >>= sh_post;
	if (num / den != result) return false;
    }
    return true;
}

bool u_method_2(int den, int mul, int sh_post, int bits) {
    int max = (1 << bits) - 1;
    for (int num = 0; num <= max; num++) {
	uint64_t result = num;
	result *= mul;
	result >>= bits;
	if (result > max) return false;
	result += (num - result)/2;
	if (result > max) return false;
	result >>= sh_post;
	if (num / den != result) return false;
    }
    return true;    
}


bool s_method_0(int den, int mul, int sh_post, int bits) {
    int min = -(1 << (bits-1)), max = (1 << (bits-1))-1;
    for (int num = min; num <= max; num++) {
	int64_t result = num;
	result *= mul;
	result >>= bits;
	if (result > max || result < min) return false;
	result >>= sh_post;
	result += (num < 0 ? 1 : 0);
	if (num / den != result) return false;
    }
    return true;
}

bool s_method_1(int den, int mul, int sh_post, int bits) {
    int min = -(1 << (bits-1)), max = (1 << (bits-1))-1;
    for (int num = min; num <= max; num++) {
	int64_t result = num;
	result *= mul;
	result >>= bits;
	if (result > max || result < min) return false;
	result += num;
	if (result > max || result < min) return false;
	result >>= sh_post;
	result += (num < 0 ? 1 : 0);
	if (num / den != result) return false;
    }
    return true;    
}


int main(int argc, char **argv) {
    /* This program computes a table to help us do cheap integer
        division by a constant. It is based on the paper "Division by
        Invariant Integers using Multiplication" by Granlund and
        Montgomery.
    */

    printf("(* Generated by util/find_inverse.c *)\n");

    for (int bits = 8; bits <= 16; bits += 8) {
	int s_min = -(1 << (bits-1));
	int s_max = (1 << (bits-1))-1;
	int u_max = (1 << bits)-1;
   	
	printf("let table_u%d = [|\n", bits);
	for (int den = 2; den <= 64; den++) {
	    for (int shift = 0; shift < 8; shift++) {
		if (u_method_0(den, shift, bits)) {
		    printf("  (0, 0, %d);\n", shift);
		    goto next_unsigned;
		}
	    }
	    
	    for (int shift = 0; shift < 8; shift++) {
		for (int mul = 0; mul <= u_max; mul++) {
		    if (u_method_1(den, mul, shift, bits)) {
			printf("  (1, %d, %d);\n", mul, shift);
			goto next_unsigned;
		    }
		}
	    }
	    
	    for (int shift = 0; shift < 8; shift++) {
		for (int mul = 0; mul <= u_max; mul++) {
		    if (u_method_2(den, mul, shift, bits)) {
			printf("  (2, %d, %d);\n", mul, shift);
			goto next_unsigned;
		    }
		}
	    }
	    printf("ERROR! No solution found for unsigned %d\n", den);
	  next_unsigned:;
	}
	printf("|]\n");
	printf("let table_s%d = [|\n", bits);
	for (int den = 2; den <= 64; den++) {

	    for (int shift = 0; shift < 8; shift++) {
		for (int mul = s_min; mul <= s_max; mul++) {
		    if (s_method_0(den, mul, shift, bits)) {
			printf("  (0, %d, %d);\n", mul, shift);
			goto next_signed;
		    }
		}
	    }
	    
	    for (int shift = 0; shift < 8; shift++) {
		for (int mul = s_min; mul <= s_max; mul++) {
		    if (s_method_1(den, mul, shift, bits)) {
			printf("  (1, %d, %d);\n", mul, shift);
			goto next_signed;
		    }
		}
	    }
	    printf("ERROR! No solution found for signed %d\n", den);
	  next_signed:;
	}
	printf("|]\n");
    }


    return 0;
}
