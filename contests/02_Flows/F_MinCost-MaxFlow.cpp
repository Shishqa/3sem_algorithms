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
    using cost_t = long long;
    static constexpr flow_t INF_FLOW = LLONG_MAX;
    static constexpr flow_t INF_COST = LLONG_MAX;

    Edge(const int& v_from, const int& v_to, 
         const flow_t& flow_capacity,
         const cost_t& edge_cost)
            : flow(0LL)
            , capacity(flow_capacity)
            , cost(edge_cost)
            , from(v_from)
            , to(v_to) {}

    bool is_saturated() const;
    flow_t residual_flow() const;

    flow_t flow;
    flow_t capacity;

    cost_t cost;

    int from, to;
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

class Graph {
public:

    Graph(FILE* file);

    Graph() = delete;

    ~Graph() = default;

    void insert_edge(const int& from, const int& to, 
                     const Edge::flow_t& capacity,
                     const Edge::cost_t& cost);

    int find_edge(const int& from, const int& to);
    
    void dump();

    size_t n_vertices();

    Edge::flow_t push_min_cost_max_flow(const int& from, const int& to);

private:

    vector<Edge> edges;
    vector<vector<int>> incidence;

    Edge::cost_t get_shortest_path(const int& from, const int& to, vector<int>& path);

    Edge::flow_t push_through_path(const int& from, const int& to, const vector<int>& path);
};

/*==========================================================================*/

int main() {

    Graph graph(stdin);

    Edge::cost_t min_cost = graph.push_min_cost_max_flow(1, graph.n_vertices());

    printf(/*"MIN_COST = "*/ "%lld\n", min_cost);

    return 0;
}

/*==========================================================================*/

Edge::cost_t Graph::get_shortest_path(const int& from, const int& to, vector<int>& path) {
 
    vector<Edge::cost_t> dist(n_vertices(), Edge::INF_COST);
 
    dist[from] = 0;
 
    for (;;) {
        bool any = false;
        for (size_t e = 0; e < edges.size(); ++e) {

            if (edges[e].is_saturated()) {
                continue;
            }

            if (dist[edges[e].from] < Edge::INF_COST && 
                dist[edges[e].to] > dist[edges[e].from] + edges[e].cost) {
                
                dist[edges[e].to] = dist[edges[e].from] + edges[e].cost;
                any = true;

                path[edges[e].to] = e;
            }
        }
        if (!any) {
            break;
        }
    }

    return dist[to];
}

/*==========================================================================*/

Edge::cost_t Graph::push_min_cost_max_flow(const int &from, const int &to) {

    vector<int> path;

    Edge::cost_t path_cost = 0;     
    Edge::flow_t path_flow = 0;   

    //dump();

    for (;;) {

        path.assign(n_vertices(), -1);
        
        Edge::cost_t cost_delta = get_shortest_path(from - 1, to - 1, path);
        if (cost_delta == Edge::INF_COST) {
            break;
        }
        path_cost += cost_delta;

        path_flow += push_through_path(from - 1, to - 1, path);

        //dump();
    }

    return path_cost;
}

/*==========================================================================*/

Edge::flow_t Graph::push_through_path(const int& from, const int& to,
                                      const vector<int>& path) {

    Edge::flow_t possible_flow = Edge::INF_FLOW;

    for (int e = path[to];; e = path[edges[e].from]) {
        possible_flow = std::min(possible_flow, edges[e].residual_flow());
        if (from == edges[e].from) {
            break;
        }
    }

    for (int e = path[to];; e = path[edges[e].from]) {
        
        edges[e].flow     += possible_flow;
        edges[e ^ 1].flow -= possible_flow;

        if (from == edges[e].from) {
            break;
        }
    }

    return possible_flow;
}

/*==========================================================================*/

bool Edge::is_saturated() const {
    return (0 == residual_flow());
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Edge::flow_t Edge::residual_flow() const {
    return capacity - flow;
}

/*==========================================================================*/

Graph::Graph(FILE* file) {

    size_t n_vertices = 0,
           n_edges    = 0;

    if (2 != fscanf(file, "%lu %lu", &n_vertices, &n_edges)) {
        throw std::runtime_error("couldn't read graph size");
    }

    edges.reserve(n_edges * 2);
    incidence.resize(n_vertices);

    for (size_t i = 0; i < n_edges; ++i) {

        int from_vertice = 0, 
            to_vertice   = 0;

        Edge::flow_t edge_capacity = 0;
        Edge::cost_t edge_cost     = 0;

        if (4 != fscanf(file, "%d %d %lld %lld", 
                              &from_vertice,
                              &to_vertice,
                              &edge_capacity,
                              &edge_cost)) {
            throw std::runtime_error("couldn't read an edge");
        }

        if (from_vertice == to_vertice) {
            continue;
        }

        --from_vertice; --to_vertice;

        /* main edge */
        insert_edge(from_vertice, to_vertice, edge_capacity, edge_cost);
        
        /* back edge */
        insert_edge(to_vertice, from_vertice, 0LL, -edge_cost);
    }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void Graph::insert_edge(const int &from, const int &to, 
                        const Edge::flow_t& capacity,
                        const Edge::cost_t& edge_cost) {

    /*
    for (const auto& e : incidence[from]) {
        if (to == edges[e].to) {
            edges[e].capacity += capacity;
        }
    }
    */

    edges.emplace_back(from, to, capacity, edge_cost);
    incidence[from].emplace_back(edges.size() - 1);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

size_t Graph::n_vertices() {
    return incidence.size();
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

            printf("\t\tedge %d: %d --(%lld/%lld){%lld}--> %d\n",
                   e + 1, edges[e].from + 1, edges[e].flow,
                   edges[e].capacity, edges[e].cost, edges[e].to + 1);
        }

        printf("\n");
    }

    printf(    "#######################################################\n");
}

/*==========================================================================*/
