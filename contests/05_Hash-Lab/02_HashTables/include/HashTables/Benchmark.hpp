/*============================================================================*/
#ifndef HASHTABLES_RESEARCH_BENCHMARK_HPP
#define HASHTABLES_RESEARCH_BENCHMARK_HPP
/*============================================================================*/
#include "IHashSet.hpp"
#include "Hashes/HashCollection.hpp"
#include <map>
#include "Timer.hpp"
/*============================================================================*/
namespace Research {

    class Benchmark {
    public:

        Benchmark() = delete;

        using Result = std::map<size_t, TimeDelta>;

        static Result run(IHashSet<Hashes::String>& hash_set);

        static Result run(IHashSet<Hashes::Number>& hash_set);

        static void dumpCSV(std::string_view filename,
                            const Result& result);

    };

}
/*============================================================================*/
#endif //HASHTABLES_RESEARCH_BENCHMARK_HPP
/*============================================================================*/