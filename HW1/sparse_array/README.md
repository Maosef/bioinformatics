## sparse array

Requirements: SDSL, C++ 20, g++ 9.3.0.
SDSL: https://github.com/simongog/sdsl-lite

compile and run: `g++ -std=c++2a -O3 -DNDEBUG -I ~/include -L ~/lib sparse_array.cpp -o program -lsdsl -ldivsufsort -ldivsufsort64 && ./program`


## Usage
```
sparse_array s;
s.create(10);

s.append("foo", 1);
s.append("bar", 5);
s.append("baz", 9);

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
```

## Functions
```
void create(uint64_t size) : Creates an empty sparse array of length size (the size of the underlying bitvector you will create).

void append(string elem, uint64_t pos) : Appends the element elem at index pos of the sparse array. You may assume this is the only way to insert elements, and so you will always insert element in-order and never will insert two element with the same position, further you may assume that you will always have pos < size (but you should probably guard against this anyway).

bool get_at_rank(uint64_t r, std::string& elem) : This function places a reference to the r-th present item in the array in the reference elem. It returns true if there was >= r items in the sparse array and false otherwise.

bool get_at_index(uint64_t r, std::string& elem): This function looks at the r-th index in the sparse bitvector; if that bit is 1, it fetches the corresponding value and binds it to the reference elem and returns true, if that bit is a 0, it simply returns false.

uint64_t num_elem_at(uint64_t r): This function returns the count of present elements (1s in the bit vector) up to and including index r.

uint64_t size() : Returns the size of the sparse array.

uint64_t num_elem() : Returns the number of present elements in the sparse array (i.e. the number of 1s in the bitvector).

save(string& fname) : Saves the sparse array to the file fname.

load(string& fname) : Loads the sparse array from the file fname
```