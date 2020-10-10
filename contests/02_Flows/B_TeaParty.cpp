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

bool can_drink_tea_k_days(const Edge::flow_t& k, Graph graph);

/*==========================================================================*/

int main() {

    size_t n_employees  = 0,
           n_tea_grades = 0;
    scanf("%lu %lu", &n_employees, &n_tea_grades);

    Graph graph(n_tea_grades, n_employees);
    read_connections(graph);

    can_drink_tea_k_days(5, graph);

    Edge::flow_t min_days = 0,
                 max_days = 100000000,
                 cur_days = 0;
 
	while (min_days + 1 < max_days) {
 
		cur_days = (min_days + max_days) / 2;
 
		if (can_drink_tea_k_days(cur_days, graph)) {
            min_days = cur_days;
		} else {
			max_days = cur_days;
		}
	}

    printf("%lld\n", min_days);

    return 0;
}

/*==========================================================================*/

void read_connections(Graph& graph) {

    std::vector<Edge::flow_t> tea_count(graph.left_size());

    for (auto& count : tea_count) {
        scanf("%lld", &count);
    }

    graph.set_source(tea_count);

    for (int e = 1; e <= graph.right_size(); ++e) {

        size_t n_falourites = 0;
        scanf("%lu", &n_falourites);

        for (size_t k = 0; k < n_falourites; ++k) {
            int grade = 0;
            scanf("%d", &grade);
            graph.connect(grade, e, Edge::INF_FLOW);
        }
    }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

bool can_drink_tea_k_days(const Edge::flow_t& k, Graph graph) {

    graph.set_sink(vector<Edge::flow_t>(graph.right_size(), k));

    Edge::flow_t max_flow = graph.push_max_flow(1, graph.n_vertices());

    Edge::flow_t min_sink = graph.min_sink();
    return (min_sink >= k);
}

/*==========================================================================*/

Edge::flow_t Graph::push_max_flow(const int& from, const int& to) {

    return edmonds_karp(from - 1, to - 1);
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
