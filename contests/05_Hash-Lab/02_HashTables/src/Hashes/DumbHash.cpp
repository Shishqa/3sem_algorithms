/*============================================================================*/
#include "HashCollection.hpp"
/*============================================================================*/
using namespace Research::Hashes;
/*============================================================================*/

uint64_t DumbHash::operator()(const String& key) {

    uint64_t hash = 0;
    for (auto& ch : key) {
        hash = (hash + static_cast<uint64_t>(ch)) % MOD;
    }

    return hash;
}