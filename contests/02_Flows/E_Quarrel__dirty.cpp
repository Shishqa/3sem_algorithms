/* (c) Shishqa, 2020 */
/*==========================================================================*/

#include <cstdio>
#include <cstdint>
#include <climits>
#include <stdexcept>

#include <vector>
#include <queue>
#include <set>

using std::vector;
using std::queue;
using std::set;

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

    Graph()  = delete;

    Graph(const size_t& n_vertices);

    ~Graph() = default;

    void insert_edge(const int& from, const int& to, 
                     const Edge::flow_t& capacity);

    void dump();

    size_t n_vertices();

    /*------------------------------------------------------*/

    Edge::flow_t push_max_flow(const int& from, const int& to);

    /*------------------------------------------------------*/

    /* fills cut with edges in cut
     * returns total edges capacity */
    Edge::flow_t find_min_cut(const int& from, const int& to,
                              set<int>& cut);

    /*------------------------------------------------------*/

    Edge::flow_t push_flow_dfs(const int& from, const int& to, 
                               const Edge::flow_t& current_flow, 
                               vector<bool>& used);

    Edge::flow_t push_flow_bfs(const int& from, const int& to);

    Edge::flow_t outflow(const int& v);

private:

    vector<Edge> edges;
    vector<vector<int>> incidence;

    /*------------------------------------------------------*/

    Edge::flow_t ford_fulkerson(const int& from, const int& to);
    
    /*------------------------------------------------------*/

    Edge::flow_t edmonds_karp(const int& from, const int& to);


    Edge::flow_t saturate_path(const int& from, const int& to,
                               const vector<int>& path);

    friend bool get_path(Graph& graph, const int& from, const int& to, vector<int>& path);
};

/*==========================================================================*/

bool get_path(Graph& graph, const int& from, const int& to, vector<int>& path);

/*==========================================================================*/

int main() {

    size_t n_vertices = 0,
           n_edges    = 0;

    size_t start  = 0,
           finish = 0;

    if (4 != scanf("%lu %lu %lu %lu", &n_vertices, &n_edges, &start, &finish)) {
        throw std::runtime_error("couldn't read graph size");
    }

    --start; --finish;

    Graph graph(n_vertices);

    for (size_t i = 0; i < n_edges; ++i) {

        int from_vertice = 0, 
            to_vertice   = 0;

        if (2 != scanf("%d %d", &from_vertice,
                                &to_vertice)) {
            throw std::runtime_error("couldn't read an edge");
        }

        --from_vertice; --to_vertice;

        /* main edge */
        graph.insert_edge(from_vertice, to_vertice, 1LL);
        
        /* back edge */
        graph.insert_edge(to_vertice, from_vertice, 0LL);
    }

    //graph.dump();


    vector<bool> used(graph.n_vertices(), false);
    while (graph.outflow(start) < 2 &&
           graph.push_flow_dfs(start, finish, Edge::INF_FLOW, used) > 0) {
        used.assign(graph.n_vertices(), false);
    }

    //printf("graph pushed\n");
    //graph.dump();

    if (graph.outflow(start) < 2) {
        printf("NO\n");
        return 0;
    }

    printf("YES\n");

    vector<int> shrek_path(graph.n_vertices(), -1);
    get_path(graph, start, finish, shrek_path);

    //printf("shrek passed\n");
    //graph.dump();

    vector<int> donkey_path(graph.n_vertices(), -1);
    get_path(graph, start, finish, donkey_path);

    //printf("donkey passed\n");
    //graph.dump();

    //printf("SHREK: ");
    for (int v = start; v >= 0; v = shrek_path[v]) {
        printf("%d ", v + 1);
    }
    printf("\n");

    //printf("DONKEY: ");
    for (int v = start; v >= 0; v = donkey_path[v]) {
        printf("%d ", v + 1);
    }
    printf("\n");

    return 0;
}

/*==========================================================================*/

bool get_path(Graph& graph, const int& from, const int& to, vector<int>& path) {

    if (from == to) {
        return true;
    }

    path[from] = -2;

    for (const auto& e : graph.incidence[from]) {

        int v = graph.edges[e].to;

        if (-1 == path[v] && graph.edges[e].flow >= 1LL) {
            if (get_path(graph, v, to, path)) {
                graph.edges[e].flow     -= 1LL;
                graph.edges[e ^ 1].flow += 1LL;
                path[from] = v;
                return true;
            }
        }
    }

    return false;
}


/*==========================================================================*/

Edge::flow_t Graph::find_min_cut(const int &from, const int &to,
                                 set<int>& cut) {

    Edge::flow_t max_flow = push_max_flow(from, to);    

    vector<bool> used(n_vertices(), false);
    
    Edge::flow_t delta = push_flow_dfs(from - 1, to - 1, Edge::INF_FLOW, used);
    if (0 != delta) {
        throw std::runtime_error("could push flow to saturated graph");
    }

    Edge::flow_t total_flow = 0;

    for (int v = 0; v < static_cast<int>(n_vertices()); ++v) {
    
        if (!used[v]) {
            continue;
        }

        for (const auto& e : incidence[v]) {
            
            if (!used[edges[e].to]) {
                total_flow += edges[e].capacity;
                cut.insert(e / 2 + 1);
            }
        }
    }

    if (total_flow != max_flow) {
        throw std::runtime_error("cut capacity and max flow are not equal");
    }

    return max_flow;
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
    } while (0LL != flow_delta);

    return max_flow;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Edge::flow_t Graph::push_flow_bfs(const int& from, const int& to) {

    vector<bool> used(n_vertices(), false);
        
    vector<int> path(n_vertices(), -1);

    queue<int> to_visit;
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

Graph::Graph(const size_t& n_vertices) {
    incidence.resize(n_vertices);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Edge::flow_t Graph::outflow(const int &v) {

    Edge::flow_t outflow = 0;
    for (const auto& e : incidence[v]) {
        outflow += edges[e].flow;
    }

    return outflow;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void Graph::insert_edge(const int &from, const int &to, 
                        const Edge::flow_t& capacity) {

    if (from == to) {
        return;
    }

    for (const auto& e : incidence[from]) {
        if (edges[e].to == to) {
            edges[e].capacity += capacity;
            return;
        }
    }

    edges.emplace_back(from, to, capacity);
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

            printf("\t\tedge %d: %d --(%lld/%lld)--> %d\n",
                   e + 1, edges[e].from + 1, edges[e].flow,
                   edges[e].capacity, edges[e].to + 1);
        }

        printf("\n");
    }

    printf(    "#######################################################\n");
}

/*==========================================================================*/
