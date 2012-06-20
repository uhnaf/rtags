#include "SHA256.h"
#include <openssl/sha.h>
#include <stdio.h>

class SHA256Private
{
public:
    SHA256_CTX ctx;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    bool finalized;
};

SHA256::SHA256()
    : priv(new SHA256Private)
{
    reset();
}

SHA256::~SHA256()
{
    delete priv;
}

void SHA256::update(const char *data, uint size)
{
    if (priv->finalized)
        priv->finalized = false;
    SHA256_Update(&priv->ctx, data, size);
}

void SHA256::update(const ByteArray &data)
{
    if (priv->finalized)
        priv->finalized = false;
    SHA256_Update(&priv->ctx, data.constData(), data.size());
}

void SHA256::reset()
{
    priv->finalized = false;
    SHA256_Init(&priv->ctx);
}

static const char* const hexLookup = "0123456789abcdef";

static inline ByteArray hashToHex(SHA256Private* priv)
{
    ByteArray out(SHA256_DIGEST_LENGTH * 2, '\0');
    const unsigned char* get = priv->hash;
    char* put = out.data();
    const char* const end = out.data() + out.size();;
    for (; put != end; ++get) {
        *(put++) = hexLookup[(*get >> 4) & 0xf];
        *(put++) = hexLookup[*get & 0xf];
    }
    return out;
}

ByteArray SHA256::hash(MapType type) const
{
    if (!priv->finalized) {
        SHA256_Final(priv->hash, &priv->ctx);
        SHA256_Init(&priv->ctx);
        priv->finalized = true;
    }
    if (type == Hex)
        return hashToHex(priv);
    return ByteArray(reinterpret_cast<char*>(priv->hash), SHA256_DIGEST_LENGTH);
}

ByteArray SHA256::hash(const ByteArray& data, MapType type)
{
    return SHA256::hash(data.constData(), data.size(), type);
}

ByteArray SHA256::hash(const char* data, uint size, MapType type)
{
    SHA256Private priv;
    SHA256_Init(&priv.ctx);
    SHA256_Update(&priv.ctx, data, size);
    SHA256_Final(priv.hash, &priv.ctx);
    if (type == Hex)
        return hashToHex(&priv);
    return ByteArray(reinterpret_cast<char*>(priv.hash), SHA256_DIGEST_LENGTH);
}
