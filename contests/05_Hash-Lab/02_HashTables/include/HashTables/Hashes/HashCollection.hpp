/*============================================================================*/
#ifndef HASHTABLES_RESEARCH_HASH_COLLECTION_HPP
#define HASHTABLES_RESEARCH_HASH_COLLECTION_HPP
/*============================================================================*/
#include <string_view>

#include "IHash.hpp"
/*============================================================================*/
namespace Research::Hashes {

    using String = std::string_view;
    using Number = uint64_t;

    /* for strings */
    /*------------------------------------------------------------------------*/

    class Md5Hash : public IHash<String> {

        uint64_t operator()(const String& key) override;

    };

    class Sha256Hash : public IHash<String> {

        uint64_t operator()(const String& key) override;

    };

    class DumbHash : public IHash<String> {
    public:

        static constexpr uint64_t MOD = 63018038201L;

        uint64_t operator()(const String& key) override;

    };

    class RabinKarpHash : public IHash<String> {
    public:

        uint64_t operator()(const String& key) override;


    };

    class Murmur3Hash : public IHash<String> {

        uint64_t operator()(const String& key) override;

    };

    /* for numbers */
    /*------------------------------------------------------------------------*/

    class TabulationHash : public IHash<Number> {
    public:

        uint64_t operator()(const Number& key) override;

    };

    class PolynomialHash : public IHash<uint64_t> {
    public:

        uint64_t operator()(const uint64_t& key) override;

    };

    class Md5NumericalHash : public IHash<Number> {

        uint64_t operator()(const Number& key) override;

    };

    class Sha256NumericalHash : public IHash<Number> {

        uint64_t operator()(const Number& key) override;

    };

    class Murmur3NumericalHash : public IHash<Number> {

        uint64_t operator()(const Number& key) override;

    };

}
/*============================================================================*/
#include "DumbHash.ipp"


/*============================================================================*/
#endif //HASHTABLES_RESEARCH_HASH_COLLECTION_HPP
/*============================================================================*/
