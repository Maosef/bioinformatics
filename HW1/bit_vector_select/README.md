## bit vector select

Requirements: SDSL, C++ 20, g++ 9.3.0.
SDSL: https://github.com/simongog/sdsl-lite

compile and run: `g++ -std=c++2a -O3 -DNDEBUG -I ~/include -L ~/lib select.cpp -o program -lsdsl -ldivsufsort -ldivsufsort64 && ./program`


## Usage
```
bit_vector b = {1,1,0,1,0,0,1};
cout << b << endl;

my_select_support s (&b);
bit_vector::select_1_type b_sel(&b);

cout << "overhead in bits: " << s.overhead() << endl;
// cout << "select 5: " << s.select1(5) << endl;

for (size_t i=1; i <= 4; i++)
    printf("computed select of %d = %d. actual select = %d\n", i, s.select1(i), b_sel(i));
```

## Functions
```
uint64_t select1(uint64_t i) : Returns the position, in the underlying bit-vector, of the ith 1.
uint64_t overhead() : Returns the size of the select data structure (in bits) that is required to support log-time select on the current bitvector (how does this relate to the size of the rank data structure built above).
save(string& fname) : Saves the select data structure for this bit vector to the file fname (your bit vector should also have a save() function).
load(string& fname) : Loads the select data structure for this bit vector from the file fname (your bit vector should also have a load() function).
```