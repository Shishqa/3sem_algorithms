/*============================================================================*/
#include "IHashSet.hpp"
/*============================================================================*/
namespace HashTables::OpenAddressing {

    template <typename T>
    class IRun {
    public:

        virtual size_t operator()(const T& key, size_t attempt) = 0;

    };

    template <typename T, typename Hash>
    class LinearRun {
    public:

        size_t operator()(const T& key, size_t attempt) override {
            return (hash(key) + attempt);
        }

    private:

        Hash hash;
    };


    template <typename T, typename Run>
    class HashSet : public IHashSet<T> {
    public:

        HashSet() = default;

    private:



    };

}
