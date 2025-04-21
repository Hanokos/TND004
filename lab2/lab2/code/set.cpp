#include "set.h"
#include "node.h"
#include <iostream>
#include <vector>
#include <compare>

// Static member definition
int Set::Node::count_nodes = 0;

/*****************************************************
 * Implementation of the member functions             *
 ******************************************************/


// Definition of static member function get_count_nodes()
int Set::get_count_nodes() {
    return Set::Node::count_nodes;
}
 /*
  * Default constructor: create an empty Set.
  * We allocate two dummy nodes and make them point to each other.
  */
Set::Set() : counter{ 0 } {
    // Create dummy head and tail
    head = new Node{ -1 };  // value is arbitrary, not used
    tail = new Node{ -1 };
    head->next = tail;
    tail->prev = head;
    // No other nodes yet, so counter remains 0.
}

/*
 * Conversion constructor: convert val into a singleton {val}.
 * We call the default constructor to set up the empty list with dummy nodes,
 * then we insert the new node between head and tail.
 */
Set::Set(int val) : Set() {  // delegate to the default constructor
    insert_node(head, val);
    counter = 1;
}

/*
 * Constructor to create a Set from a sorted vector of unique ints.
 * We assume the vector is already sorted increasingly, and the values are unique.
 */
Set::Set(const std::vector<int>& list_of_values) : Set() {  // delegate to the default constructor
    Node* p = head;
    // Insert each value at the end (always before the tail)
    for (int val : list_of_values) {
        insert_node(p, val);  // insert after the last inserted node
        p = p->next;         // move to the newly inserted node
        ++counter;
    }
}

/*
 * Copy constructor: create a new Set as a copy of Set S.
 * We allocate new dummy nodes and then iterate S's list to copy each node.
 */
Set::Set(const Set& S) : Set() {  // delegate to the default constructor
    Node* current_S = S.head->next;
    Node* p = head; // pointer to last inserted node in new list
    while (current_S != S.tail) {
        insert_node(p, current_S->value);
        p = p->next;
        ++counter;
        current_S = current_S->next;
    }
}

/*
 * Transform the Set into an empty set.
 * Remove all nodes from the list except the dummy nodes.
 */
void Set::make_empty() {
    // Start after the dummy head
    Node* current = head->next;
    // Remove nodes until reaching the dummy tail
    while (current != tail) {
        Node* temp = current;
        current = current->next;
        remove_node(temp);
    }
    // After removing all nodes, relink dummy head and tail
    head->next = tail;
    tail->prev = head;
    counter = 0;
}

/*
 * Destructor: deallocate all memory (Nodes) allocated for the list.
 */
Set::~Set() {
    make_empty();  // remove all actual nodes
    delete head;
    delete tail;
}

/*
 * Assignment operator: assign new contents to *this Set, replacing its current content.
 * Uses the copy-and-swap idiom: our parameter S is by value (and so is a copy).
 */
Set& Set::operator=(Set S) {
    // swap the contents of *this with S.
    std::swap(head, S.head);
    std::swap(tail, S.tail);
    std::swap(counter, S.counter);
    return *this;
}

/*
 * Test whether val belongs to the Set.
 * This function does not modify the Set.
 */
bool Set::is_member(int val) const {
    Node* current = head->next;
    // Because the list is sorted, we can stop early if current->value > val.
    while (current != tail && current->value <= val) {
        if (current->value == val)
            return true;
        current = current->next;
    }
    return false;
}

/*
 * Three-way comparison operator.
 * We iterate over both sets simultaneously. Since they are both sorted,
 * a single pass through each list is enough to decide ordering.
 * Return:
 *   - std::partial_ordering::equivalent if both sets contain the same elements,
 *   - std::partial_ordering::less if *this is a proper subset or
 *     comes before S lexicographically,
 *   - std::partial_ordering::greater if *this is a proper superset or
 *     comes after S lexicographically,
 *   - std::partial_ordering::unordered otherwise.
 */
std::partial_ordering Set::operator<=>(const Set& S) const {
    // Determine if *this is a subset of S.
    bool this_subset_S = true;
    for (Node* p = head->next; p != tail; p = p->next) {
        if (!S.is_member(p->value)) {
            this_subset_S = false;
            break;
        }
    }

    // Determine if S is a subset of *this.
    bool S_subset_this = true;
    for (Node* p = S.head->next; p != S.tail; p = p->next) {
        if (!this->is_member(p->value)) {
            S_subset_this = false;
            break;
        }
    }

    if (this_subset_S && S_subset_this)
        return std::partial_ordering::equivalent;   // Both sets are identical.
    else if (this_subset_S && !S_subset_this)
        return std::partial_ordering::less;         // *this is a proper subset of S.
    else if (!this_subset_S && S_subset_this)
        return std::partial_ordering::greater;      // *this is a proper superset of S.
    else
        return std::partial_ordering::unordered;      // They are not comparable by inclusion.
}
/*
 * Test whether Set *this and S represent the same set.
 * We rely on the efficient three-way comparison operator.
 */
bool Set::operator==(const Set& S) const {
    return (*this <=> S) == std::partial_ordering::equivalent;
}

/*
 * Modify Set *this such that it becomes the union of *this with S.
 * We merge the two sorted lists, keeping each distinct element.
 */
Set& Set::operator+=(const Set& S) {
    Node* p1 = head->next;
    Node* p2 = S.head->next;

    // We iterate in a similar manner to merging sorted sequences.
    while (p2 != S.tail) {
        // Advance p1 until we find a node that is not less than p2->value.
        while (p1 != tail && p1->value < p2->value)
            p1 = p1->next;

        // If p1 points to a value greater than p2->value, then insert p2->value.
        if (p1 == tail || p1->value > p2->value) {
            // Insert new node right before p1 (i.e., after p1->prev)
            insert_node(p1->prev, p2->value);
            ++counter;
        }
        // Otherwise, the value is already present.
        p2 = p2->next;
    }
    return *this;
}

/*
 * Modify Set *this such that it becomes the intersection of *this with S.
 * We remove any nodes from *this that do not appear in S.
 */
Set& Set::operator*=(const Set& S) {
    Node* p1 = head->next;
    Node* p2 = S.head->next;

    // Iterate while there are elements in *this and in S.
    while (p1 != tail && p2 != S.tail) {
        if (p1->value < p2->value) {
            // p1->value is not in S, remove it.
            Node* temp = p1;
            p1 = p1->next;
            remove_node(temp);
            --counter;
        }
        else if (p1->value > p2->value) {
            // Advance p2 to catch up.
            p2 = p2->next;
        }
        else {
            // p1->value == p2->value, keep it in the intersection.
            p1 = p1->next;
            p2 = p2->next;
        }
    }
    // Remove any remaining nodes in *this (they are not in S).
    while (p1 != tail) {
        Node* temp = p1;
        p1 = p1->next;
        remove_node(temp);
        --counter;
    }
    return *this;
}

/*
 * Modify Set *this such that it becomes the set difference between *this and S.
 * That is, remove every node from *this that is present in S.
 */
Set& Set::operator-=(const Set& S) {
    // If S is the same object as *this, then removing all elements yields an empty set.
    if (this == &S) {
        make_empty();
        return *this;
    }

    Node* p1 = head->next;
    Node* p2 = S.head->next;

    // Iterate over both lists.
    while (p1 != tail && p2 != S.tail) {
        if (p1->value < p2->value) {
            // p1->value does not appear in S, so keep it.
            p1 = p1->next;
        }
        else if (p1->value > p2->value) {
            // p2->value is less; advance p2.
            p2 = p2->next;
        }
        else {
            // p1->value is found in S.
            Node* temp = p1;  // Save pointer to the node to be removed.
            p1 = p1->next;    // Advance p1 before removal.
            remove_node(temp);
            --counter;
            p2 = p2->next;
        }
    }
    return *this;
}

/*****************************************************
 * Private Member Functions -- Implementation         *
 ******************************************************/

 /*
  * Insert a new Node storing val after the Node pointed by p.
  */
void Set::insert_node(Node* p, int val) {
    // Create new node (you may want to use new Node(val) if you have such a constructor)
    Node* new_node = new Node{ val };

    // Adjust pointers to insert new_node after p
    new_node->next = p->next;
    new_node->prev = p;
    p->next->prev = new_node;
    p->next = new_node;
    // Node count is maintained externally.
}

/*
 * Remove the Node pointed by p.
 */
void Set::remove_node(Node* p) {
    // Adjust the pointers to bypass p
    p->prev->next = p->next;
    p->next->prev = p->prev;
    delete p;
    // Node count is maintained externally.
}

/*
 * Write Set *this to stream os.
 */
void Set::write_to_stream(std::ostream& os) const {
    if (counter == 0) {
        os << "Set is empty!";
    }
    else {
        Node* ptr = head->next;
        os << "{ ";
        while (ptr != tail) {
            os << ptr->value << " ";
            ptr = ptr->next;
        }
        os << "}";
    }
}
