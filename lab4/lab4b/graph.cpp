/*********************************************
 * file:	~\code4b\graph.h                  *
 * remark:implementation of undirected graphs *
 **********************************************/

#include <iostream>
#include <algorithm>
#include <format>
#include <cassert>     // assert
#include <limits>      // std::numeric_limits
#include <algorithm>   // std::make_heap(), std::pop_heap(), std::push_heap()

#include "graph.h"
#include "dsets.h"

// Note: graph vertices are numbered from 1 -- i.e. there is no vertex zero

// -- CONSTRUCTORS

// Create a graph with n vertices and no vertices
Graph::Graph(int n) : table(n + 1), size{n}, n_edges{0} {
    assert(n >= 1);
}

Graph::Graph(const std::vector<Edge> &V, int n) : Graph{n} {
    for (auto e : V) {
        insertEdge(e);
    }
}

// -- MEMBER FUNCTIONS

// insert undirected edge e
// update weight if edge e is present
void Graph::insertEdge(const Edge &e) {
    assert(e.from >= 1 && e.from <= size);
    assert(e.to >= 1 && e.to <= size);

    auto edge_insertion = [&T = this->table, &n = this->n_edges](const Edge &e1) {
        if (auto it = std::find_if(begin(T[e1.from]), end(T[e1.from]),
                                   [e1](const Edge &ed) { return e1.links_same_nodes(ed); });
            it == end(T[e1.from])) {
            T[e1.from].push_back(e1);  // insert new edge e1
            ++n;                       // increment the counter of edges
        } else {
            it->weight = e1.weight;  // update the weight
        }
    };

    edge_insertion(e);
    edge_insertion(e.reverse());
}

// remove undirected edge e
void Graph::removeEdge(const Edge &e) {
    assert(e.from >= 1 && e.from <= size);
    assert(e.to >= 1 && e.to <= size);

    auto edgeRemoval = [&T = this->table, &n = this->n_edges](const Edge &e1) {
        auto it = std::find_if(begin(T[e1.from]), end(T[e1.from]),
                               [e1](const Edge &ed) { return e1.links_same_nodes(ed); });

        assert(it != end(T[e1.from]));
        T[e1.from].erase(it);  // remove edge e1
        --n;                   // decrement the counter of edges
    };

    edgeRemoval(e);
    edgeRemoval(e.reverse());
}

// Prim's minimum spanning tree algorithm
void Graph::mstPrim() const {
    const int INF = std::numeric_limits<int>::max();
    std::vector<int> dist(size + 1, INF);
    std::vector<int> path(size + 1, 0);
    std::vector<bool> done(size + 1, false);

    using pii = std::pair<int, int>;   // (–weight, vertex)
    std::vector<pii> heap;

    // start at 1
    dist[1] = 0;
    heap.emplace_back(0, 1);          // store –0 == 0
    std::make_heap(heap.begin(), heap.end());

    int total_weight = 0;
    while (!heap.empty()) {
        // extract top (most negative ⇒ smallest actual weight)
        std::pop_heap(heap.begin(), heap.end());
        auto [neg_w, u] = heap.back();
        heap.pop_back();

        if (done[u]) continue;
        done[u] = true;

        int w = -neg_w;               // recover real weight
        total_weight += w;

        // relax neighbors
        for (auto const& e : table[u]) {
            int v = e.to, ew = e.weight;
            if (!done[v] && ew < dist[v]) {
                dist[v] = ew;
                path[v] = u;
                heap.emplace_back(-ew, v);
                std::push_heap(heap.begin(), heap.end());
            }
        }
    }

    // output MST
    std::cout << "Prim's MST edges:\n";
    for (int v = 2; v <= size; ++v) {
        std::cout << std::format("({:2}, {:2}, {:2})\n", path[v], v, dist[v]);
    }
    std::cout << "Total weight: " << total_weight << "\n";
}


void Graph::mstKruskal() const {
    // 1) Collect all unique edges (u < v)
    std::vector<Edge> heap;
    heap.reserve(n_edges);
    for (int u = 1; u <= size; ++u) {
        for (auto const& e : table[u]) {
            if (e.from < e.to) {
                heap.push_back(e);
            }
        }
    }

    // 2) Turn 'heap' into a min-heap by weight:
    //    std::make_heap makes a max-heap by default,
    //    so we give it a comparator that inverts weight.
    auto cmp = [](Edge const& a, Edge const& b) {
        return a.weight > b.weight;
    };
    std::make_heap(heap.begin(), heap.end(), cmp);

    // 3) Prepare union-find
    DSets ds(size);
    int totalWeight = 0;

    std::cout << "Kruskal's MST edges:\n";

    // 4) Repeatedly extract the smallest edge
    while (!heap.empty()) {
        // pop the minimum element into heap.back()
        std::pop_heap(heap.begin(), heap.end(), cmp);
        Edge e = heap.back();
        heap.pop_back();

        int ru = ds.find(e.from);
        int rv = ds.find(e.to);
        if (ru != rv) {
            ds.join(ru, rv);
            std::cout << e << "\n";
            totalWeight += e.weight;
        }
    }

    // 5) Print total
    std::cout << std::format("Total weight = {}\n", totalWeight);
}



// print graph
void Graph::printGraph() const {
    std::cout << std::format("{:-<66}\n", '-');
    std::cout << "Vertex  adjacency lists\n";
    std::cout << std::format("{:-<66}\n", '-');

    for (int v = 1; v <= size; v++) {
        std::cout << std::format("{:4} : ", v);
        for (auto const &e : table[v]) {
            std::cout << std::format("({:2}, {:2}) ", e.to, e.weight);
        }
        std::cout << "\n";
    }
    std::cout << std::format("{:-<66}\n", '-');
}
