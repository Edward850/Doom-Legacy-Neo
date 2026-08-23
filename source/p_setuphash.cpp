
#include "p_setuphash.h"

// Need to be included as C headers
extern "C" {
#include "../crypto-algorithms/sha256.h"
#include "../crypto-algorithms/md5.h"
}

static SHA256_CTX* levelhash;
static MD5_CTX* md5Hash;
BYTE levelhashsum[SHA256_BLOCK_SIZE];

void P_HashLevelInit(void)
{
	levelhash = new SHA256_CTX;
	sha256_init(levelhash);
}

void P_HashLevelData(void* data, size_t len)
{
	sha256_update(levelhash, (BYTE*)data, len);
}

void P_HashLevelFinalize(void)
{
	sha256_final(levelhash, levelhashsum);
	delete levelhash;
	levelhash = nullptr;
}

void P_HashMD5Init(void)
{
	md5Hash = new MD5_CTX;
	md5_init(md5Hash);
}

void P_HashMD5Data(void* data, size_t len)
{
	md5_update(md5Hash, (BYTE*)data, len);
}

void P_HashMD5Finalize(unsigned char* data)
{
	md5_final(md5Hash, data);
	delete md5Hash;
	md5Hash = nullptr;
}

