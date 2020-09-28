// (c) Shishqa, 2020
/*===========================================================================*/

#include <cstdint>
#include <cstdio>
#include <climits>
#include <vector>
#include <queue>
#include <set>

using std::vector;
using std::queue;
using std::set;

/*===========================================================================*/

typedef int64_t flow_t;
typedef int32_t vertex_t;
typedef int32_t edge_t;

static constexpr flow_t INF_FLOW = 0x7FFFFFFFFFFFFFFF;

struct Edge {
    vertex_t from, to;
    flow_t capacity;
    flow_t flow;
};

/*---------------------------------------------------------------------------*/

typedef vector<vector<edge_t>> Graph;
typedef vector<Edge> EdgeVec;

struct MaxFlowMeta {
    EdgeVec edges;
    Graph   graph;
};

/*---------------------------------------------------------------------------*/

struct Cut {
    set<edge_t> edges;
    flow_t total_capacity;
};

/*===========================================================================*/

void read_connections(MaxFlowMeta& meta);

flow_t find_max_flow(const vertex_t from, const vertex_t to, MaxFlowMeta& meta);

Cut find_min_cut(const vertex_t from, const vertex_t to, MaxFlowMeta& meta);

/*---------------------------------------------------------------------------*/

flow_t push_flow_dfs(const vertex_t from, const vertex_t to, const flow_t min_cap, 
                     vector<bool>& used, MaxFlowMeta& meta);

/*---------------------------------------------------------------------------*/

flow_t saturate_path(const vertex_t from, const vertex_t to, 
                     const vector<vertex_t>& path, MaxFlowMeta& meta);

flow_t push_flow_bfs(const vertex_t from, const vertex_t to, MaxFlowMeta& meta);

/*===========================================================================*/

int main() {
   
    MaxFlowMeta meta = {};

    read_connections(meta);

    Cut min_cut = find_min_cut(0, static_cast<vertex_t>(meta.graph.size() - 1), meta);

    printf("%lu %lu\n", min_cut.edges.size(),
                        min_cut.total_capacity);

    for (const auto& e : min_cut.edges) {
        printf("%d ", (e / 2) + 1);
    }

    return 0;
}

/*===========================================================================*/

flow_t find_max_flow(const vertex_t from, const vertex_t to, MaxFlowMeta& meta) {

    flow_t delta    = 0,
           max_flow = 0;

    vector<bool> used(meta.graph.size(), false);
    do {
        delta = push_flow_dfs(from, to, INF_FLOW, used, meta);
        max_flow += delta;
        used.assign(meta.graph.size(), false);
    } while (delta != 0);

    return max_flow;
}

/*---------------------------------------------------------------------------*/

Cut find_min_cut(const vertex_t from, const vertex_t to, MaxFlowMeta& meta) {
    
    Cut min_cut = {};

    min_cut.total_capacity = find_max_flow(from, to, meta);

    vector<bool> used(meta.graph.size(), false);
    push_flow_dfs(from, to, INF_FLOW, used, meta);

    for (const auto& v : used) {
        
        if(!used[v]) {
            continue;
        }

        for (const auto& e : meta.graph[v]) {
            if (!used[meta.edges[e].to]) {
                min_cut.edges.insert(e); 
            }
        }
    }

    return min_cut;
}

/*---------------------------------------------------------------------------*/

flow_t push_flow_bfs(const vertex_t from, const vertex_t to, MaxFlowMeta& meta) {

    vector<edge_t> path(meta.graph.size(), -1);

    vector<bool> used(meta.graph.size(), false);
    queue<vertex_t> to_visit;

    to_visit.push(from);

    while (!to_visit.empty()) {

        vertex_t curr_vertex = to_visit.front();
                               to_visit.pop();

        used[curr_vertex] = true;
       
        if (curr_vertex == to) {
            return saturate_path(from, to, path, meta);
        }

        for (const auto& e : meta.graph[curr_vertex]) {
            if (!used[meta.edges[e].to] && meta.edges[e].capacity > meta.edges[e].flow) {
                to_visit.push(meta.edges[e].to);
                path[meta.edges[e].to] = e;
            }
        }
    }

    return 0;
}

/*---------------------------------------------------------------------------*/

flow_t saturate_path(const vertex_t from, const vertex_t to, 
                     const vector<edge_t>& path,
                     MaxFlowMeta& meta) {
    
    flow_t flow = INF_FLOW;

    vertex_t it_vertex = to;
    while (it_vertex != from) {
        auto edge_id = path[it_vertex];

        flow = std::min(flow, meta.edges[edge_id].capacity - meta.edges[edge_id].flow);

        it_vertex = meta.edges[edge_id].from;
    }

    it_vertex = to;
    while (it_vertex != from) {
        auto edge_id = path[it_vertex];

        meta.edges[edge_id].flow     += flow;
        meta.edges[edge_id ^ 1].flow -= flow;

        it_vertex = meta.edges[edge_id].from;
    }

    return flow;
}

/*---------------------------------------------------------------------------*/

flow_t push_flow_dfs(const vertex_t from, const vertex_t to, const flow_t min_cap, 
                     vector<bool>& used, MaxFlowMeta& meta) {

    if (from == to) {
        return min_cap;
    }

    used[from] = true;

    for (const auto& e : meta.graph[from]) {

        auto u = meta.edges[e].to;

        if (!used[u] && meta.edges[e].flow < meta.edges[e].capacity) {

            flow_t delta = push_flow_dfs(
                    u, to, 
                    std::min(min_cap, meta.edges[e].capacity - meta.edges[e].flow),
                    used, meta
            );

            if (delta > 0) {
                meta.edges[e].flow     += delta;
                meta.edges[e ^ 1].flow -= delta;
                return delta;
            }
        }
    }

    return 0;
}

/*---------------------------------------------------------------------------*/

void read_connections(MaxFlowMeta& meta) {

    size_t n_vertexes = 0,
           n_edges    = 0;

    if (2 != scanf("%lu %lu", &n_vertexes, &n_edges)) {
        throw 1;
    }

    meta.edges.resize(4 * n_edges);
    meta.graph.resize(n_vertexes);

    for (edge_t e = 0; e < static_cast<edge_t>(n_edges); ++e) {    

        Edge current_edge = {};
        
        if (3 != scanf("%d %d %ld", 
                       &current_edge.from,
                       &current_edge.to,
                       &current_edge.capacity)) {
            throw 1;
        }

        current_edge.flow = 0;
        --current_edge.from;
        --current_edge.to;

        Edge back_edge = {current_edge.to, 
                          current_edge.from, 
                          current_edge.capacity, 
                          0};

        meta.edges[2 * e]     = current_edge;
        meta.edges[2 * e + 1] = back_edge;

        meta.graph[current_edge.from].push_back(2 * e);
        meta.graph[current_edge.to].push_back(2 * e + 1);
    }
}

/*===========================================================================*/
