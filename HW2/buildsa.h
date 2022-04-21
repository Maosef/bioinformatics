using namespace std;
using namespace sdsl;

int simple_accelerant_search(string& reference, string& query, csa_bitcompressed<>& suffix_array, string endpoint);

int binary_search_sa(string& reference, string& query, csa_bitcompressed<>& suffix_array, string endpoint);

int test();
