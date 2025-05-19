/*********************************************
 * file:	~\code4a\digraph.cpp              *
 * remark: implementation of directed graphs  *
 **********************************************/

#include <iostream>
#include <algorithm>
#include <limits>  //std::numeric_limits
#include <vector>
#include <cassert>
#include <queue>
#include <format>

#include "digraph.h"

// Note: graph vertices are numbered from 1 -- i.e. there is no vertex zero

// -- CONSTRUCTORS

Digraph::Digraph(int n)
    : table(n + 1)  // slot zero not used
    , size{n}       // number of verices
    , n_edges{0}
    , dist(n + 1)
    , path(n + 1)
    , done(n + 1) {
    assert(n >= 1);
    // Note: graph vertices are numbered from 1 -- i.e. there is no vertex zero
}

// Create a digraph with n vertices and the edges in V
Digraph::Digraph(const std::vector<Edge>& V, int n) : Digraph{n} {
    for (auto e : V) {
        insertEdge(e);
    }
}

// -- MEMBER FUNCTIONS

// insert directed edge e = (u, v, w)
// update weight w if edge (u, v) is present
void Digraph::insertEdge(const Edge& e) {
    assert(e.from >= 1 && e.from <= size);
    assert(e.to >= 1 && e.to <= size);

    // Check if edge e already exists
    if (auto it = std::find_if(begin(table[e.from]), end(table[e.from]),
                               [e](const Edge& ed) { return e.links_same_nodes(ed); });
        it == end(table[e.from])) {
        table[e.from].push_back(e);  // insert new edge e
        ++n_edges;
    } else {
        it->weight = e.weight;  // update the weight
    }
}

// remove directed edge e
void Digraph::removeEdge(const Edge& e) {
    assert(e.from >= 1 && e.from <= size);
    assert(e.to >= 1 && e.to <= size);

    auto it = std::find_if(begin(table[e.from]), end(table[e.from]),
                           [e](const Edge& ed) { return e.links_same_nodes(ed); });

    assert(it != end(table[e.from]));
    table[e.from].erase(it);
    --n_edges;
}

// construct unweighted single source shortest path-tree for start vertex s
void Digraph::uwsssp(int s) const {
    assert(s >= 1 && s <= size);

    const int INF = std::numeric_limits<int>::max();
    // initialize all distances to INF and paths to 0
    std::fill(dist.begin(), dist.end(), INF);
    std::fill(path.begin(), path.end(), 0);

    std::queue<int> q;
    dist[s] = 0;
    q.push(s);

    // BFS traversal
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (auto& e : table[v]) {
            int u = e.to;
            if (dist[u] == INF) {
                dist[u] = dist[v] + 1;
                path[u] = v;
                q.push(u);
            }
        }
    }
}

// construct positive weighted single source shortest path-tree for start vertex s
// Dijkstra's algorithm
void Digraph::pwsssp(int s) const {
    assert(s >= 1 && s <= size);

    const int INF = std::numeric_limits<int>::max();
    // initialize distances, paths, and done flags
    std::fill(dist.begin(), dist.end(), INF);
    std::fill(path.begin(), path.end(), 0);
    std::fill(done.begin(), done.end(), false);

    // min‐heap storing (distance, vertex)
    using PII = std::pair<int, int>;
    std::priority_queue<PII, std::vector<PII>, std::greater<PII>> pq;

    dist[s] = 0;
    pq.push({ 0, s });

    while (!pq.empty()) {
        auto [d, v] = pq.top();
        pq.pop();
        if (done[v]) continue;
        done[v] = true;

        for (auto& e : table[v]) {
            int u = e.to;
            int w = e.weight;
            if (!done[u] && d + w < dist[u]) {
                dist[u] = d + w;
                path[u] = v;
                pq.push({ dist[u], u });
            }
        }
    }
}


// print graph
void Digraph::printGraph() const {
    std::cout << std::format("{:-<66}\n", '-');
    std::cout << "Vertex  adjacency lists\n";
    std::cout << std::format("{:-<66}\n", '-');

    for (int v = 1; v <= size; ++v) {
        std::cout << std::format("{:4} : ", v);
        for (auto const& e : table[v]) {
            std::cout << std::format("({:2}, {:2}) ", e.to, e.weight);
        }
        std::cout << "\n";
    }
    std::cout << std::format("{:-<66}\n", '-');
}

// print shortest path tree for s
void Digraph::printTree() const {
    std::cout << std::format("{:-<22}\n", '-');
    // std::cout << "----------------------\n";
    std::cout << "vertex    dist    path\n";
    std::cout << std::format("{:-<22}\n", '-');
    // std::cout << "----------------------\n";

    for (int v = 1; v <= size; ++v) {
        std::cout << std::format("{:4} : {:6} {:6}\n", v,
                                 ((dist[v] == std::numeric_limits<int>::max()) ? -1 : dist[v]),
                                 path[v]);
    }
    std::cout << std::format("{:-<22}\n", '-');
    // std::cout << "----------------------\n";
}
// print shortest path from source to t and the corresponding path length
void Digraph::printPath(int t) const {
    assert(t >= 1 && t <= size);
    const int INF = std::numeric_limits<int>::max();

    // if unreachable
    if (dist[t] == INF) {
        std::cout << "No path to " << t << "\n";
        return;
    }

    // collect vertices on the path by walking back via `path[]`
    std::vector<int> seq;
    for (int v = t; v != 0; v = path[v]) {
        seq.push_back(v);
    }
    // reverse to get source → … → t
    std::reverse(seq.begin(), seq.end());

    // print it
    std::cout << "Path: ";
    for (size_t i = 0; i < seq.size(); ++i) {
        std::cout << seq[i];
        if (i + 1 < seq.size())
            std::cout << " -> ";
    }
    std::cout << "  (length: " << dist[t] << ")\n";
}
