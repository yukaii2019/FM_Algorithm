#ifndef _FM_H_
#define _FM_H_

#include <string>
#include<vector>
//#include "list.h"
template<class T>
class node{
    public:
        node(T x):cell(x),prev(nullptr),next(nullptr){}
        ~node(){}

    //private:
        T cell;
        node<T>* next;
        node<T>* prev;
};

template<class T>
class list{
    public:
        list():first(nullptr), size(0){}
        void push(T n, node<T>** a);
        void remove(node<T>** a);
        bool empty();

    //private:
        node<T>* first;
        node<T>* last;
        int size;
};

typedef class net_distribute{
    public:
        net_distribute():A(0),B(0){}
        int A;
        int B;
}NET_DIST;

typedef class cell{
    public:
        cell(int c, int s):cell_name(c), size(s){}
        int cell_name;
        int size;
        bool operator < (const cell& rhs){
            return size < rhs.size;
        }
}CELL;

class floorplane{
    public:
    floorplane();
    
    void read_cells(std::string filename);
    void read_nets(std::string filename);
    void output_the_result(std::string filename);

    void initialization(void);
    void initialize_net_distribution(void);
    void initialize_cell_gain(void);
    void initialize_bucket_list(void);
    void initialize_cell_locked(void);

    bool find_cell_to_move(int* cell,int* gain);
    void update_gain(int moved_cell);
    void check_critical(int net, int cell, int* Fn, int* Tn, int* possi_update_F, int* possi_update_T);
    int FM_main(void);
    void Initialize_for_secod_FM(void);
    int calculate_cut_size(void);

    void show_bucket(void);
    void show_position(void);
    //private:
    
    std::vector<CELL> cells;
    std::vector<int>* cell_array;
    std::vector<std::vector<int>> net_array;
    NET_DIST* distribution;
    int* cell_gain;
    int* cell_size;
    bool* cell_locked;
    node<int>** pos_in_bucket;
    int A_size = 0;
    int B_size = 0;
    
    bool* position;
    bool* initial_position;
    
    int Pmax = 0;
    
    list<int>* ListA;
    list<int>* ListB;
    int max_gain_A;
    int max_gain_B;
    
    int cell_number_A = 0;
    std::vector<int> move_order;
    
    int cell_max_idx = -1;

    int r = 1;
    int G = 0;
    int best_G = 0;
    int best_move = 0;
    int best_num_A;
    int initial_cut_size;
    int best_A_size;
    int best_B_size;
    int cut_size;

};

#endif
