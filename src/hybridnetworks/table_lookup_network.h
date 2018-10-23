//
// Created by  ngs on 01/08/2018.
//
#ifndef STATNLP_TABLE_LOOKUP_NETWORK_H
#define STATNLP_TABLE_LOOKUP_NETWORK_H


#include "network.h"


class TableLookupNetwork: public Network{
public:
    TableLookupNetwork();
    TableLookupNetwork(int networkId, Instance *ptr_inst, LocalNetworkParam *ptr_param);
    TableLookupNetwork(int networkId, Instance *ptr_inst, long *ptr_nodes, int ***ptr_children, LocalNetworkParam *ptr_param);
    ~TableLookupNetwork();
    void EarlyRelease();
    bool AddNode(long nodeId);
    void AddEdge(long parent, std::vector<long> &children);
    void CheckLinkValidity(long parentId, std::vector<long> &children);
    void CheckNodeValidity(long nodeId);
    void FinalizeNetwork();
    std::unordered_map<long, int> * FinalizeNodes();
    long *GetAllNodes();
    int*** GetAllChildren();

    int CountNodes() override;

    long GetNode(int k) override;

    //std::vector<int> GetNodeArray(int k) override;

    int **GetChildren(int k) override;

    bool IsRemovded(int k) override;

    void Remove(int k) override;

    bool IsRoot(int k) override;

    bool IsLeaf(int k) override;

    bool IsContain(long node) override;

    int GetChildrens_Size(int node_index) override;

    int *GetChildren_Size(int node_index) override;

    int *GetChildrens_Size();
    int **GetChildren_Size();
    //get the num of nodes during compiling network, this function is for test only.
    int GetTempNodeSize();
protected:
    //the parent_id and its hyperedges, each hyperdeg contains multiple nodes_id.
    std::unordered_map<long, std::list<std::vector<long> *> *> *ptr_children_tmp_;
    //the vector of node Ids.
    // the node IDs.
    long *ptr_nodes_;
    int count_of_nodes_;
    //index matrix: parent node index (not ID), hyperedges rooted by parent node, children node index for a hyperedge.
    int *** ptr_children_;
    //std::vector<std::vector<std::vector<int>>> *ptr_children_;
//    std::unordered_map<long, std::list<long>> *ptr_children_tmp_;
    int node_size_;
    //record the num of hyperedges for each parent, the index of the array is the index of the node.
    int *ptr_childrens_size_;
    //the parent index, the num of node in each hyperedge that is rooted by parent node.
    int **ptr_children_size_;
    int network_size_;

};

#endif //STATNLP_TABLE_LOOKUP_NETWORK_H
