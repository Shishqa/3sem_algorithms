// (c) Shishqa, 2020
/*==============================================================================*/

#ifdef SHISHQA
    #define $D( code ) code
#else
    #define $D( code ) if (0) { code }
#endif

#include <cstdio>
#include <stdexcept>

#include <vector>
#include <set>
#include <map>

/*==============================================================================*/

struct BipartileGraph {
    
    typedef std::set<size_t> Neighbours;

    BipartileGraph() = delete;

    BipartileGraph(const size_t& a_size,
                   const size_t& b_size);

    void connect(const size_t& a_vertex,
                 const size_t& b_vertex);

    [[nodiscard]] size_t a_size() const;

    [[nodiscard]] size_t b_size() const;

    const Neighbours& get_neighbours(const size_t& a_vertex) const; 

    void dump() const; 

private:

    const size_t b_part_size;

    std::vector<std::set<size_t>> a_neighbours;
};

/*------------------------------------------------------------------------------*/

struct Matching {

    void add_match(const size_t& a_vertex,
                   const size_t& b_vertex);

    int get_match(const size_t& b_vertex) const;

    void print() const;

    [[nodiscard]] size_t size() const;

private:

    std::map<size_t, size_t> matches;

    friend void find_min_vertex_cover(BipartileGraph& graph, Matching& max_matching);
};

/*------------------------------------------------------------------------------*/

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

/*==============================================================================*/

void solve_birthday_problem();

void connect_non_familiar(BipartileGraph& graph);

void find_max_independent_set(BipartileGraph& graph, Matching& max_matching);

void reach_a(const size_t& b_vertex, BipartileGraph& graph, Matching& max_matching,
             std::set<size_t>& used_a, std::set<size_t>& used_b);

void reach_b(const size_t& a_vertex, BipartileGraph& graph, Matching& max_matching,
             std::set<size_t>& used_a, std::set<size_t>& used_b);

/*=[MAIN]=======================================================================*/

int main() {

    size_t n_sets = 0;
    if(!scanf("%lu", &n_sets)) {
        throw std::runtime_error("Error: scanf failed to read\n");
    }

    for (size_t i = 0; i < n_sets; ++i) {
        solve_birthday_problem();
        printf("\n");
    }

    return 0;
}

/*==============================================================================*/

void solve_birthday_problem() {
    
    size_t n_boys = 0, n_girls = 0;
    if(scanf("%lu %lu", &n_boys, &n_girls) != 2) {
        throw std::runtime_error("Error: scanf failed to read\n");
    }
    
    BipartileGraph non_familiarity(n_boys, n_girls);

    connect_non_familiar(non_familiarity);

    KuhnAlgo kuhn(n_boys);
    Matching max_matching = kuhn.get_max_matching(non_familiarity);

    find_max_independent_set(non_familiarity, max_matching);
}

/*------------------------------------------------------------------------------*/

void connect_non_familiar(BipartileGraph& graph) {
    
    std::set<size_t> familiar_girls; 
    for (size_t boy = 0; boy < graph.a_size(); ++boy) {
        
        for(size_t girl = 0;;) {

            if (!scanf("%lu", &girl)) {
                throw std::runtime_error("Error: scanf failed to read\n");
            }

            if (!girl) {
                break;
            }

            familiar_girls.insert(girl - 1); 
        }

        for (size_t girl = 0; girl < graph.b_size(); ++girl) {
            if (!familiar_girls.count(girl)) {
                graph.connect(boy, girl);
            }
        }

        familiar_girls.clear();
    }
}

/*------------------------------------------------------------------------------*/

void reach_a(const size_t& b_vertex, BipartileGraph& graph, Matching& max_matching,
             std::set<size_t>& used_a, std::set<size_t>& used_b) {

    if (used_b.count(b_vertex)) {
        return;
    }

    used_b.insert(b_vertex);

    int a_neighbour = max_matching.get_match(b_vertex);
    if (a_neighbour != -1) {
        reach_b(a_neighbour, graph, max_matching, used_a, used_b);
    }
}

/*------------------------------------------------------------------------------*/

void reach_b(const size_t& a_vertex, BipartileGraph& graph, Matching& max_matching,
             std::set<size_t>& used_a, std::set<size_t>& used_b) {

    if (used_a.count(a_vertex)) {
        return;
    }

    used_a.insert(a_vertex);

    for (const auto& b_neighbour : graph.get_neighbours(a_vertex)) {
        
        if(max_matching.get_match(b_neighbour) != a_vertex) {
            reach_a(b_neighbour, graph, max_matching, used_a, used_b);
        }

    }
}

/*------------------------------------------------------------------------------*/

void find_max_independent_set(BipartileGraph& graph, Matching& max_matching) {

    std::set<size_t> used_a;
    std::set<size_t> used_b;

    for (size_t a = 0; a < graph.a_size(); ++a) {
       
        bool is_filled = false;
        for (const auto& neighbour : graph.get_neighbours(a)) {
            if (max_matching.get_match(neighbour) == a) {
                is_filled = true;
            }
        }

        if (!is_filled) {
            reach_b(a, graph, max_matching, used_a, used_b);            
        }
    }

    printf("%lu %lu\n", used_a.size(), graph.b_size() - used_b.size());    

    for (const auto& a : used_a) {
        printf("%lu ", a + 1);
    }
    printf("\n");

    for (size_t b = 0; b < graph.b_size(); ++b) {
        if (!used_b.count(b)) {
            printf("%lu ", b + 1);
        }
    }
    printf("\n");

}

/*==============================================================================*/

KuhnAlgo::KuhnAlgo(const size_t& a_size) 
    : used(std::vector<bool>(a_size)) {}

/*------------------------------------------------------------------------------*/    

Matching KuhnAlgo::get_max_matching(const BipartileGraph& graph) {

    for (size_t a_v = 0; a_v < graph.a_size(); ++a_v) {
        used.assign(graph.a_size(), false);
        try_find_augmenting_path(a_v, graph); 
    }
    return matching;
}

/*------------------------------------------------------------------------------*/

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

            return true;
        }
    }

    return false; 
}

/*==============================================================================*/

BipartileGraph::BipartileGraph(const size_t& a_size,
                               const size_t& b_size)
        : a_neighbours(std::vector<std::set<size_t>>(a_size)),
          b_part_size(b_size) {}

/*------------------------------------------------------------------------------*/

void BipartileGraph::connect(const size_t& a_vertex, 
                             const size_t& b_vertex) {
    a_neighbours.at(a_vertex).insert(b_vertex);
}

/*------------------------------------------------------------------------------*/

const BipartileGraph::Neighbours& BipartileGraph::get_neighbours(const size_t& a_vertex) const {
    return a_neighbours.at(a_vertex);
} 

/*------------------------------------------------------------------------------*/

size_t BipartileGraph::a_size() const {
    return a_neighbours.size();
}

/*------------------------------------------------------------------------------*/

size_t BipartileGraph::b_size() const {
    return b_part_size;
}

/*------------------------------------------------------------------------------*/

void BipartileGraph::dump() const {
    for (size_t i = 0; i < a_size(); ++i) {
        
        printf("%lu -> ", i + 1);

        for (const auto& neighbour : a_neighbours[i]) {
            printf("%lu ", neighbour + 1);
        }

        printf("\n");
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

/*------------------------------------------------------------------------------*/

void Matching::add_match(const size_t& a_vertex, 
                         const size_t& b_vertex) {
    matches[b_vertex] = a_vertex; 
}

/*------------------------------------------------------------------------------*/

size_t Matching::size() const {
    return matches.size();
}

/*==============================================================================*/
