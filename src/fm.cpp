#include <algorithm>
#include <iostream>
#include <cstring>
#include <fstream>
#include <ctime>
#include "fm.h"

using namespace std;

bool floorplane::find_cell_to_move(int* cell, int* gain){
    int i = max_gain_A;
    int j = max_gain_B;
    
    bool found = false;
    while(i>=-Pmax || j>= -Pmax){
        if(i >= j){
            node<int>* n = ListA[i+Pmax].first;
            while(n != nullptr){
                int c = n->cell;
                int new_size_A = A_size - cell_size[c];
                int new_size_B = B_size + cell_size[c];
                float cri = (new_size_A+new_size_B)/10.0;
                if(abs(new_size_A - new_size_B)<cri){
                    found = true;
                    A_size = new_size_A;
                    B_size = new_size_B;
                    *cell = c;
                    *gain = i;
                    break;
                }
                n = n->next;
            }
            if(found == false){
                i = i-1;
            }
            else{
                break;
            }
        }
        else {
            node<int>* n = ListB[j+Pmax].first;
            while(n!=nullptr){
                int c = n->cell;
                int new_size_A = A_size + cell_size[c];
                int new_size_B = B_size - cell_size[c];           
                
                float cri = (new_size_A+new_size_B)/10.0;

                if(abs(new_size_A - new_size_B)<cri){
                    found = true;
                    A_size = new_size_A;
                    B_size = new_size_B;
                    *cell = c;
                    *gain = j;
                    break;
                }
                n = n->next;
            }
            if(found == false){
                j = j -1;
            }
            else{
                break;
            }
        }
    }
    return found;
}
void floorplane::check_critical(int net, int cell, int* Fn, int* Tn, int* possi_update_F, int* possi_update_T){
    *Fn = 0;
    *Tn = 0;
    bool from = position[cell];
    for(int i =0 ; i < net_array[net].size();i++){
        if (position[net_array[net][i]] == from){
            *Fn = *Fn+1;
            if(net_array[net][i] != cell){
                *possi_update_F = net_array[net][i];
            }
        }
        else{
            *possi_update_T = net_array[net][i];
            *Tn = *Tn+1;
        }
    }
}

void floorplane::update_gain(int moved_cell){
    bool F = position[moved_cell];
    bool T;
    if(F == 0){
        T = 1;
        cell_number_A = cell_number_A - 1;
        ListA[cell_gain[moved_cell] + Pmax].remove(&pos_in_bucket[moved_cell]);
    }
    else{
        T = 0;
        cell_number_A = cell_number_A + 1;
        ListB[cell_gain[moved_cell] + Pmax].remove(&pos_in_bucket[moved_cell]);
    }

    for(int i = 0 ; i< cell_array[moved_cell].size(); i++){
        int Fn;
        int Tn;
        int net = cell_array[moved_cell][i];
        int possi_update_F;
        int possi_update_T;
        
        check_critical(net, moved_cell, &Fn, &Tn, &possi_update_F, &possi_update_T);

        if(Tn == 0){
            for(int j = 0 ; j<net_array[net].size();j++){
                int c = net_array[net][j];
                if(cell_locked[c] == false){
                    if(F == 0){
                        ListA[cell_gain[c]+Pmax].remove(&pos_in_bucket[c]);    
                        ListA[cell_gain[c]+1+Pmax].push(c,&pos_in_bucket[c]);
                        max_gain_A = (ListA[max_gain_A+1+Pmax].size != 0)?max_gain_A+1:max_gain_A;
                    }
                    else{
                        ListB[cell_gain[c]+Pmax].remove(&pos_in_bucket[c]);    
                        ListB[cell_gain[c]+1+Pmax].push(c,&pos_in_bucket[c]);
                        max_gain_B = (ListB[max_gain_B+1+Pmax].size != 0)?max_gain_B+1:max_gain_B;
                    }
                    cell_gain[c] += 1;
                }
            }
        }
        else if(Tn ==1){
            if(cell_locked[possi_update_T] == false){
                if(F == 0){
                    ListB[cell_gain[possi_update_T]+Pmax].remove(&pos_in_bucket[possi_update_T]);    
                    ListB[cell_gain[possi_update_T]-1+Pmax].push(possi_update_T,&pos_in_bucket[possi_update_T]);
                    max_gain_B = (ListB[max_gain_B+Pmax].size != 0)?max_gain_B:max_gain_B - 1;
                }
                else{
                    ListA[cell_gain[possi_update_T]+Pmax].remove(&pos_in_bucket[possi_update_T]);    
                    ListA[cell_gain[possi_update_T]-1+Pmax].push(possi_update_T,&pos_in_bucket[possi_update_T]);
                    max_gain_A = (ListA[max_gain_A+Pmax].size != 0)?max_gain_A:max_gain_A - 1;
                }     
                cell_gain[possi_update_T] -= 1;
            }
        }
        Fn -= 1;
        Tn += 1;
        if(Fn == 0){
            for(int j=0; j<net_array[net].size();j++){
                int c = net_array[net][j];
                if(cell_locked[c] == false){
                    if(F == 0){
                        ListB[cell_gain[c]+Pmax].remove(&pos_in_bucket[c]);    
                        ListB[cell_gain[c]-1+Pmax].push(c,&pos_in_bucket[c]);
                        
                        max_gain_B = (ListB[max_gain_B+Pmax].size != 0)?max_gain_B:max_gain_B - 1;
                    }
                    else{
                        ListA[cell_gain[c]+Pmax].remove(&pos_in_bucket[c]);    
                        ListA[cell_gain[c]-1+Pmax].push(c,&pos_in_bucket[c]);
                        
                        max_gain_A = (ListA[max_gain_A+Pmax].size != 0)?max_gain_A:max_gain_A - 1;
                    }
                    cell_gain[c]-=1; 
                }
            }
        }
        else if (Fn == 1){
            if(cell_locked[possi_update_F] == false){
                if(F == 0){
                    ListA[cell_gain[possi_update_F]+Pmax].remove(&pos_in_bucket[possi_update_F]);    
                    ListA[cell_gain[possi_update_F]+1+Pmax].push(possi_update_F,&pos_in_bucket[possi_update_F]);
                    max_gain_A = (ListA[max_gain_A+1+Pmax].size != 0)?max_gain_A+1:max_gain_A;
                }
                else{
                    ListB[cell_gain[possi_update_F]+Pmax].remove(&pos_in_bucket[possi_update_F]);    
                    ListB[cell_gain[possi_update_F]+1+Pmax].push(possi_update_F,&pos_in_bucket[possi_update_F]);
                    max_gain_B = (ListB[max_gain_B+1+Pmax].size != 0)?max_gain_B+1:max_gain_B;
                }
                cell_gain[possi_update_F] += 1;
            }
        }
    }   
    position[moved_cell] = T;
}

int floorplane::calculate_cut_size(void){
    int cut_size = 0 ;
    for(int i = 0; i < net_array.size() ; i++){
        bool a = 0;
        bool b = 0;
        for(int j=0 ; j< net_array[i].size();j++){
            int c = net_array[i][j];
            if(position[c] == 0){
                a = 1;
            }
            else{
                b = 1;
            }
            if(a==1 && b==1){
                cut_size += 1;
                break;
            }
        }
    }
    return cut_size;
}

int floorplane::FM_main(void){
    best_num_A = cell_number_A;
    initial_cut_size = calculate_cut_size();
    best_A_size = A_size;
    best_B_size = B_size;
    clock_t start_time = clock(); 
    clock_t now_time;
    while(1){
        int founded_cell;
        int gain;
        if(find_cell_to_move(&founded_cell, & gain)){
            cell_locked[founded_cell] = true;
            move_order.push_back(founded_cell);
            G += gain;
            update_gain(founded_cell);
            if(G > best_G){
                best_G = G;
                best_num_A = cell_number_A;
                best_move = r;
                best_A_size = A_size;
                best_B_size = B_size;
            }
            //cout << "round " << r+1 << endl;
            //cout << "moved_cell " << founded_cell << endl;
            //cout << "cut size " << initial_cut_size-G << endl;
            //cout << "A: " << cell_number_A << endl;
            //cout << "B: " << cells.size() - cell_number_A << endl;
            //cout << "gain: " << gain << endl;
            //cout << endl << endl;
        }
        else{
            break;
        }
        now_time = clock();
        if((float)(now_time - start_time)/CLOCKS_PER_SEC  > 25){
            break;
        }

        if(G <= -10){
            break;
        }
        r += 1;
    }
    cut_size =  initial_cut_size-best_G;
    return cut_size;
}

void floorplane::initialization(void){
    sort(cells.begin(),cells.end());
    position = new bool[cell_max_idx+1];
    initial_position = new bool[cell_max_idx+1];

    for(int i =0 ;i<cell_max_idx+1;i++){
        position[i] = 1;
        initial_position[i] = 1;
    }

    float cri = (A_size+B_size)/10.0;
    int i = 1;
    while(abs(A_size-B_size)>cri){
        position[cells[i].cell_name] = 0;
        initial_position[cells[i].cell_name] = 0;
        B_size -= cells[i].size;
        A_size += cells[i].size;
        cri = (A_size+B_size)/10.0;
        i = i+1;
        cell_number_A += 1;
    }
}

void floorplane::initialize_net_distribution(void){
    distribution = new NET_DIST[net_array.size()];
    for(int i = 1;i<net_array.size();i++){
        for(int j=0;j<net_array[i].size();j++){
            if(position[net_array[i][j]]==0){
                distribution[i].A = distribution[i].A+1;
            }
            else{
                distribution[i].B = distribution[i].B+1;
            }
        }
    }
}

void floorplane::initialize_cell_gain(void){
    cell_gain = new int[cell_max_idx+1];  
    for(int i = 0 ; i< cell_max_idx+1;i++){
        cell_gain[i] = 0;
    }

    for(int i = 1;i<cell_max_idx+1;i++){
        
        for(int j = 0;j<cell_array[i].size();j++){
            int net = cell_array[i][j];
            int F=0;
            int T=0;
            if(position[i] == 0){
                F = distribution[net].A;
                T = distribution[net].B;
            }
            else{
                F = distribution[net].B;
                T = distribution[net].A;
            }
            if (F == 1){
                cell_gain[i] = cell_gain[i] +1;
            }
            if (T == 0){
                cell_gain[i] = cell_gain[i] -1;
            }
        }
    }
}

void floorplane::initialize_bucket_list(void){
    ListA = new list<int> [2*Pmax+1];
    ListB = new list<int> [2*Pmax+1];
    cell_size = new int[cell_max_idx+1];
    for(int i = 0; i< cell_max_idx+1;i++){
        cell_size[i] = 0;
    }
    pos_in_bucket = new node<int>* [cell_max_idx + 1];
    for(int i = 0;i<cell_max_idx+1;i++){
        pos_in_bucket[i] = nullptr;
    }
  
    max_gain_A = -1;
    max_gain_B = -1;

    for(int i = 0; i< cells.size();i++){
        int c = cells[i].cell_name;
        int g = cell_gain[c];
        bool p = position[c];

        cell_size[c] = cells[i].size;

        if(p==0){
            if(g > max_gain_A){
                max_gain_A = g;
            }
            ListA[g+Pmax].push(c, & pos_in_bucket[c]);
        }
        else{
            if(g > max_gain_B){
                max_gain_B = g;
            }
            ListB[g+Pmax].push(c, &pos_in_bucket[c]);
        }
    }
}

void floorplane:: initialize_cell_locked(void){
    cell_locked = new bool[cell_max_idx+1];
    for(int i = 0 ; i< cell_max_idx +1 ;i++){
        cell_locked[i] = false;
    }
}
void floorplane:: Initialize_for_secod_FM(void){
    for(int i = 0 ; i< cell_max_idx +1 ;i++){
        cell_locked[i] = false;
    }
    for(int i = 0 ; i < best_move ;i++){
        int c = move_order[i];
        initial_position[c] = (initial_position[c] == 0)? 1: 0;
    }
    move_order.clear();

    for(int i = 0; i < cell_max_idx+1;i++){
        position[i] = initial_position[i];
    }
    cell_number_A = best_num_A;
    r = 1;
    G = 0;
    best_G = 0;
    best_move = 0;
    initial_cut_size = cut_size;
    A_size = best_A_size;
    B_size = best_B_size;
}

//void read_cells(char** argv, vector<CELL>* cells,int* B_size, int* cell_max_idx){
void floorplane::read_cells(string filename){
    ifstream f;
    //string path = "../testcases/"; 
    //f.open(path + filename,ios::in);
    f.open(filename,ios::in);
    if(!f){
        printf("can't not read the cell file");
    }
    while(!f.eof()){
        string c;
        int s;
        int c_int;
        f >> c;
        f >> s;
        if(c != ""){
            c_int = stoi(c.erase(0,1));
            CELL cell(c_int,s);
            cells.push_back(cell);
            B_size += s;
            if(c_int>cell_max_idx){
                cell_max_idx = c_int;
            }
        }   
    }
    f.close();
}

//void read_nets(char** argv, vector<int>** cell_array,vector<vector<int>>* net_array, int* Pmax, int cell_max_idx){
void floorplane::read_nets(string filename){
    cell_array = new vector<int> [cell_max_idx+1];
    ifstream f;
    //string path = "../testcases/"; 
    //f.open(path + filename,ios::in);
    f.open(filename,ios::in);
    if(!f){
        printf("can't not read the net file");
    }
    string in;
    int now_net;
    
    vector<int> net_arr;
    
    int p = 0;

    while(!f.eof()){
        f >> in;
        if(in[0] == 'n'){
            net_array.push_back(net_arr);
            net_arr.clear();
            now_net = stoi(in.erase(0,1));
            if(p>Pmax){
                Pmax = p;
            }
            p = 0;
        }
        else if (in[0] == 'c'){
            p = p+1;
            int cell_int = stoi(in.erase(0,1));
            cell_array[cell_int].push_back(now_net);
            net_arr.push_back(cell_int);
        }
    }   
    net_array.push_back(net_arr);
    if(p>Pmax){
        Pmax = p;
    }
    f.close();
}


//void output_the_result(char** argv, int cut_size, int num_A, int num_B, vector<CELL>* cells, bool** initial_position, vector<int>* move_order,int best_move){
void floorplane::output_the_result(string filename){
    for(int i = 0 ; i < best_move ;i++){
        int c = move_order[i];
        initial_position[c] = (initial_position[c] == 0)? 1: 0;
    }
    ofstream f;
    //string path = "../output/";
    //string fullname = filename; 
    //size_t lastindex = fullname.find_last_of("."); 
    //string rawname = fullname.substr(0, lastindex); 


    //f.open(path + rawname + ".out",ios::out);
    f.open(filename,ios::out);
    if(!f){
        printf("can't not open the out file");
    }
    f << "cut_size " << cut_size << endl;
    f << "A " << best_num_A << endl;
    for(int i = 0 ; i<cells.size();i++){
        int c = cells[i].cell_name;
        if(initial_position[c] == 0){
            f << "c" << c << endl;
        }
    }
    f << "B " << cells.size() - best_num_A << endl;
    for(int i = 0 ; i<cells.size();i++){
        int c = cells[i].cell_name;
        if(initial_position[c] == 1){
            f << "c" << c << endl;
        }
    } 
    f.close();
}

template<class T>
void  list<T>::push(T n, node<T>** a){
    node<T>* tmp = new node<T>(n);
    if(empty()){
        *a = tmp;
        first = tmp;
        last = tmp;
    }
    else{
        *a = tmp;
        last->next = tmp;
        tmp->prev = last;
        last = tmp;
    }
    size += 1;
}
template<class T>
void list<T>::remove(node<T>** a){
    if(*a == first && *a == last){
        first = nullptr; 
        last = nullptr;
    }
    else if(*a == first){
        first = first->next;
        first->prev = nullptr;
    }
    else if(*a == last){
        last = last->prev;
        last->next = nullptr;
    }
    else{
        (*a)->prev->next = (*a)->next;
        (*a)->next->prev = (*a)->prev;
    }
    delete *a;
}
template<class T>
bool list<T>::empty(){
    return first == nullptr;
}

floorplane::floorplane(){}



void floorplane:: show_position(void){
    for(int i = 0; i<cell_max_idx+1;i++){
        for(int j = 0 ; j< cells.size();j++){
            if(i == cells[j].cell_name){
                cout << "cell "<< i << " in position" << position[i] << endl; 
            }
        }
    }
}
void floorplane:: show_bucket(void){

    for(int i=-Pmax; i<Pmax+1;i++){
        cout << "bucket "<< i << " ";
        node<int>* n = ListA[i+Pmax].first ;
        while(n != nullptr){
            cout << "  " << n->cell;
            n = n->next;
        }
        cout << endl;
    }
    cout << "max_gain " << max_gain_A << endl;
    cout << endl;
    
    for(int i=-Pmax; i<Pmax+1;i++){
        cout << "bucket "<< i << " ";
        node<int>* n = ListB[i+Pmax].first ;
        while(n != nullptr){
            cout << "  " << n->cell;
            n = n->next;
        }
        cout << endl;
    }
    cout << "max_gain " << max_gain_B << endl;
    cout << endl; 
}
