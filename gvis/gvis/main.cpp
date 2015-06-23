//
//  main.cpp
//  gvis
//
//  Created by Marco Grubert on 6/20/15.
//  Copyright (c) 2015 Marco Grubert. All rights reserved.
//

#include <iostream>
#include <set>
#include <string>

using namespace std;

struct Node;
typedef set<Node> TNodeCollection;

class Node {
public:
    Node(uint32_t ident) : m_id(ident) {}
    uint32_t getId() const { return m_id; }
    const set<uint32_t>& neighbors() const { return m_neighbors; }
    set<uint32_t>& neighbors() { return m_neighbors; }
private:
    uint32_t m_id;
    set<uint32_t> m_neighbors;
    friend bool operator< (const Node& lhs, const Node& rhs) {
        return lhs.m_id<rhs.m_id;
    }
};

class DistMatrix {
public:
    DistMatrix() :
    m_dist(nullptr) {
    }
    
    DistMatrix(size_t width, size_t height,
               int32_t value=numeric_limits<int32_t>::max()) :
    m_width(width)
    {
        m_size= width*height;
        m_dist= new int32_t[ m_size ];
        std::fill(m_dist, m_dist+m_size, value);
    }
    DistMatrix(DistMatrix &&rhs) :
    m_size(0), m_dist(nullptr)
    {
        swap(*this, rhs);
    }
    DistMatrix& operator=(DistMatrix rhs) {
        swap(*this, rhs);
        return *this;
    }
    DistMatrix(const DistMatrix &rhs) :
    m_width(rhs.m_width),
    m_size(rhs.m_size)
    {
        m_dist= new int32_t[ m_size ];
        std::copy(rhs.m_dist, rhs.m_dist+rhs.m_size, this->m_dist);
    }
    ~DistMatrix() {
        delete[] m_dist;
    }
    int32_t& at(size_t x, size_t y) {
        return m_dist[y*m_width+x];
    }
    size_t getWidth() { return m_width; }
private:
    int32_t* m_dist;
    size_t m_size;
    size_t m_width;
    
    friend void swap(DistMatrix& lhs, DistMatrix& rhs) {
        using std::swap;
        swap(lhs.m_dist, rhs.m_dist);
        swap(lhs.m_width, rhs.m_width);
    }
};


class GraphHelper {
private:
    DistMatrix m_dist;
public:
    GraphHelper() {}
    // iterator for all, number of nodes, ID for node, neighbor IDs for node
    void calcAllDistances(const TNodeCollection &allNodes) {
        const size_t numNodes= allNodes.size();
        m_dist= DistMatrix(numNodes, numNodes);
        
        // initialize for identity and neighbors
        for (const Node &n : allNodes) {
            m_dist.at(n.getId(), n.getId())=0;
            for (const auto neighborId: n.neighbors()) {
                m_dist.at(n.getId(), neighborId)=1;
            }
        }
        
        // check for better path from I to J via K
        for (const Node &k: allNodes) {
            printDistances();
            for (const Node &i: allNodes) {
                for (const Node &j: allNodes) {
                    int32_t iToK= m_dist.at(i.getId(), k.getId());
                    int32_t kToJ= m_dist.at(k.getId(), j.getId());
                    // ignore INF entries to avoid overflows
                    if (   iToK>= numeric_limits<int32_t>::max()
                        || kToJ>= numeric_limits<int32_t>::max())
                        continue;
                    int32_t& currentDist= m_dist.at(i.getId(), j.getId());
                    currentDist= min( currentDist, iToK+kToJ );
                }
            }
        }
    }
    void printDistances() {
        const bool onlyUR= true;
        
        for (size_t i=0; i<m_dist.getWidth(); ++i) {
            cout << i << "  -->" << string( (onlyUR ? i+1 : 1), '\t');
            for (size_t j= (onlyUR ? i : 0); j<m_dist.getWidth(); ++j) {
                cout << m_dist.at(i, j) << "\t";
            }
            cout <<endl <<endl;
        }
    }
};

int main(int argc, const char * argv[]) {
    TNodeCollection nodes;
    for (uint32_t id=0; id<4; ++id) {
        Node n(id);
        if (id<3)
            n.neighbors().insert(id+1);
        nodes.insert(move(n));
    }
    GraphHelper helper;
    helper.calcAllDistances(nodes);
    return 0;
}
