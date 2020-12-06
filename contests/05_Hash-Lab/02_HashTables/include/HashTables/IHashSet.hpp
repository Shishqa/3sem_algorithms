/*============================================================================*/
#ifndef RESEARCH_IHASHSET_HPP
#define RESEARCH_IHASHSET_HPP
/*============================================================================*/
namespace Research {

    template<typename T>
    class IHashSet {
    public:

        IHashSet() = default;

        virtual ~IHashSet() = default;

        virtual void add(const T &key) = 0;

        virtual bool find(const T &key) = 0;

        virtual void remove(const T &key) = 0;

    };

}
/*============================================================================*/
#endif
/*============================================================================*/