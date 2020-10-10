#include <cstdio>
#include <cstdlib>
#include <ctime>

int main(int argc, char *argv[])
{
    constexpr size_t n_edges = 400,
                     n_vertexes = 100;

    srand(time(NULL));

    printf("%lu %lu\n", n_vertexes, n_edges);
    
    size_t edge_cnt = 0;
    for (size_t i = 1; i <= n_vertexes; ++i) {
        for (size_t j = 1; j < i; ++j) {
            if (rand() % 2) {
                printf("%lu %lu 10000000\n", j, i);
                if (++edge_cnt > n_edges) {
                    return 0;
                }
            }
        }
    }

    return 0;
}
