

#include <iostream>
#include <stdio.h>
#include <math.h>

#include <sdsl/vectors.hpp>
#include <sdsl/bit_vectors.hpp>

#include <chrono>


using namespace std;
using namespace sdsl;


class my_select_support {
    bit_vector bitvector;
    rank_support_v<1> b_rank;

    public:
        my_select_support (bit_vector * b) {
            bitvector = *b;
            b_rank = rank_support_v<1>(b);
        }
        uint64_t select1(uint64_t i) {
            
            int l = 0;
            int r = bitvector.size()-1;
            int mid = (l+r)/2;

            while (l<r) {
                int rank = b_rank(mid+1);
                if (rank < i)
                    l = mid+1;
                else
                    r = mid;

                mid = (l+r)/2;
            }

            return mid;
        };
        uint64_t overhead() {
            return size_in_bytes(b_rank);
        }
        void save(string& fname) {
            store_to_file(bitvector, "bvector_" + fname);
            store_to_file(b_rank, fname);
        }
        void load(string& fname) {
            bitvector = bit_vector();
            b_rank = rank_support_v<1>();
            load_from_file(bitvector, "bvector_" + fname);
            load_from_file(b_rank, fname);

            cout << bitvector << endl;
            printf("reloaded. size is %d\n", overhead());
        }
};


int main(){
    bit_vector b = {1,1,0,1,0,0,1};
    cout << b << endl;

    my_select_support s (&b);
    bit_vector::select_1_type b_sel(&b);

    cout << "overhead in bits: " << s.overhead() << endl;
    // cout << "select 5: " << s.select1(5) << endl;

    for (size_t i=1; i <= 4; i++)
        printf("computed select of %d = %d. actual select = %d\n", i, s.select1(i), b_sel(i));

    // save and load
    // string fname = "savefile.sdsl";
    // s.save(fname);
    // s.load(fname);

    int i = 1;
    // segfault
    printf("computed select of %d = %d. actual select = %d\n", i, s.select1(i), b_sel(i));

    // for (size_t i=1; i <= 4; i++)
    //     printf("computed select of %d = %d. actual select = %d\n", i, s2.select1(i), b_sel(i));

    // performance test: print N, time to do 1000 select operations, overhead
    int num_ops = 10000;
    for (int N : {1000, 10000, 100000, 1000000, 10000000}) {
        
        b = bit_vector(N, 0);

        // put in N/2 1s randomly
        int num_ones = N/2;
        for (int i=0; i<num_ones; i++) {
            int idx = rand() % N;         // v1 in the range 0 to N-1
            b[idx] = 1;
        }

        my_select_support b_select (&b);

        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

        for (int i=0; i<num_ops; i++) {
            int n = rand() % num_ones;
            b_select.select1(n);
        }
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

        // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[mics]" << std::endl;
        int time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        printf("N=%d, time=%d, overhead=%d\n", N, time, b_select.overhead());
    }

}
