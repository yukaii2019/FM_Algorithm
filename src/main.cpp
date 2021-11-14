#include <iostream>
#include <ctime>
#include "fm.h"

using namespace std;

int main (int argc, char* argv[]){

    clock_t start  = clock();
    clock_t now;
    clock_t in_start;
    clock_t in_finish;
    clock_t out_start;
    clock_t out_finish;
    clock_t exe_start;
    clock_t exe_finish;


    floorplane fp;
    int last_cut_size;
    int new_cut_size;

    /*===============input =================*/
    in_start = clock();

    fp.read_cells(string(argv[2]));
    fp.read_nets(string(argv[1]));

    in_finish = clock();
    /*======================================*/

    /*===============execution===============*/
    exe_start = clock();

    fp.initialize_cell_locked();  
    fp.initialization();
    fp.initialize_net_distribution();
    fp.initialize_cell_gain();
    fp.initialize_bucket_list(); 
    last_cut_size = fp.FM_main();
    cout << "cut size " << last_cut_size << endl;
    for(int k = 0; k < 20;k= k +1){
        now = clock();
        if((float)(now - start)/CLOCKS_PER_SEC > 270){
            break;
        }
        fp.Initialize_for_secod_FM();
        fp.initialize_net_distribution();
        fp.initialize_cell_gain();
        fp.initialize_bucket_list(); 
        new_cut_size = fp.FM_main();
        cout << "cut size " << new_cut_size << endl;
        if(new_cut_size == last_cut_size){
            break;
        }
        last_cut_size = new_cut_size;
    }
    exe_finish = clock();
    /*=======================================*/

    /*===============output==================*/
    out_start = clock();

    fp.output_the_result(string(argv[3]));
    
    out_finish = clock();
    /*=======================================*/

    cout << "IO time is " << (float)(out_finish - out_start - in_start + in_finish)/CLOCKS_PER_SEC << " seconds" << endl;
    cout << "execution time is " << (float)(exe_finish-exe_start)/CLOCKS_PER_SEC << " seconds" << endl;
    cout << "best cut is " << last_cut_size << endl; 
    return 0;
}

























