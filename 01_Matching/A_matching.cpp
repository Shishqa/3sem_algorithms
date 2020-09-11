#ifdef SHISHQA
    #define $D( code ) code
#else
    #define $D( code ) if (0) { code }
#endif

#include <fstream>
#include <iostream>

#include <stdexcept>
#include <vector>
#include <set>
#include <map>


struct Matching {

    void add_match(const size_t& a_vertex,
                   const size_t& b_vertex);

    int get_match(const size_t& b_vertex) const;

    void print() const;

private:

    std::map<size_t, size_t> matches;
};


struct BipartileGraph {
    
    typedef std::set<size_t> Neighbours;

    BipartileGraph() = delete;

    BipartileGraph(const size_t& a_size);

    void connect(const size_t& a_vertex,
                 const size_t& b_vertex);


    [[nodiscard]] size_t a_size() const;

    const Neighbours& get_neighbours(const size_t& a_vertex) const; 

    void dump(std::ofstream& file) const; 

private:

    std::vector<std::set<size_t>> a_neighbours;
};


struct KuhnAlgo {

    KuhnAlgo() = delete;

    KuhnAlgo(const size_t& a_size);

    Matching get_max_matching(const BipartileGraph& graph);

private:

    bool try_find_augmenting_path(const size_t& a_vertex,
                                  const BipartileGraph& graph);

    static const int NO_MATCH = -1;

    std::vector<bool> used;
    Matching matching;
};


void read_connections(BipartileGraph& graph);

/*=[MAIN]=======================================================================*/

int main() {

    size_t a_size = 0, b_size = 0;
    $D( std::cout << "# Enter size of left  part: "; )
    std::cin >> a_size;
    $D( std::cout << "# Enter size of right part: "; )
    std::cin >> b_size;

    BipartileGraph graph(a_size);

    read_connections(graph); 
    $D( std::cout << std::endl; )

    KuhnAlgo kuhn_algo(a_size);
    Matching max_matching = kuhn_algo.get_max_matching(graph);
    $D( std::cout << std::endl; )

    max_matching.print();

    return 0;
}

/*==============================================================================*/

void read_connections(BipartileGraph& graph) {
    static const size_t NEIGHBOUR_LINE_END = 0;

    $D( std::cout << "# Enter neighbours of left part vertexes (terminated with 0)\n"; )

    for (size_t a_vertex = 0; a_vertex < graph.a_size(); ++a_vertex) {
        
        $D( std::cout << "# Neighbours of " << a_vertex + 1 << " vertex: "; )
        
        for (size_t b_vertex = 0;;) {
            std::cin >> b_vertex;
            if (b_vertex == NEIGHBOUR_LINE_END) {
                break;
            }
            graph.connect(a_vertex, b_vertex - 1);
        } 
    } 
}

/*==============================================================================*/

KuhnAlgo::KuhnAlgo(const size_t& a_size) 
    : used(std::vector<bool>(a_size)) {}


Matching KuhnAlgo::get_max_matching(const BipartileGraph& graph) {

    $D( std::cout << "# Started for looking for max matching\n"; )

    for (size_t a_v = 0; a_v < graph.a_size(); ++a_v) {
        used.assign(graph.a_size(), false);
        try_find_augmenting_path(a_v, graph); 
    }
    return matching;
}


bool KuhnAlgo::try_find_augmenting_path(const size_t& a_vertex,
                                        const BipartileGraph& graph) {
    if (used[a_vertex]) {
        return false;
    }
    used[a_vertex] = true;

    for (const auto& b_vertex : graph.get_neighbours(a_vertex)) {
        int match = matching.get_match(b_vertex);
        if (match == NO_MATCH ||
            try_find_augmenting_path(match, graph)) {

            matching.add_match(a_vertex, b_vertex);
            $D( std::cout << "#     matched: " << a_vertex + 1 << " - " << b_vertex + 1 << std::endl; )

            return true;
        }
    }

    return false; 
}

/*==============================================================================*/

BipartileGraph::BipartileGraph(const size_t& a_size)
        : a_neighbours(std::vector<std::set<size_t>>(a_size)) {}


void BipartileGraph::connect(const size_t& a_vertex, 
                             const size_t& b_vertex) {
    a_neighbours.at(a_vertex).insert(b_vertex);
}


const BipartileGraph::Neighbours& BipartileGraph::get_neighbours(const size_t& a_vertex) const {
    return a_neighbours.at(a_vertex);
} 


size_t BipartileGraph::a_size() const {
    return a_neighbours.size();
}


void BipartileGraph::dump(std::ofstream& file) const {
   
    file << "BipartileGraph:\n";

    for (size_t a_vertex = 0; a_vertex < a_size(); ++a_vertex) {
        file << a_vertex + 1 << " -> ";
        for (const auto& neighbour : a_neighbours[a_vertex]) {
            file << neighbour + 1 << " ";
        }
        file << std::endl;
    }
}

/*==============================================================================*/

int Matching::get_match(const size_t& b_vertex) const {
    try {
        return static_cast<int>(matches.at(b_vertex));
    } catch(const std::out_of_range& ex) {
        return -1;
    }
}


void Matching::add_match(const size_t& a_vertex, 
                         const size_t& b_vertex) {
    matches[b_vertex] = a_vertex; 
}


void Matching::print() const {
    $D( std::cout << "# Matching of size "; ) 
        std::cout << matches.size() << std::endl;
    $D( std::cout << "# Matches:\n"; )
    for (const auto& match : matches) {
        $D( std::cout << "#     "; )
        std::cout << match.second + 1 << " " << match.first + 1 << std::endl;
    }
}

