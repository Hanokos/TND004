#pragma once

#include <iostream>
#include <vector>
#include <cassert>

//#define TEST_PRIORITY_QUEUE

/**
 * A heap based priority queue where the root is the smallest element -- min heap
 */
template <class Comparable>
class PriorityQueue {
public:
    /**
     * Constructor to create a queue with the given capacity
     */
    explicit PriorityQueue(int initCapacity = 100);

    /**
     * Constructor to initialize a priority queue based on a given vector V
     */
    explicit PriorityQueue(const std::vector<Comparable>& V);

    // Disable copying
    PriorityQueue(const PriorityQueue&) = delete;
    PriorityQueue& operator=(const PriorityQueue&) = delete;

    /**
     * Make the queue empty
     */
    void makeEmpty();

    /**
     * Check is the queue is empty
     * Return true if the queue is empty, false otherwise
     */
    bool isEmpty() const;

    /**
     * Get the size of the queue, i.e. number of elements in the queue
     */
    size_t size() const;

    /**
     * Get the smallest element in the queue
     */
    Comparable findMin();

    /**
     * Remove and return the smallest element in the queue
     */
    Comparable deleteMin();

    /**
     * Add a new element x to the queue
     */
    void insert(const Comparable& x);

private:
    std::vector<Comparable> pq;

    // Auxiliary member functions

    /**
     * Restore the heap property
     */
    void heapify();

    /**
     * Test whether pq is a min heap
     */
    bool isMinHeap() const;

    // Helpers for percolation
    void percolateUp(size_t idx);
    void percolateDown(size_t idx);
};

/* *********************** Member functions implementation *********************** */

// Constructor with capacity
template <class Comparable>
PriorityQueue<Comparable>::PriorityQueue(int initCapacity) {
    pq.reserve(initCapacity);
    assert(isEmpty());  // do not remove this line
}

// Build-heap constructor
template <class Comparable>
PriorityQueue<Comparable>::PriorityQueue(const std::vector<Comparable>& V) : pq{V} {
    heapify();
#ifdef TEST_PRIORITY_QUEUE
    assert(isMinHeap());
#endif
}

// Make empty
template <class Comparable>
void PriorityQueue<Comparable>::makeEmpty() {
    pq.clear();
}

// Check empty
template <class Comparable>
bool PriorityQueue<Comparable>::isEmpty() const {
    return pq.empty();
}

// Size
template <class Comparable>
size_t PriorityQueue<Comparable>::size() const {
    return pq.size();
}

// Find min
template <class Comparable>
Comparable PriorityQueue<Comparable>::findMin() {
    assert(!isEmpty());
    return pq.front();
}

// Delete min
template <class Comparable>
Comparable PriorityQueue<Comparable>::deleteMin() {
    assert(!isEmpty());
    Comparable minItem = pq.front();
    // Move last to root and pop
    pq.front() = pq.back();
    pq.pop_back();
    if (!pq.empty()) {
        percolateDown(0);
    }
#ifdef TEST_PRIORITY_QUEUE
    assert(isMinHeap());
#endif
    return minItem;
}

// Insert
template <class Comparable>
void PriorityQueue<Comparable>::insert(const Comparable& x) {
    pq.push_back(x);
    percolateUp(pq.size() - 1);
#ifdef TEST_PRIORITY_QUEUE
    assert(isMinHeap());
#endif
}

// Heapify (build heap)
template <class Comparable>
void PriorityQueue<Comparable>::heapify() {
    assert(pq.size() > 1);
    // Start from last parent down to root
    for (int i = (int)pq.size() / 2 - 1; i >= 0; --i) {
        percolateDown(i);
    }
}

// Check heap property
template <class Comparable>
bool PriorityQueue<Comparable>::isMinHeap() const {
    size_t n = pq.size();
    for (size_t i = 0; i <= n / 2; ++i) {
        size_t left = 2 * i + 1;
        size_t right = 2 * i + 2;
        if (left < n && pq[i] > pq[left]) return false;
        if (right < n && pq[i] > pq[right]) return false;
    }
    return true;
}

// Percolate up
template <class Comparable>
void PriorityQueue<Comparable>::percolateUp(size_t idx) {
    while (idx > 0) {
        size_t parent = (idx - 1) / 2;
        if (pq[idx] < pq[parent]) {
            std::swap(pq[idx], pq[parent]);
            idx = parent;
        } else {
            break;
        }
    }
}

// Percolate down
template <class Comparable>
void PriorityQueue<Comparable>::percolateDown(size_t idx) {
    size_t n = pq.size();
    while (true) {
        size_t left = 2 * idx + 1;
        size_t right = 2 * idx + 2;
        size_t smallest = idx;
        if (left < n && pq[left] < pq[smallest]) smallest = left;
        if (right < n && pq[right] < pq[smallest]) smallest = right;
        if (smallest != idx) {
            std::swap(pq[idx], pq[smallest]);
            idx = smallest;
        } else {
            break;
        }
    }
}
