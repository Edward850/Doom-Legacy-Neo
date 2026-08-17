#include "../crypto-algorithms/sha256.h"

static SHA256_CTX levelhash;
BYTE levelhashsum[SHA256_BLOCK_SIZE];

void P_HashLevelInit(void)
{
	sha256_init(&levelhash);
}

void P_HashLevelData(void* data, size_t len)
{
	sha256_update(&levelhash, (BYTE*)data, len);
}

void P_HashLevelFinalize(void)
{
	sha256_final(&levelhash, levelhashsum);
}
