#include <fstream>
#include <iostream>

#include <vector>
#include <set>


struct Match {
    
    Match(const size_t& a_vertex,
          const size_t& b_vertex);

    void print() const;

private:

    size_t a_vertex,
           b_vertex;
};


struct Matching {

    void print() const;

private:

    std::set<Match> matches;
};


struct BipartileGraph {
    
    BipartileGraph(const size_t& a_size,
                   const size_t& b_size);

    void connect(const size_t& a_vertex,
                 const size_t& b_vertex);

    Matching get_max_matching() const;

    [[nodiscard]] size_t a_size() const;

    // [[nodiscard]] size_t b_size();

    void dump(std::ofstream& file) const;

private:

    std::vector<std::set<size_t>> a_vertex_neighbours;
};


void read_connections(BipartileGraph& graph);


int main() {

    size_t a_size = 0, b_size = 0;
    std::cout << "# Enter size of left part: ";
    std::cin >> a_size;
    std::cout << "# Enter size of right part: ";
    std::cin >> b_size;

    BipartileGraph graph(a_size, b_size);

    read_connections(graph); 

    std::ofstream file("log.txt");
    graph.dump(file);
    file.close();

    Matching max_matching = graph.get_max_matching();

    max_matching.print();

    return 0;
}


void read_connections(BipartileGraph& graph) {
    static const size_t NEIGHBOUR_LINE_END = 0;

    std::cout << "Enter neighbours of left part vertexes\n";

    for (size_t a_vertex = 0; a_vertex < graph.a_size(); ++a_vertex) {
        std::cout << "Neighbours of " << a_vertex + 1 << " vertex: ";
        for (size_t b_vertex = 0;;) {
            std::cin >> b_vertex;
            if (b_vertex == NEIGHBOUR_LINE_END) {
                break;
            }
            graph.connect(a_vertex, b_vertex);
        } 
    } 
}


Matching BipartileGraph::get_max_matching() const {
    std::cout << "getting max matching:\n";
    return Matching();
}


BipartileGraph::BipartileGraph(const size_t& a_size,
                               const size_t& b_size)
        : a_vertex_neighbours(std::vector<std::set<size_t>>(a_size)) {}


void BipartileGraph::connect(const size_t& a_vertex, 
                             const size_t& b_vertex) {
    
    /* TODO: check if vertexes are in borders */

    a_vertex_neighbours[a_vertex].insert(b_vertex);
}


size_t BipartileGraph::a_size() const {
    return a_vertex_neighbours.size();
}


void BipartileGraph::dump(std::ofstream& file) const {
   
    file << "BipartileGraph:\n";

    for (size_t a_vertex = 0; a_vertex < a_size(); ++a_vertex) {
        file << a_vertex + 1 << " -> ";
        for (const auto& neighbour : a_vertex_neighbours[a_vertex]) {
            file << neighbour << " ";
        }
        file << std::endl;
    }
}


void Matching::print() const {
    std::cout << "Matching -- " << matches.size() << ":\n";
    for (const auto& match : matches) {
        match.print();
    }
}


void Match::print() const {
    std::cout << a_vertex << " - " << b_vertex << "\n";
}
