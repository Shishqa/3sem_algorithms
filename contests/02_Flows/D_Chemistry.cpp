/* (c) Shishqa, 2020 */
/*==========================================================================*/

#include <cstdio>
#include <cstdint>
#include <climits>
#include <stdexcept>

#include <vector>
#include <queue>

using std::vector;

/*==========================================================================*/

struct Edge {

    using flow_t = long long;
    static constexpr flow_t INF_FLOW = LLONG_MAX;

    Edge(const int& v_from, const int& v_to, 
         const flow_t& flow_capacity)
            : flow(0LL)
            , capacity(flow_capacity)
            , from(v_from)
            , to(v_to) {}

    bool is_saturated();
    flow_t residual_flow();

    flow_t flow;
    flow_t capacity;

    int from, to;
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

class Graph {
public:

    Graph() = delete;

    Graph(const size_t& left_size,
          const size_t& right_size);

    ~Graph() = default;

    void connect(const int& left_vertice, 
                 const int& right_vertice,
                 const Edge::flow_t& capacity);

    void dump();

    size_t n_vertices();

    size_t left_size();

    size_t right_size();

    Edge::flow_t push_max_flow(const int& from, const int& to);

    void set_sink(const vector<Edge::flow_t>& sink_cap); 
   
    Edge::flow_t min_sink();

    void set_source(const vector<Edge::flow_t>& source_cap); 

private:

    vector<Edge> edges;
    vector<vector<int>> incidence;

    const size_t left_part_size,
                 right_part_size;

    void insert_edge(const int& from, const int& to, 
                     const Edge::flow_t& capacity);

    /*------------------------------------------------------*/

    Edge::flow_t ford_fulkerson(const int& from, const int& to);
    
    Edge::flow_t push_flow_dfs(const int& from, const int& to, 
                               const Edge::flow_t& current_flow, 
                               vector<bool>& used);
    
    /*------------------------------------------------------*/

    Edge::flow_t edmonds_karp(const int& from, const int& to);

    Edge::flow_t push_flow_bfs(const int& from, const int& to);

    Edge::flow_t saturate_path(const int& from, const int& to,
                               const vector<int>& path);
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void read_connections(Graph& graph);

int get_degree(const char& c);

bool check_connect(const int& y,  const int& x,
                   const int& dy, const int& dx,
                   const size_t& width,
                   const size_t& height);

/*==========================================================================*/

int main() {

    static constexpr size_t MAX_PAPER_WIDTH = 50;

    size_t paper_height = 0,
           paper_width  = 0;
    scanf("%lu %lu", &paper_height, &paper_width);

    const size_t paper_size = paper_width * paper_height;

    vector<char[MAX_PAPER_WIDTH + 1]> paper(paper_height);
    for (auto& line : paper) {
        scanf("%s", line);
    }
    
    if (1 == paper_size) {
        printf("Invalid");
        return 0;
    }

    const size_t n_whites = (paper_size + 1) / 2,
                 n_blacks = paper_size - n_whites;

    Graph graph(n_whites, n_blacks);

    vector<Edge::flow_t> whites_deg(n_whites);
    vector<Edge::flow_t> blacks_deg(n_blacks);
    
    Edge::flow_t whites_cap = 0,
                 blacks_cap = 0;

    const int delta[4][2] = {
        {-1, 0},
        {1,  0},
        {0, -1},
        {0,  1}
    };

    bool has_any_symbol = false;

    for (int i = 0; i < paper_height; ++i) {
        for (int j = 0; j < paper_width; ++j) {

            int deg = get_degree(paper[i][j]);

            if ('.' != paper[i][j]) {
                has_any_symbol = true;
            }

            if (0 == (i + j) % 2) { 

                whites_deg[(i * paper_width + j) / 2] = deg;
                whites_cap += deg;

                if ('.' != paper[i][j]) {

                    for (size_t k = 0; k < 4; ++k) {
                        if (check_connect(i, j, delta[k][0], delta[k][1], 
                                          paper_width, paper_height) &&
                            '.' != paper[i + delta[k][0]][j + delta[k][1]]) {

                            graph.connect((i * paper_width + j) / 2 + 1, 
                                          ((i + delta[k][0]) * paper_width + j + delta[k][1]) / 2 + 1, 
                                          1LL);
                        }
                    }
                }

           } else { 

                blacks_deg[(i * paper_width + j) / 2] = deg;
                blacks_cap += deg;

            }

        }
    }

    graph.set_source(whites_deg);
    graph.set_sink(blacks_deg);

    if (blacks_cap != whites_cap ||
        !has_any_symbol) {
        printf("Invalid");
        return 0;
    }

    Edge::flow_t flow = graph.push_max_flow(1, graph.n_vertices());

    if (flow == blacks_cap) {
        printf("Valid");
    } else {
        printf("Invalid");
    }

    return 0;
}

/*==========================================================================*/

bool check_connect(const int& y,  const int& x,
                   const int& dy, const int& dx,
                   const size_t& width,
                   const size_t& height) {
    return (0 <= y + dy && y + dy < height &&
            0 <= x + dx && x + dx < width);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

int get_degree(const char& c) {
    
    switch (c) {               
        case '.':
            return 0;

        case 'H':
            return 1;

        case 'O':
            return 2;

        case 'N':
            return 3;
         
        case 'C':
            return 4;
        
        default:
            throw std::runtime_error("unknown symbol");
    }
}

/*==========================================================================*/

Edge::flow_t Graph::push_max_flow(const int& from, const int& to) {

    return ford_fulkerson(from - 1, to - 1);
}

/*==========================================================================*/

Edge::flow_t Graph::edmonds_karp(const int& from, const int& to) {

    Edge::flow_t max_flow   = 0,
                 flow_delta = 0;
    do {
        flow_delta = push_flow_bfs(from, to);
        max_flow += flow_delta;
    } while (0 != flow_delta);

    return max_flow;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Edge::flow_t Graph::push_flow_bfs(const int& from, const int& to) {

    vector<bool> used(n_vertices(), false);
        
    vector<int> path(n_vertices(), -1);

    std::queue<int> to_visit;
    to_visit.push(from);

    while (!to_visit.empty()) {

        int v = to_visit.front();
                to_visit.pop();

        used[v] = true;

        if (to == v) {
            return saturate_path(from, to, path);
        }

        for (const auto& e : incidence[v]) {
            if (!used[edges[e].to] && !edges[e].is_saturated()) {
                to_visit.push(edges[e].to);
                path[edges[e].to] = e;
            }
        }
    }

    return 0;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Edge::flow_t Graph::saturate_path(const int& from, const int& to,
                                  const vector<int>& path) {

    Edge::flow_t possible_flow = Edge::INF_FLOW;

    for (int v = to; from != v; v = edges[path[v]].from) {
        possible_flow = std::min(possible_flow, edges[path[v]].residual_flow());
    }

    for (int v = to; from != v; v = edges[path[v]].from) {
        edges[path[v]].flow     += possible_flow;
        edges[path[v] ^ 1].flow -= possible_flow;
    }

    return possible_flow;
}

/*==========================================================================*/

Edge::flow_t Graph::ford_fulkerson(const int& from, const int& to) {

    vector<bool> used(n_vertices(), false);

    Edge::flow_t max_flow   = 0,
                 flow_delta = 0;

    do {
        used.assign(n_vertices(), false);

        flow_delta = push_flow_dfs(from, to, Edge::INF_FLOW, used);
        max_flow += flow_delta;

    } while (0 != flow_delta);

    return max_flow;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Edge::flow_t Graph::push_flow_dfs(const int& from, const int& to,
                                  const Edge::flow_t& max_flow,
                                  vector<bool>& used) {

    if (used[from]) {
        return 0;
    }

    used[from] = true;

    if (from == to) {
        return max_flow;
    }

    for (const auto& e : incidence[from]) {

        int v = edges[e].to;

        if (!edges[e].is_saturated()) {

            Edge::flow_t flow_delta = push_flow_dfs(
                    v, to, 
                    std::min(max_flow, edges[e].residual_flow()), 
                    used
                    );

            if (flow_delta > 0) {

                edges[e].flow     += flow_delta;
                edges[e ^ 1].flow -= flow_delta;
                
                return flow_delta;
            }
        }
    }

    return 0;
}

/*==========================================================================*/

bool Edge::is_saturated() {
    return (0 == residual_flow());
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Edge::flow_t Edge::residual_flow() {
    return capacity - flow;
}

/*==========================================================================*/

Graph::Graph(const size_t& left_size, const size_t& right_size)
    : left_part_size(left_size)
    , right_part_size(right_size) {

    incidence.resize(left_size + right_size + 2);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void Graph::set_sink(const vector<Edge::flow_t> &sink_cap) {

    if (sink_cap.size() != right_part_size) {
        throw std::runtime_error("setting sink of different size");
    }

    const int sink = 1 + left_part_size + right_part_size;

    for (size_t v = 1; v <= right_part_size; ++v) {
       
        if (sink_cap[v - 1] == 0) {
            continue;
        }

        insert_edge(v + left_part_size, sink, sink_cap[v - 1]);
        insert_edge(sink, v + left_part_size, 0LL);
    }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Edge::flow_t Graph::min_sink() {
    
    Edge::flow_t min_sink = Edge::INF_FLOW;

    const int sink = 1 + left_part_size + right_part_size;

    for (const auto& e : incidence[sink]) {

        min_sink = std::min(min_sink, edges[e ^ 1].flow); 

    }

    return min_sink;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void Graph::set_source(const vector<Edge::flow_t> &source_cap) {

    if (source_cap.size() != left_part_size) {
        throw std::runtime_error("setting source of different size");
    }

    const int source = 0;

    for (size_t v = 1; v <= left_part_size; ++v) {
        
        if (source_cap[v - 1] == 0) {
            continue;
        }

        insert_edge(source, v, source_cap[v - 1]);
        insert_edge(v, source, 0LL);
    }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void Graph::connect(const int &left_vertice, const int &right_vertice,
                    const Edge::flow_t& capacity) {

    insert_edge(left_vertice, left_part_size + right_vertice, capacity); 
    insert_edge(left_part_size + right_vertice, left_vertice, 0LL); 

}

/*==========================================================================*/

void Graph::insert_edge(const int &from, const int &to, 
                        const Edge::flow_t& capacity) {

    edges.emplace_back(from, to, capacity);
    incidence[from].emplace_back(edges.size() - 1);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

size_t Graph::n_vertices() {
    return incidence.size();
}

size_t Graph::left_size() {
    return left_part_size;
}

size_t Graph::right_size() {
    return right_part_size;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void Graph::dump() {

    printf(    "#######################################################\n"
               " GRAPH\n"
               " n_vertices = %lu\n"
               " n_edges    = %lu\n"
               "_______________________________________________________\n\n",
               incidence.size(), edges.size());

    for (int v = 0; v < static_cast<int>(n_vertices()); ++v) {

        printf(" vertice %d (deg = %lu):\n",
               v + 1, incidence[v].size()); 

        for (const auto& e : incidence[v]) {

            printf("\t\tedge %d: %d --(%lld/%lld)--> %d\n",
                   e + 1, edges[e].from + 1, edges[e].flow,
                   edges[e].capacity, edges[e].to + 1);
        }

        printf("\n");
    }

    printf(    "#######################################################\n");
}

/*==========================================================================*/
