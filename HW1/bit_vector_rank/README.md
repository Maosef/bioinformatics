## bit vector rank

Requirements: SDSL, C++ 20, g++ 9.3.0.
SDSL: https://github.com/simongog/sdsl-lite

compile and run: `g++ -std=c++2a -O3 -DNDEBUG -I ~/include -L ~/lib bit_vector_rank.cpp -o program -lsdsl -ldivsufsort -ldivsufsort64 && ./program`


## Usage
```
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
```

## Functions
```
uint64_t rank1(uint64_t i) : Returns the number of 1s in the underlying bit-vector up to position i (inclusive).
uint64_t overhead() : Returns the size of the rank data structure (in bits) that is required to support constant-time rank on the current bitvector.
save(string& fname) : Saves the rank data structure for this bit vector to the file fname (your bit vector should also have a save() function).
load(string& fname) : Loads the rank data structure for this bit vector from the file fname (your bit vector should also have a load() function).
```