//  bit_vector b;
//  // do some stuff with b
//  rank_support r(&b); // let r access b via a pointer
//  auto x = r(i); // x now holds the value of rank1(b, i);
//                 // I'm using r(i) (operator()) as a shorthand
//                 // for rank1(.) as defined below.

// uint64_t rank1(uint64_t i) : Returns the number of 1s in the underlying bit-vector up to position i (inclusive).
// uint64_t overhead() : Returns the size of the rank data structure (in bits) that is required to support constant-time rank on the current bitvector.
// save(string& fname) : Saves the rank data structure for this bit vector to the file fname (your bit vector should also have a save() function).
// load(string& fname) : Loads the rank data structure for this bit vector from the file fname (your bit vector should also have a load() function).


#include <iostream>
#include <stdio.h>
#include <math.h>

#include <sdsl/vectors.hpp>
#include <sdsl/bit_vectors.hpp>

#include <bitset>

using namespace std;
using namespace sdsl;


class my_rank_support {
    bit_vector bitvector;
    int_vector<> superblocks;
    int_vector<> blocks;
    int superblock_size;
    int block_size;
    int n;

    public:
        my_rank_support (bit_vector * b) {

            bitvector = *b;
            n = bitvector.size();
            superblock_size = pow(log2(n),2)/2;
            block_size = log2(n)/2;
            
            int num_superblocks = n/superblock_size;
            int num_blocks = n/block_size;
            int blocks_per_superblock = superblock_size/block_size;

            superblocks = int_vector<>(num_superblocks, 0, superblock_size);
            blocks = int_vector<>(num_blocks, 0, block_size);

            int block_prefix_sum = 0;
            int prefix_sum = 0;
            
            int cur_block_size = 0;
            int cur_superblock_size = 0;
            
            int cur_block_idx = 1;
            int cur_superblock_idx = 1;

            for (int i = 0; i < n; i++) {
                block_prefix_sum += bitvector[i];
                prefix_sum += bitvector[i];

                cur_block_size += 1;
                cur_superblock_size += 1;

                if (cur_superblock_size == superblock_size && cur_superblock_idx < num_superblocks) {
                    superblocks[cur_superblock_idx] = prefix_sum;
                    cur_superblock_idx += 1;
                    cur_superblock_size = 0;
                }

                if (cur_block_size == block_size and cur_block_idx < num_blocks) {
                    blocks[cur_block_idx] = block_prefix_sum - superblocks[cur_superblock_idx-1];
                    cur_block_idx += 1;
                    cur_block_size = 0;
                }
            }

            // cout << "superblocks: " << superblocks << endl;
            // cout << "blocks: " << blocks << endl;

            printf("s size = %d, num s = %d, b size = %d, num b = %d, total size = %d\n",
                superblock_size, num_superblocks, block_size, num_blocks, superblock_size*num_superblocks + block_size*num_blocks);

            // printf("theoretical overhead: %f + %f = %f\n", 
            // 2*N/log2(N), 2*N*log2(log2(N))/log2(N), 2*N/log2(N) + 2*N*log2(log2(N))/log2(N));
            printf("theoretical overhead: %f + %f = %f\n", 
                2*n/log2(n), 2*n*log2(log2(n))/log2(n), 2*n/log2(n) + 2*n*log2(log2(n))/log2(n));

            printf("actual size: %d\n", 8*(size_in_bytes(superblocks) + size_in_bytes(blocks)));


        }
        uint64_t rank1(uint64_t i) {

            i -= 1;
            int ans = superblocks[(i/superblock_size)] + blocks[(i/block_size)];

            // printf("idx: %lu, s idx: %lu, b idx: %lu\n", 
            //     i, i/superblock_size, i/block_size);
            // cout << "s: " << superblocks[(i/superblock_size)] << ", b: " << blocks[(i/block_size)] << endl;
            // printf("partial ans: %d\n", ans);

            // find the start of the containing block, extract the bits and count
            int q = i / block_size;
            int block_start = q * block_size;

            // value_type get_int(size_type idx, const uint8_t len=64) const;
            uint64_t block = bitvector.get_int(block_start, i+1-block_start);

            return ans + popcount(block);

            // uint64_t masked_b = *bitvector.data(); // wrong: we can't fit any bitvector in 64 bits
            // bit_vector masked_b = bitvector;

            // // cout << "bits: " << std::bitset<64>(masked_b) << endl;
            // cout << "bits: " << masked_b << endl;

            // masked_b = masked_b << block_start;
            // masked_b = masked_b >> (block_start + num_bits_to_right);

            // // cout << "masked bitvector: " << bit_vector(masked_b) << endl;
            // std::cout << "popcount( " << std::bitset<64>(masked_b) << " ) = "
            //       << std::popcount(masked_b) << '\n';

            // // for (int j = block_start; j < i + 1; j++)
            // //     ans += bitvector[j];
                
            // return ans + popcount(masked_b);
            // return ans;
        };
        uint64_t overhead() {


            return 8*(size_in_bytes(superblocks) + size_in_bytes(blocks));
        }
        void save(string& fname) {
            store_to_file(bitvector, "bitvector_" + fname);
            store_to_file(superblocks, "superblocks_" + fname);
            store_to_file(blocks, "blocks_" + fname);
        }
        void load(string& fname) {
            // bitvector = bit_vector();
            // b_rank = rank_support_v<1>();
            load_from_file(bitvector, "bitvector_" + fname);
            load_from_file(superblocks, "superblocks_" + fname);
            load_from_file(blocks, "blocks_" + fname);

            cout << bitvector << endl;
            printf("reloaded. size is %d\n", overhead());
        }
};



int main(){
    // bit_vector b = {1,1,0,1,0,0,1};
    bit_vector b = {1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0};
    cout << b << endl;

    my_rank_support r (&b);
    cout << "overhead in bits: " << r.overhead() << endl;

    rank_support_v<1> b_rank(&b); // <- pointer to b

    // int i = 6;
    // printf("computed rank of %d = %lu. actual rank = %lu\n", i, r.rank1(i), b_rank(i));

    // save and load

    for (size_t i=1; i <= b.size(); i++)
        printf("computed rank of %lu = %lu. actual rank = %lu\n", i, r.rank1(i), b_rank(i));

    // performance test: print N, time to do 1000 select operations, overhead
    int num_ops = 100000;
    for (int N : {1000, 10000, 100000, 1000000, 10000000}) {
        
        b = bit_vector(N, 0);

        // put in N/2 1s randomly
        int num_ones = N/2;
        for (int i=0; i<num_ones; i++) {
            int idx = rand() % N;         // v1 in the range 0 to N-1
            b[idx] = 1;
        }
        
        my_rank_support b_rank (&b);

        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

        for (int i=0; i<num_ops; i++) {
            int n = rand() % N;
            b_rank.rank1(n);
        }
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

        // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[mics]" << std::endl;
        int time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        printf("N=%d, time=%d, overhead=%d\n", N, time, b_rank.overhead());

        // 2n/logn + 2nloglogn/logn
        
        // printf("N=%d, time=%d, overhead=%d, theoretical overhead: %d\n", 
        //     N, time, b_rank.overhead(), 2*N/log2(N) + 2*N*log2(log2(N))/log2(N));

    }
}
