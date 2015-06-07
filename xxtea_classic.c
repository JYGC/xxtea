#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define DELTA 0x9e3779b9
#define MX (((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4)) ^ ((sum ^ y) + (key[(p & 3) ^ e] ^ z)))


void btea(uint32_t *, int, uint32_t const key[4]);
void pack(uint64_t *, uint32_t, uint32_t);
void unpack(uint64_t , uint32_t *, uint32_t *);


int main(int argc, char *argv[])
{
	uint32_t key[4] = {0x342d3221, 0x4320fa22, 0x46257a42, 0x9002bf22};
	uint64_t b64;
	uint32_t b64_split[2];

	if (strcmp(argv[1], "-e") == 0) {
		while((b64 = getc(stdin)) != EOF) {
			unpack(b64, &b64_split[0], &b64_split[1]);
			btea(b64_split, 2, key);
			pack(&b64, b64_split[0], b64_split[1]);
			fwrite(&b64, sizeof(uint64_t), 1, stdout);
		}
		b64 = 0x00000000ffffffff;
		fwrite(&b64, sizeof(uint64_t), 1, stdout);
	} else if (strcmp(argv[1], "-d") == 0) {
		while (1) {
			fread(&b64, sizeof(uint64_t), 1, stdin);
			if (b64 == 0x00000000ffffffff)
				break;
			unpack(b64, &b64_split[0], &b64_split[1]);
			btea(b64_split, -2, key);
			pack(&b64, b64_split[0], b64_split[1]);
			putc(b64, stdout);
		}
	}

	return 0;
}


void btea(uint32_t *v, int n, uint32_t const key[4])
{
	uint32_t y, z, sum;
	unsigned p, rounds, e;
	if(n > 1) {				/* Encrypt */
		rounds = 6 + 52/n;
		sum = 0;
		z = v[n - 1];
		do {
			sum += DELTA;
			e = (sum >> 2) & 3;
			for (p = 0; p < n - 1; p++) {
				y = v[p + 1];
				z = v[p] += MX;
			}
			y = v[0];
			z = v[n - 1] += MX;
		} while (--rounds);
	} else if (n < -1) {	/* Decrypt */
		n = -n;
		rounds = 6 + 52/n;
		sum = rounds * DELTA;
		y = v[0];
		do {
			e = (sum >> 2) & 3;
			for (p = n - 1; p > 0; p--) {
				z = v[p - 1];
				y = v[p] -= MX;
			}
			z = v[n - 1];
			y = v[0] -= MX;
			sum -= DELTA;
		} while (--rounds);
	}
}


void pack(uint64_t *b64, uint32_t b32_0, uint32_t b32_1)
{
	*b64 = ((uint64_t) b32_0) << 32 | b32_1;
}


void unpack(uint64_t b64, uint32_t *b32_0, uint32_t *b32_1)
{
	*b32_0 = (uint32_t)((b64 & 0xFFFFFFFF00000000) >> 32);
	*b32_1 = (uint32_t)(b64 & 0xFFFFFFFF);
}
