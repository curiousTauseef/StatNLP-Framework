//
// Created by  ngs on 01/08/2018.
//

#include "network_id_manager.h"

std::vector<int> *NetworkIDManager::ptr_capacity_vec_ = new std::vector<int>{ComParam::FOREST_MAX_HEIGHT + 1,
                                                                             ComParam::FOREST_MAX_WIDTH + 1,
                                                                             ComParam::FOREST_MAX_HEIGHT + 1,
                                                                             ComParam::FOREST_MAX_HEIGHT + 1,
                                                                             ComParam::NETWORK_MAX_DEPTH};
NetworkIDManager::NetworkIDManager() {

}

NetworkIDManager::~NetworkIDManager() {
    delete ptr_capacity_vec_;
}

/**
 *
 * The capacity of the network. Defaultly we have 5 elements and it can be resized.
 * @param cap
 *
 */
void NetworkIDManager::SetCapacity(std::vector<int> cap) {
    int size = cap.size();
    /*resize the size of capacity vetor*/
    ptr_capacity_vec_->resize(size);
    for(int i =0; i < size; ++i){
        (*ptr_capacity_vec_)[i] = cap[i];
    }
}

/**
 *
 * Convert a node array an node ID, which is unique in the graph.
 * @param vec
 * @return
 *
 */
long NetworkIDManager::ToHybridNodeID(std::vector<int> &vec) {
    long value = vec[0];
    for(int i = 1; i < vec.size(); ++i){
        if(vec[i] >= (*ptr_capacity_vec_)[i]){
            std::cout << "Invalid: capacity"<<std::endl;
            return -1;
        }
        value = value * (*ptr_capacity_vec_)[i] + vec[i];
    }
    return value;
}

/**
 *
 * Convert a nodeId to an array.
 *
 * @param nodeId
 * @return
 *
 */
std::vector<int> NetworkIDManager::ToHybridNodeArray(long nodeId) {
    int size = ptr_capacity_vec_->size();
    std::vector<int> result(size,0);
    for(int k=size-1; k>=1; --k){
        long v = nodeId / (*ptr_capacity_vec_)[k];
        result[k] = (int) (nodeId % (*ptr_capacity_vec_)[k]);
        nodeId = v;
    }
    result[0] = (int) nodeId;
    return result;
}

/***
 * Return the index of value in the array ptr_array
 *
 * @param ptr_array
 * @param array_size
 * @param value
 * @return
 */

int NetworkIDManager::BinarySearch(long *ptr_array, int array_size, long value) {
    long first = 0;
    int last = array_size - 1;
    int middle = 0;
    while (first <= last)
    {
        middle = (first + last) / 2; //this finds the mid point
        if (ptr_array[middle] == value) {
            return middle;
        }
        else if (ptr_array[middle]> value) // if it's in the lower half
        {
            last = middle - 1;
        }
        else {
            first = middle + 1;                 //if it's in the upper half
        }
    }
    return -1;  // not found
}