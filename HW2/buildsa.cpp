
#include <iostream>
#include <stdio.h>
#include <math.h>

#include <chrono>
#include <unordered_map>
#include <filesystem>
#include <fstream>

#include <sys/stat.h>



#include <sdsl/suffix_arrays.hpp>

#include <cereal/archives/binary.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/tuple.hpp>

#include <sstream>

#include "buildsa.h"

using namespace std;
using namespace sdsl;


int test() {
    return 0;
}

// long GetFileSize(std::string filename)
// {
//     struct stat stat_buf;
//     int rc = stat(filename.c_str(), &stat_buf);
//     return rc == 0 ? stat_buf.st_size : -1;
// }

std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}


// search the suffix array using the simple accelerant
int simple_accelerant_search(string& reference, string& query, csa_bitcompressed<>& suffix_array, string endpoint) {

    int l = 0;
    int r = suffix_array.size();
    int c;

    int lcp_lp = 0;
    int lcp_rp = 0;

    while (true) {
        c = (r+l)/2;

        // find the longest common prefix
        // # determine whether p < T[sa[c]:] by doing comparisons
        // # starting from left-hand sides of p and T[sa[c]:]
        bool query_lt = true; // assume p < T[sa[c]:] until proven otherwise
        // int i = 0;
        int i = min(lcp_lp, lcp_rp);
        while (i < query.size() && suffix_array[c]+i < reference.size()) {
            if (query[i] < reference[suffix_array[c]+i]) {
                break;
            } else if (query[i] > reference[suffix_array[c]+i]) {
                query_lt = false;
                break;
            }
            i += 1;
        }

        // if query has been fully matched
        if (endpoint == "right" && i == query.size()) {
            if (c == r-1)
                return r;
            else
                l=c;
                lcp_lp = i;
            continue;
        }

        if (query_lt) {
            if (c == l+1)
                return c;
            else
                r = c;
                lcp_rp = i;
        } else {
            if (c == r-1)
                return r;
            else
                l=c;
                lcp_lp = i;
        }
    }
}




// search the suffix array
// endpoint: left, right
int binary_search_sa(
    string& reference, 
    string& query, 
    csa_bitcompressed<>& suffix_array, 
    string endpoint,
    bool reduced_interval = false,
    int left = -1,
    int right = -1
    ) {

    int l; int r; int c;
    if (reduced_interval) {
        l = left;
        r = right;
    } else {
        l = 0;
        r = suffix_array.size();
    }


    while (true) {
        c = (r+l)/2;

        // find the longest common prefix
        // # determine whether p < T[sa[c]:] by doing comparisons
        // # starting from left-hand sides of p and T[sa[c]:]
        bool query_lt = true; // assume p < T[sa[c]:] until proven otherwise
        int i = 0;
        while (i < query.size() && suffix_array[c]+i < reference.size()) {
            if (query[i] < reference[suffix_array[c]+i]) {
                break;
            } else if (query[i] > reference[suffix_array[c]+i]) {
                query_lt = false;
                break;
            }
            i += 1;
        }

        // if query has been fully matched, go right
        if (endpoint == "right" && i == query.size()) {
            if (c == r-1)
                return r;
            else
                l=c;
            continue;
        }

        if (query_lt) {
            if (c == l+1)
                return c;
            else
                r = c;
        } else {
            if (c == r-1)
                return r;
            else
                l=c;
        }
    }
}

// enumerate prefixes
void enumerate_strings(string prefix, int k, char chars[], vector<string>& prefixes) {

    if (prefix.size() == k) {
        // if (prefix.size() == k) {
        // cout << prefix << endl;
        prefixes.push_back(prefix);
        return;
    }
    // for (char c : chars) {
    for (int i=0; i<4; i++) {
        char c = chars[i];
        prefix.push_back(c);
        
        enumerate_strings(prefix, k, chars, prefixes);
        prefix.pop_back();
    }
    
}





void test_suffix_array(string& reference_path) {

    string line;
    string reference;

    // read file
    ifstream MyReadFile(reference_path);
    getline (MyReadFile, line); // skip first line
    // Use a while loop together with the getline() function to read the file line by line
    while (getline (MyReadFile, line)) {
        // cout << myText;
        reference += line;
    }
    MyReadFile.close();

    cout << "file length: " << reference.length() << " bytes.\n";


    csa_bitcompressed<> csa;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    construct_im(csa, reference, 1);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[mics]" << std::endl;
    int time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    printf("genome size=%lu, time=%d, SA size=%lu\n", reference.length(), time, csa.size());
    // cout << "csa.size(): " << csa.size() << endl;

}

unordered_map<string, tuple<int,int>> build_prefix_table(string& reference, csa_bitcompressed<>& csa, int k) {
    
    unordered_map<string, tuple<int,int>> prefix_table;
    vector<string> prefixes;
    char chars[] = {'A','C','G','T'};
    enumerate_strings("", k, chars, prefixes);
    printf("k=%d, prefixes=%d\n", k, prefixes.size());

    // cout << "prefixes.size(): " << prefixes.size() << endl;
    
    // string prefix = "CC";   
    // cout << "query: " << prefix << endl;     
    // // int left_idx = binary_search_sa(reference, prefix, csa, "left");
    // // int right_idx = binary_search_sa(reference, prefix, csa, "right") - 1;
    // int left_idx = simple_accelerant_search(reference, prefix, csa, "left");
    // int right_idx = simple_accelerant_search(reference, prefix, csa, "right") - 1;
    // printf("left idx=%d, right idx=%d\n", left_idx, right_idx);


    for (string prefix : prefixes) {
    // for (int i=0; i<1; i++) {
    //     string prefix = prefixes[i];     
        // int left_idx =  (reference, prefix, csa, "left");
        // int right_idx = binary_search_sa(reference, prefix, csa, "right") - 1;  
        int left_idx = simple_accelerant_search(reference, prefix, csa, "left");
        int right_idx = simple_accelerant_search(reference, prefix, csa, "right") - 1; 

        prefix_table[prefix] = make_tuple(left_idx, right_idx);
        // prefix_table[prefix] = make_tuple(0,0);
    }

    // cout << "prefix_table size: " << prefix_table.size() << endl;

    // test prefix table
    // for (string prefix : {"AAG", "GCC","CGC"}) {
    //     // cout << prefix << " prefix table: " << prefix_table[prefix] << endl;
    //     auto [left_idx, right_idx] = prefix_table[prefix];
    //     // tie(left_idx, right_idx) = prefix_table[prefix];

    //     printf("prefix: %s, left idx=%d, right idx=%d\n", prefix.c_str(), left_idx, right_idx);

    // }

    return prefix_table;
}



int main(int argc, char *argv[]){

    // cout << "argv: " << *argv << endl;
    bool verbose = false;
    
    bool make_prefix_table = false;
    int k;
    string reference_path;
    string output_path;

    // preftab (optional), reference, output
    if (argc == 3) {
        reference_path = argv[1];
        output_path = argv[2];
    } else if (argc == 5) {
        make_prefix_table = true;
        k = stoi(argv[2]);
        reference_path = argv[3];
        output_path = argv[4];
    }

    // cout << "reference_path: " << reference_path << endl;


    // Read reference
    string line;
    string reference;
    ifstream MyReadFile(reference_path);
    getline (MyReadFile, line); // skip first line
    // cout << "get line" << line << endl;
    while (getline (MyReadFile, line)) {
        // cout << "get line" << endl;
        reference += line;
    }
    MyReadFile.close();


    // suffix array
    csa_bitcompressed<> csa;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    construct_im(csa, reference, 1);
    reference += "$"; // important for the binary search to work

    unordered_map<string, tuple<int,int>> prefix_table;
    if (make_prefix_table) {
        // cout << "make_prefix_table" << endl;
        prefix_table = build_prefix_table(reference, csa, k);
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[mics]" << std::endl;
    int time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    // save
    store_to_file(csa, output_path);
    // load_from_file(csa, output_path);

    string preftab_path = output_path + "_preftab.cereal";
    {
        std::ofstream os(preftab_path, ios::binary);
        cereal::BinaryOutputArchive oarchive(os); // Create an output archive

        // MyClass m1;
        oarchive(reference, prefix_table, k); // Write the data to the archive
    } // archive goes out of scope, ensuring all contents are flushed

    // filesystem::path p{output_path};
    // std::cout << "The size of " << p.u8string() << " is " <<
    //     std::filesystem::file_size(p) << " bytes.\n";
    
    // cout << "file size: " << GetFileSize(preftab_path) << endl;
    // int preftab_size = filesize(preftab_path.c_str());
    int preftab_size = filesize(preftab_path.c_str());

    // cout << "preftab size: " << preftab_size << endl;

    int sa_size = size_in_bytes(csa);
    printf("genome size=%lu, time=%d ms, SA size=%lu, preftab size=%d, total_size=%d\n", 
        reference.length(), time, sa_size, preftab_size, sa_size+preftab_size);
    // cout << "length: " << reference.length() << " bytes.\n";
    // cout << "csa.size(): " << csa.size() << endl;

    if (verbose) {
        for (size_t i=0; i<csa.size(); i++) {
            // cout << i << " " << reference.substr(csa[i]) << endl;
            cout << i << " " << extract(csa,csa[i],csa.size()-1) << endl;
        }
    }

    


    

    {
        std::ifstream is(preftab_path, ios::binary);

        cereal::BinaryInputArchive iarchive(is); // Create an input archive

        // MyClass m1;
        string new_reference;
        unordered_map<string, tuple<int,int>> new_table;
        int k;

        iarchive(new_reference, new_table, k); // Read the data from the archive
    }




    // multiple references
    // int N = 2;
    // string references[N] = {"data/ecoli.fa", "data/celegans.fa"};
    // for (int i = 0; i < N; i++) {
    //     // cout << argv[i] << "\n";
    //     test_suffix_array(references[i]);
    // }
}
