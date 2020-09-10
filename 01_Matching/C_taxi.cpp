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

struct Matching {

    void add_match(const size_t& a_vertex,
                   const size_t& b_vertex);

    int get_match(const size_t& b_vertex) const;

    void print() const;

    [[nodiscard]] size_t size() const;

private:

    std::map<size_t, size_t> matches;
};

/*------------------------------------------------------------------------------*/

struct BipartileGraph {
    
    typedef std::set<size_t> Neighbours;

    BipartileGraph() = delete;

    explicit BipartileGraph(const size_t& a_size);

    void connect(const size_t& a_vertex,
                 const size_t& b_vertex);


    [[nodiscard]] size_t a_size() const;

    const Neighbours& get_neighbours(const size_t& a_vertex) const; 

    void dump() const; 

private:

    std::vector<std::set<size_t>> a_neighbours;
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

/*------------------------------------------------------------------------------*/

struct Point {
    int x, y;
};

/*------------------------------------------------------------------------------*/

struct Order {

    int start_time;
    int drive_time;

    Point start, finish;
};

/*==============================================================================*/

void read_orders(const size_t& n_orders,
                 BipartileGraph& graph);


int time_dist(const Point& a, const Point& b);

/*=[MAIN]=======================================================================*/

int main() {

    size_t n_orders = 0;
    if(!scanf("%lu", &n_orders)) {
        throw std::runtime_error("Error: scanf failed to read\n");
    }
   
    BipartileGraph graph(n_orders);
    read_orders(n_orders, graph); 

    $D( graph.dump(); )

    KuhnAlgo kuhn_algo(n_orders);
    Matching max_matching = kuhn_algo.get_max_matching(graph);

    printf("%lu\n", n_orders - max_matching.size());

    return 0;
}

/*==============================================================================*/

int time_dist(const Point& a, const Point& b) {

    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

/*------------------------------------------------------------------------------*/

void read_orders(const size_t& n_orders,
                 BipartileGraph& graph) {
    
    static const int ORDER_TIME_GAP = 1;

    std::vector<Order> orders(n_orders);

    int hours = 0, minutes = 0;
    for (size_t i = 0; i < n_orders; ++i) {

        if (scanf("%d:%d %d %d %d %d", &hours, &minutes,
                                       &orders[i].start.x,  &orders[i].start.y,
                                       &orders[i].finish.x, &orders[i].finish.y) < 6) {
            throw std::runtime_error("Error: scanf failed to read\n");
        }

        orders[i].start_time = minutes + 60 * hours;
        orders[i].drive_time = time_dist(orders[i].start, orders[i].finish);

        $D( printf("order: %d -> %d (+%d)\n", orders[i].start_time, 
                                              orders[i].start_time + orders[i].drive_time, 
                                              ORDER_TIME_GAP); )

        for (size_t j = 0; j < i; ++j) {
            
            if (orders[j].start_time + orders[j].drive_time + ORDER_TIME_GAP + 
                time_dist(orders[j].finish, orders[i].start) <= orders[i].start_time) {
                graph.connect(j, i);
            }

        }
    } 
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

BipartileGraph::BipartileGraph(const size_t& a_size)
        : a_neighbours(std::vector<std::set<size_t>>(a_size)) {}

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
