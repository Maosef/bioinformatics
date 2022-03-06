#include <iostream>
#include <stdio.h>
#include <math.h>

#include <sdsl/vectors.hpp>
#include <sdsl/bit_vectors.hpp>

using namespace std;
using namespace sdsl;


class sparse_array {
    bit_vector bitvector;
    rank_support_v<1> b_rank;

    vector<string> elements;
    char *buffer;
    unsigned int total_count = 0;

    public:
        // sparse_array () {
            
        // }
        void create(uint64_t size) {
            bitvector = bit_vector(size, 0);
            b_rank = rank_support_v<1>(&bitvector);
        }
        void append(string elem, uint64_t pos) {
            elements.push_back(elem);
            bitvector[pos] = 1;
        }
        bool get_at_rank(uint64_t r, std::string& elem) {
            // gets r-th present item in the array
            if (num_elem() >= r) {
                // *elem = elements[r-1];
                elem = elements[r-1];
                return true;
            } else 
                return false;
        }
        bool get_at_index(uint64_t r, std::string& elem) {
            if (bitvector[r] == 1) {
                return get_at_rank(b_rank(r+1), elem);
            } else
                return false;
        }
        uint64_t num_elem_at(uint64_t r) {
            return b_rank(r+1); // rank uses 1 indexing
        }
        uint64_t size() {
            return bitvector.size();
        }
        uint64_t num_elem() {
            return b_rank(size());
        }
        void save(string& fname) {
            store_to_file(bitvector, "bvector_" + fname);
            store_to_file(b_rank, fname);

            // save elements
            buffer = serialize(elements, &total_count);
        }
        void load(string& fname) {
            // bitvector = bit_vector();
            // b_rank = rank_support_v<1>();
            load_from_file(bitvector, "bvector_" + fname);
            load_from_file(b_rank, fname);

            deserialize(elements, buffer, total_count);

            cout << bitvector << endl;
            printf("reloaded.\n");
        }

        
char* serialize(vector<string> &v, unsigned int *count) {
    unsigned int total_count = 0;

    for(int i = 0; i < v.size(); i++ ) {
        // cout << v[i]<< endl;
        total_count += v[i].length() + 1;
    }

    char *buffer = new char[total_count];

    int idx = 0;

    for(int i = 0; i < v.size(); i++ ) {
        string s = v[i];
        for (int j = 0; j < s.size(); j ++ ) {
            buffer[idx ++] = s[j];
        }
        buffer[idx ++] = 0;
    }

    *count  = total_count;

    return buffer;
}

void deserialize(vector<string> &restore,  char* buffer, int total_count) {
    for(int i = 0; i < total_count; i ++ ) {
        const char *begin = &buffer[i];
        int size = 0;
        while(buffer[i++]) {
            size += 1;
        }
        restore.push_back(string(begin, size));
    }
}

};



int main(){

    sparse_array s;
    s.create(10);

    s.append("foo", 1);
    s.append("bar", 5);
    s.append("baz", 9);

    // save and load
    // string fname = "savefile";
    // s.save(fname);
    // s.load(fname);

    string p1;
    s.get_at_rank(1, p1);
    cout << "pointer result " << p1 << endl;

    if (s.get_at_index(5, p1))
        cout << "pointer result " << p1 << endl;
    else
        cout << "no element" << endl;
    
    if (s.get_at_index(0, p1))
        cout << "pointer result " << p1 << endl;
    else
        cout << "no element" << endl;

    // performance test: print N, sparsity, speed of "get_at_rank", "get_at_index"; memory
    // first fix sparsity, then fix N
    
    int num_ops = 10;
    for (int N : {1000, 10000, 100000, 1000000, 10000000}) {
    // for (int N : {10000}) {
        for (float sparsity : {.01, .05, .1, .25, .5}) {
        
        s.create(N);

        // put in objects randomly
        int num_elements = sparsity*N;
        for (int i=0; i<num_elements; i++) {
            int idx = rand() % N;         // v1 in the range 0 to N-1
            // int idx = i;
            s.append("element", idx);
        }
        
        // printf("time get_rank\n");
        // time get_at_rank
        string str;
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        for (int i=0; i<num_ops; i++) {
            int idx = rand() % num_elements;
            s.get_at_rank(idx, str);
        }
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        int time_1 = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

        // printf("time get_index\n");
        // time get_at_index
        begin = std::chrono::steady_clock::now();
        for (int i=0; i<num_ops; i++) {
            int idx = rand() % N;
            s.get_at_index(idx, str);
        }
        end = std::chrono::steady_clock::now();
        int time_2 = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();


        printf("N=%d, sparsity=%f, time for get_at_rank=%d, time for get_at_index=%d\n", N, sparsity, time_1, time_2);
    }
    }

}
