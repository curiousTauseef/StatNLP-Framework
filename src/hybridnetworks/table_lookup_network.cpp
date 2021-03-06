//
// Created by  ngs on 01/08/2018.
//

#include <iostream>
#include "table_lookup_network.h"

TableLookupNetwork::TableLookupNetwork() {

    ptr_children_tmp_ = new std::unordered_map<long, std::list<std::vector<long> *> *>;
    ptr_childrens_size_ = nullptr;
    ptr_children_size_ = nullptr;
    ptr_nodes_ = nullptr;
    ptr_children_ = nullptr;
    ptr_vec_matrix_ = nullptr;
}

TableLookupNetwork::~TableLookupNetwork() {

    delete []ptr_nodes_;

    for(int i =0; i < node_size_; ++i){
        int childrens_size = ptr_childrens_size_[i];
        for(int j=0; j < childrens_size; ++j){
                delete []ptr_children_[i][j];
        }
        delete []ptr_children_[i];
    }
    delete []ptr_children_;

    for(int i =0; i<node_size_; ++i){
        delete []ptr_children_size_[i];
    }
    delete []ptr_children_size_;

    delete []ptr_childrens_size_;
    //delete ptr_children_tmp_;
    if(nullptr != ptr_vec_matrix_){
        for(auto it = ptr_vec_matrix_->begin(); it != ptr_vec_matrix_->end(); ++it){
            std::vector<long> *ptr_vec = (*it);
            if(nullptr != ptr_vec){
                delete ptr_vec;
            }
        }
        delete ptr_vec_matrix_;
    }
}

void TableLookupNetwork::EarlyRelease() {
    //delete ptr_children_temp.
    for(auto it = ptr_children_tmp_->begin(); it!=ptr_children_tmp_->end(); ++it){
        std::list<std::vector<long>*> *ptr_childrens_list = (*it).second;
        if(nullptr == (*it).second){
            continue;
        }
        for(auto itt = ptr_childrens_list->begin(); itt != ptr_childrens_list->end(); ++itt){
            delete (*itt);
        }
        delete ptr_childrens_list;
    }
    delete ptr_children_tmp_;
}

TableLookupNetwork::TableLookupNetwork(int networkId, Instance *ptr_inst, LocalNetworkParam *ptr_param):Network(networkId,ptr_inst,ptr_param) {
    ptr_children_tmp_ = new std::unordered_map<long, std::list<std::vector<long>*>*>;
    ptr_childrens_size_ = nullptr;
    ptr_children_size_ = nullptr;
    ptr_nodes_ = nullptr;
    ptr_children_ = nullptr;
    ptr_vec_matrix_ = nullptr;
}

TableLookupNetwork::TableLookupNetwork(int networkId, Instance *ptr_inst, long *ptr_nodes, int ***ptr_children,
                                       LocalNetworkParam *ptr_param):Network(networkId,ptr_inst,ptr_param) {
    ptr_children_tmp_ = new std::unordered_map<long, std::list<std::vector<long>*>*>;
    ptr_nodes_ = ptr_nodes;
    ptr_children_ = ptr_children;
    ptr_vec_matrix_ = nullptr;

}

TableLookupNetwork::TableLookupNetwork(int networkId, Instance *ptr_inst, TableLookupNetwork *ptr_network,
                                       LocalNetworkParam *ptr_param, int node_size):Network(networkId,ptr_inst,ptr_param) {
    ptr_children_tmp_ = new std::unordered_map<long, std::list<std::vector<long>*>*>;
    ptr_nodes_ = ptr_network->GetAllNodes();
    ptr_children_ = ptr_network->GetAllChildren();
    ptr_children_size_ = ptr_network->GetChildren_Size();
    ptr_childrens_size_ = ptr_network->GetChildrens_Size();
    node_size_ = node_size;
    ptr_vec_matrix_ = nullptr;

}

bool TableLookupNetwork::AddNode(long nodeId) {
    if(ptr_children_tmp_->find(nodeId) != ptr_children_tmp_->end()){
        return false;
    }
    //note that null_list is meaningless.
    //FIXME:
    //std::list<std::vector<long>*>* ;
    ptr_children_tmp_->insert(std::make_pair(nodeId, nullptr));
    return true;
}

/**
 *
 * Add a hyper-edge
 * @param parent
 * @param children
 */
void TableLookupNetwork::AddEdge(long parent, std::vector<long> &children) {
    this->CheckLinkValidity(parent,children);
    //int size_tmp = ptr_children_tmp_->size();
    auto it = ptr_children_tmp_->find(parent);
    std::vector<long> *ptr_children = new std::vector<long>;
    (*ptr_children) = children;
    //if the hyperedge rooted by parent is not found, create hash_map;
    if(it == ptr_children_tmp_->end() || nullptr == (*it).second){
        std::list<std::vector<long>*> *ptr_children_list = new std::list<std::vector<long>*>;
        ptr_children_list->push_back(ptr_children);
        //if the hyperedge is not exist in the hash_map
        if(it == ptr_children_tmp_->end()){
            ptr_children_tmp_->insert(std::make_pair(parent,ptr_children_list));
        } else{
            //if the hyperedge exist and was created by AddNode function, then replaces the nullptr.
            (*it).second = ptr_children_list;
        }
    } else {
        //add this hyperedge to the list, which is root by parent. (*it).second is a pointer to the list.
        (*it).second->push_back(ptr_children);
    }
}

bool TableLookupNetwork::CheckLinkValidity(long parent, std::vector<long>& children) {
    for(int i=0, size = children.size(); i<size; ++i){
        if(children[i] >= parent){
            std::cerr << "This link seems to be invalid:"<<std::endl;
            std::cerr << "Instance ID and Network ID is: "<<GetInstance()->GetInstanceId()<<", "<<GetNetworkID()<<std::endl;
            std::vector<int> c_vec = NetworkIDManager::ToHybridNodeArray(children[i]);
            std::vector<int> p_vec = NetworkIDManager::ToHybridNodeArray(parent);
            std::cout << "children node array is: "<<children[i]<<" "<<c_vec[0]<<","<<c_vec[1]<<","<<c_vec[2]<<","<<c_vec[3]<<std::endl;
            std::cout << "parent node array is: "<<parent<<" "<<p_vec[0]<<","<<p_vec[1]<<","<<p_vec[2]<<","<<p_vec[3]<<std::endl;
        }
    }
    CheckNodeValidity(parent);
    int children_size = children.size();
    for(int i=0; i<children_size; ++i){
        CheckNodeValidity(children[i]);
    }
    return true;
}

void TableLookupNetwork::CheckNodeValidity(long node) {
    if(ptr_children_tmp_->find(node) == ptr_children_tmp_->end()){
        std::cerr << "This node seems to be invalid"<<std::endl;
    }
}

long * TableLookupNetwork::GetAllNodes() {
    return this->ptr_nodes_;
}

int *** TableLookupNetwork::GetAllChildren() {
    return this->ptr_children_;
}

std::unordered_map<long, int>* TableLookupNetwork::FinalizeNodes() {
    node_size_ = ptr_children_tmp_->size();
    ptr_nodes_ = new long[node_size_];
    ptr_childrens_size_ = new int[node_size_];
    ptr_children_size_ = new int*[node_size_];
    ptr_children_ = new int **[node_size_];
    std::vector<long> *ptr_node_vector_temp = new std::vector<long>;
    for(auto it = ptr_children_tmp_->begin(); it!=ptr_children_tmp_->end(); ++it){
        ptr_node_vector_temp->push_back((*it).first);
    }
    //sort the Node ID in the vector.
    std::sort(ptr_node_vector_temp->begin(),ptr_node_vector_temp->end());
    //ptr_nodeId2Index_map_tmp: the fist element is the nodeID,and the second element is node index;
    std::unordered_map<long, int> *ptr_nodeId2Index_map_tmp = new std::unordered_map<long, int>;
    //save the node ID to ptr_nodes_ according to the node index.
    for(int i = 0; i < node_size_; ++i){
        long nodeId = (*ptr_node_vector_temp)[i];
        ptr_nodes_[i] = nodeId;
        ptr_nodeId2Index_map_tmp->insert(std::make_pair(nodeId,i));
    }
    //release the memory;
    delete ptr_node_vector_temp;
    return ptr_nodeId2Index_map_tmp;
}

void TableLookupNetwork::FinalizeNetwork() {
    std::unordered_map<long, int> * ptr_nodeId2Index_map_tmp = FinalizeNodes();
    for(auto it = this->ptr_children_tmp_->begin(); it!=this->ptr_children_tmp_->end(); ++it){
        long parent = (*it).first;
        //get the parent index;
        int parent_index = ptr_nodeId2Index_map_tmp->find(parent)->second;
        //all hypereges root by a parent;
        std::list<std::vector<long>*>* ptr_childrens = (*it).second;
        if(nullptr == ptr_childrens){
                this->ptr_children_[parent_index] = new int*[1];
                this->ptr_children_[parent_index][0] = new int[1];
                this->ptr_children_[parent_index][0][0] = 0;
                this->ptr_childrens_size_[parent_index] = 1;
                this->ptr_children_size_[parent_index] = new int[1];
                ptr_children_size_[parent_index][0] = 0;
        }else{
            //num of hyper-edges rooted by node parent.
            int childrens_size = ptr_childrens->size();
            this->ptr_children_[parent_index] = new int*[childrens_size];
            //the number of hyper edges rooted by node parent
            this->ptr_childrens_size_[parent_index] = childrens_size;
            //ptr_children_size_[parent_index] denotes the array whose value will be the num of nodes in each hyperedge.
            this->ptr_children_size_[parent_index] = new int[childrens_size];
            int hyper_edge_no = 0;
            //iterate each hyperedge rooted by parent node. itt is a vector, which stores the nodes in a hpyeredge.
            for(auto itt = ptr_childrens->begin(); itt!=ptr_childrens->end(); ++itt){
                int size = (*itt)->size();
                int *ptr_node_vector = new int[size];
                //the num of nodes in each hyperedge.
                ptr_children_size_[parent_index][hyper_edge_no] = size;
                //ittt is a node ID
                int node_no = 0;
                for(auto ittt = (*itt)->begin(); ittt != (*itt)->end(); ++ittt){
                    //find the node index
                    long id = (*ittt);
                    int index = ptr_nodeId2Index_map_tmp->find(id)->second;
                    ptr_node_vector[node_no] = index;
                    node_no++;
                }
                this->ptr_children_[parent_index][hyper_edge_no] = ptr_node_vector;
                hyper_edge_no++;
            }
        }
    }
    //FIXME: in original code, the ptr_chilren[parent] is allocated a space with new int[1][0] if it points to nullptr.
    //FIXME: however, this action will be much more complicated in C++ and hence we just leave is as nullptr
    //release the memory;
    delete ptr_nodeId2Index_map_tmp;
    //release the ptr_children_tmp;
    EarlyRelease();
}

int TableLookupNetwork::CountNodes() {
    return node_size_;
}

/**
 *
 * Return the number of hyperedges root by a parent node, which is indexed by node_index (not the nodeID)..
 *
 * @param node_index
 * @return
 */
int TableLookupNetwork::GetChildrens_Size(int node_index) {
    if(node_index > node_size_){
        std::cerr << "Error: the index is bigger than the node size"<<std::endl;
        return -1;
    }
    return ptr_childrens_size_[node_index];
}


/**
 * Return an array, which contains the the number nodes for each hyperedge.
 *
 * @param node_index
 * @return
 */
int *TableLookupNetwork::GetChildren_Size(int node_index) {
    if(node_index > node_size_){
        std::cerr << "Error: the index is bigger than the node size"<<std::endl;
        return nullptr;
    }
    //the pointer stores the num of children for each hyperedge
    return ptr_children_size_[node_index];
}

/**
 *
 * @param k the kth node
 *
 * @return the node ID.
 *
 */
long TableLookupNetwork::GetNode(int k) {
    return this->ptr_nodes_[k];
}

int **TableLookupNetwork::GetChildren(int k) {
    return this->ptr_children_[k];
}

bool TableLookupNetwork::IsRemovded(int k) {
    return false;
}

void TableLookupNetwork::Remove(int k) {

}

bool TableLookupNetwork::IsRoot(int k) {
    if(k == this->CountNodes() - 1){
        return true;
    } else{
        return false;
    }
}

bool TableLookupNetwork::IsLeaf(int k) {
    //no childrens (hypedge) for the leaf node.
    if(0 == this->GetChildrens_Size(k)){
        return true;
    } else{
        return false;
    }
}
/***
 * Check if the node is stored in the children_tmp_
 *
 * @param node
 * @return
 */
bool TableLookupNetwork::IsContain(long node) {
    if(ptr_children_tmp_->end() != ptr_children_tmp_->find(node)){
        return true; /* existed */
    } else{
        return false; /* no existed*/
    }
}

int* TableLookupNetwork::GetChildrens_Size() {
    return ptr_childrens_size_;
}

int** TableLookupNetwork::GetChildren_Size() {
    return ptr_children_size_;
}

int TableLookupNetwork::GetTempNodeSize() {
    return ptr_children_tmp_->size();
}

int TableLookupNetwork::BinarySearch(int array_size, long value) {
    return NetworkIDManager::BinarySearch(this->GetAllNodes(),array_size,value);
}

bool TableLookupNetwork::Contain(TableLookupNetwork *ptr_network) {
    if(this->CountNodes() < ptr_network->CountNodes()){
        std::cout << "size of this is less than the size of network"<<std::endl;
        return false;
    }
    int start = 0;
    /*check each node in the targeting ptr_network*/
    for(int j = 0; j < ptr_network->CountNodes(); ++j){
        long node1 = ptr_network->GetNode(j);
        /*hyberedge no (eg, 0th, 1st, 2nd...), and nodeId in each hyperedge*/
        int **ptr_children1 = ptr_network->GetChildren(j);
        /*number of hyperedge for the j th node*/
        int num_of_hyperedge1 = ptr_network->GetChildrens_Size(j);
        /*number of nodes in each hyperedge, the index of the pointer is the no of the hyperedge*/
        int *ptr_num_of_children1 = ptr_network->GetChildren_Size(j);
        bool found = false;
        for(int k = start; k<this->CountNodes(); ++k){
            long node2 = this->GetNode(k);
            int **ptr_children2 = this->GetChildren(k);
            int num_of_hyperedge2 = this->GetChildrens_Size(k);
            int *ptr_num_of_children2 = this->GetChildren_Size(k);
            if(node1 == node2){
                /*compare the nodes for two hyperedges*/
                for(int c1 = 0; c1 < num_of_hyperedge1; ++c1){
                    std::vector<long> *ptr_child1_nodes_vec  = ptr_network->ToNodes(ptr_children1[c1],ptr_num_of_children1[c1]);
                    bool child_found = false;
                    for(int c2 = 0; c2 < num_of_hyperedge2; ++c2){
                        std::vector<long> *ptr_child2_nodes_vec = this->ToNodes(ptr_children2[c2],ptr_num_of_children2[c2]);
                        if(std::equal(ptr_child1_nodes_vec->begin(),ptr_child1_nodes_vec->end(),ptr_child2_nodes_vec->begin())){
                            child_found = true;
                        }
//                        int temp_size = ptr_child2_nodes_vec->size();
//                        std::vector<int> vec = NetworkIDManager::ToHybridNodeArray((*ptr_child2_nodes_vec)[0]);
//                        std::cout << "c2 unlabeled: " <<vec[0]<<" "<<vec[1]<<" "<<vec[2]<<" "<<vec[3]<<std::endl;
                    }
                    if(false == child_found){
                        std::vector<int> vec = NetworkIDManager::ToHybridNodeArray(node1);
                        std::cout << "Error: NOT FOUND: " <<vec[0]<<" "<<vec[1]<<" "<<vec[2]<<" "<<vec[3]<<std::endl;
                        vec = NetworkIDManager::ToHybridNodeArray((*ptr_child1_nodes_vec)[0]);
                        std::cout << "c1 labeled: " <<vec[0]<<" "<<vec[1]<<" "<<vec[2]<<" "<<vec[3]<<std::endl;
                        //std::cout << "Error: NOT FOUND: " <<node1<<std::endl;
                        return false;
                    }
                }
                found = true;
                start = k;
                break;
            }
        }
        if(false == found){
            std::cout << "Error: NOT FOUND" <<node1<<std::endl;
            return  false;
        }
    }
    return true;
}

std::vector<long>* TableLookupNetwork::ToNodes(int *ptr_ks, int size) {
    if(nullptr == ptr_vec_matrix_){
        ptr_vec_matrix_ = new std::vector<std::vector<long>*>;
    }
    std::vector<long>* ptr_nodes_vec = new std::vector<long>;
    for(int i = 0; i < size; ++i){
        long nodeid = this->GetNode(ptr_ks[i]);
        ptr_nodes_vec->push_back(nodeid);
    }
    ptr_vec_matrix_->push_back(ptr_nodes_vec);
    return ptr_nodes_vec;
}