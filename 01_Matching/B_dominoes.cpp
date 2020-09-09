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


template <typename T> 
using matrix = std::vector<std::vector<T>>;


struct Matching {

    void add_match(const size_t& a_vertex,
                   const size_t& b_vertex);

    int get_match(const size_t& b_vertex) const;

    [[nodiscard]] size_t size() const;

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


size_t fill_graph(std::ifstream& input, BipartileGraph& graph, 
                  const size_t& board_height, 
                  const size_t& board_width);


size_t pave_dominoes(BipartileGraph& graph);

/*=[MAIN]=======================================================================*/

int main() {

    std::ifstream input("dominoes.in");

    size_t board_height = 0, board_width = 0;
    input >> board_height >> board_width;

    // checkerboard painting
    // #_#_#_#_
    // _#_#_#_#   '_' - white cell
    // #_#_#_#_
    BipartileGraph white_cells(board_height * board_width / 2);

    long long dominoe_cost = 0, cell_cost = 0;
    input >> dominoe_cost >> cell_cost;

    size_t empty_cell_cnt = fill_graph(input, white_cells, 
                                       board_height, board_width); 
    input.close();

    size_t n_dominoes = 0,
           n_cells    = 0;

    if (cell_cost * 2 > dominoe_cost) {
        n_dominoes = pave_dominoes(white_cells);
        n_cells = empty_cell_cnt - 2 * n_dominoes;
    } else {
        n_cells = empty_cell_cnt;
    }

    $D( std::cout << "need to put " << n_dominoes << " dominoes\n"
                     "and " << n_cells << " cells\n"; )

    std::ofstream output("dominoes.out");

    output << n_cells * cell_cost + n_dominoes * dominoe_cost;
    
    output.close();

    return 0;
}

/*==============================================================================*/

size_t fill_graph(std::ifstream& input, BipartileGraph& graph, 
                  const size_t& board_height, 
                  const size_t& board_width) {

    static const char EMPTY = '*';

    matrix<char> board(board_height, std::vector<char>(board_width));

    for (int y = 0; y < board_height; ++y) {
        for (int x = 0; x < board_width; ++x) {
            
            input >> board[y][x];

        }
    }
   
    std::vector<int> dx = {-1,  0,  0,  1};
    std::vector<int> dy = { 0, -1,  1,  0};

    size_t empty_cell_cnt = 0;

    for (int y = 0; y < board_height; ++y) {
        for (int x = 0; x < board_width; ++x) {

            if ((x + y) % 2 && board[y][x] == EMPTY) {
                for (size_t i = 0; i < 4; ++i) {
                    if (0 <= x + dx[i] && x + dx[i] < board_width  &&
                        0 <= y + dy[i] && y + dy[i] < board_height &&
                        board[y + dy[i]][x + dx[i]] == EMPTY) {

                        graph.connect((y * board_width + x) / 2, 
                                      ((y + dy[i]) * board_width + x + dx[i]) / 2);
                    }
                }
            }

            if (board[y][x] == EMPTY) {
                ++empty_cell_cnt;
            }
        
        }
    }   

    return empty_cell_cnt;
}


size_t pave_dominoes(BipartileGraph& white_cells) {
    
    KuhnAlgo kuhn(white_cells.a_size());
    Matching paving = kuhn.get_max_matching(white_cells);

    return paving.size();
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


size_t Matching::size() const {
    return matches.size();
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

