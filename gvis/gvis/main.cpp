//
//  main.cpp
//  gvis
//
//  Created by Marco Grubert on 6/20/15.
//  Copyright (c) 2015 Marco Grubert. All rights reserved.
//

#include <iostream>
#include <set>
using namespace std;

struct Node;
typedef set<Node> TNodeCollection;

struct Node {
    Node(uint32_t ident) : id(ident) {}
    uint32_t id;
    set<uint32_t> neighbors;
};

class Matrix {
public:
    Matrix(size_t width, size_t height,
           int32_t value=numeric_limits<int32_t>::max()) :
        m_width(width)
    {
        m_dist= new int32_t[ width * height ];
        std::fill(m_dist, m_dist+(width*height), value);
    }
    Matrix(Matrix &&rhs) {
        m_dist=nullptr;
        swap(*this, rhs);
    }
    ~Matrix() {
        delete[] m_dist;
    }
    int32_t& at(size_t x, size_t y) {
        return m_dist[y*m_width+x];
    }
private:
    int32_t* m_dist;
    size_t m_width;
    
    Matrix(const Matrix &rhs)=delete;
    Matrix& operator=(const Matrix &rhs)=delete;
    friend void swap(Matrix& lhs, Matrix& rhs) {
        using std::swap;
        swap(lhs.m_dist, rhs.m_dist);
        swap(lhs.m_width, rhs.m_width);
    }
};

void calcAllDistances(const TNodeCollection &allNodes) {
    const size_t numNodes= allNodes.size();
    Matrix m_dist(numNodes, numNodes);
    
    // initialize for identity and neighbors
    for (const Node &n : allNodes) {
        m_dist.at(n.id, n.id)=0;
        for (const auto neighborId: n.neighbors) {
            const Node& neighbor= *allNodes.find(neighborId);
            m_dist.at(n.id, neighbor.id)=1;
        }
    }
    for (const Node &k: allNodes) {
        for (const Node &i: allNodes) {
            for (const Node &j: allNodes) {
                // check for better path via K and store it
                int32_t& currentDist= m_dist.at(i.id, j.id);
                currentDist= min(currentDist,
                                 m_dist.at(i.id, k.id) + m_dist.at(k.id, j.id)
                                 );
            }
        }
    }
    
    // print
    for (uint32_t i=0; i<numNodes-1; ++i) {
        cout << i << "  --> ";
        for (uint32_t j=i; j<numNodes; ++j) {
            cout << m_dist.at(i, j) << " ";
        }
        cout <<endl;
    }
}
//1 let dist be a |V| × |V| array of minimum distances initialized to ∞ (infinity)
//2 for each vertex v
//3    dist[v][v] ← 0
//4 for each edge (u,v)
//5    dist[u][v] ← w(u,v)  // the weight of the edge (u,v)
//6 for k from 1 to |V|
//7    for i from 1 to |V|
//8       for j from 1 to |V|
//9          if dist[i][j] > dist[i][k] + dist[k][j]
//10             dist[i][j] ← dist[i][k] + dist[k][j]
//11         end if

int main(int argc, const char * argv[]) {
    TNodeCollection nodes;
    for (uint32_t id=0; id<4; ++id) {
        Node n(id);
        nodes.insert(move(n));
    }
    calcAllDistances(nodes);
    return 0;
}
