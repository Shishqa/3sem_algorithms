#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>

#include <new>
#include <vector>
#include <set>


struct BipartileGraph {
    
    BipartileGraph(const size_t& a_size,
                   const size_t& b_size);

    void connect(const int& a_vertex,
                 const int& b_vertex);

    void get_max_matching();

    void dump(FILE* file);

private:

    std::vector<std::set<int>> a_vertex_neighbours;
};


int main() {

    static const int NEIGHBOUR_LINE_END = 0;

    size_t a_size = 0, b_size = 0;
    std::cin >> a_size >> b_size;

    BipartileGraph graph(a_size, b_size);

    int b_vertex = 0;
    for (int a_vertex = 0; a_vertex < a_size; ++a_vertex) {
        do {
            std::cin >> b_vertex;
            graph.connect(a_vertex, b_vertex);
        } while (b_vertex != NEIGHBOUR_LINE_END);
    }

    graph.dump(stdout);

    return 0;
}


BipartileGraph::BipartileGraph(const size_t& a_size,
                               const size_t& b_size)
        : a_vertex_neighbours(std::vector<std::set<int>>(a_size)) {}


void BipartileGraph::connect(const int& a_vertex, 
                             const int& b_vertex) {
    
    /* TODO: check if vertexes are in borders */

    a_vertex_neighbours[a_vertex].insert(b_vertex);
}


void BipartileGraph::dump(FILE* file) {
    if (!file) {
        throw std::runtime_error("Error: file");
    }

    for (int a_vertex = 0; a_vertex < a_vertex_neighbours.size(); ++a_vertex) {
        fprintf(file, "%d -> ", a_vertex + 1);
        for (auto& neighbour : a_vertex_neighbours[a_vertex]) {
            fprintf(file, "%d ", neighbour);
        }
        fprintf(file, "\n");
    }
}
