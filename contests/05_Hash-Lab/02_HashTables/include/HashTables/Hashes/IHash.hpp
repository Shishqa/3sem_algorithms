/*============================================================================*/
#ifndef HASHTABLES_RESEARCH_IHASH_HPP
#define HASHTABLES_RESEARCH_IHASH_HPP
/*============================================================================*/
#include <cstdint>
/*============================================================================*/
namespace Research::Hashes {

    template <typename T>
    class IHash {

        virtual uint64_t operator()(const T& key) = 0;

    };

}
/*============================================================================*/
#endif //HASHTABLES_RESEARCH_IHASH_HPP
/*============================================================================*/