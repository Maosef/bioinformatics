
#include <iostream>
#include <stdio.h>
#include <math.h>

#include <sdsl/suffix_arrays.hpp>
// #include <sdsl/bit_vectors.hpp>

#include <chrono>

#include <cereal/archives/binary.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/tuple.hpp>


// #include "buildsa.h"

using namespace std;
using namespace sdsl;




// search the suffix array using the simple accelerant
int simple_accelerant_search(
    string& reference, 
    string& query, 
    csa_bitcompressed<>& 
    suffix_array, 
    string endpoint,
    int left_idx,
    int right_idx
    ) {

    int l = left_idx;
    int r = right_idx;
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
// doesn't work when r-l=1
int simple_binary_search(
    string& reference, 
    string& query, 
    csa_bitcompressed<>& suffix_array, 
    string endpoint,
    int left_idx,
    int right_idx,
    bool verbose=true
) {

    int l = left_idx; int r = right_idx; int c;
    int N = reference.size();
    // cout << reference.substr(suffix_array[l],20) << endl;
    // cout << reference.substr(suffix_array[r],20) << endl;

    // int cnt=0;
    
    while (true) {
        c = (r+l)/2;
        // cout << c << endl;

        // find the longest common prefix
        // # determine whether p < T[sa[c]:] by doing comparisons
        // # starting from left-hand sides of p and T[sa[c]:]
        bool query_lt = true; // assume p < T[sa[c]:] until proven otherwise
        int i = 0;
        while (i < query.size() && suffix_array[c]+i < N) {
            if (query[i] < reference[suffix_array[c]+i]) {
                break;
            } else if (query[i] > reference[suffix_array[c]+i]) {
                query_lt = false;
                break;
            }
            i += 1;
        }

        // cout << endpoint << i << query.size() << "query_lt: " << query_lt << endl;

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

        
    // addresses a strange edge case that comes up when l==r
    if (l==r) {
        // if (reference.substr(suffix_array[l]).find(query) == 0) {
        if (i == query.size()) {
        // cout << "l=r, found match" << endl;
        return l;
    } else {
        cout << "didn't find match" << endl;
        return -1;

    }
    }
    
    }

    

    

}


// search the suffix array, either naive or simpaccel
// endpoint: left, right
int binary_search_sa(
    string& reference, 
    string& query, 
    csa_bitcompressed<>& suffix_array, 
    string endpoint,
    string query_mode,
    int prefix_len = 0,
    int left_idx = -1,
    int right_idx = -1
) {

    int l; int r; int c;
    if (prefix_len == 0) {
        l = 0;
        r = suffix_array.size();
    } else { // use prefix table
        // query = query.substr(prefix_len);
        l = left_idx;
        r = right_idx;
    }

    // cout << reference.substr(suffix_array[l],20) << endl;
    // cout << reference.substr(suffix_array[r],20) << endl;


    if (query_mode == "naive") {
        return simple_binary_search(
            reference,
            query,
            suffix_array,
            endpoint,
            l,
            r
        );
    } else if (query_mode == "simpaccel") {
        return simple_accelerant_search(
            reference,
            query,
            suffix_array,
            endpoint,
            l,
            r
        );
    } else {
        return -1;
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


int query_len = 21;

int main(int argc, char *argv[]){

    // reference, output, preftab (optional)
    string sa_path = argv[1];
    string queries_path = argv[2];
    string query_mode = argv[3];
    string output_path = argv[4];

    // cout << "sa_path: " << sa_path << endl;

    // load data
    csa_bitcompressed<> csa;
    load_from_file(csa, sa_path);
    
    string preftab_path = sa_path + "_preftab.cereal";
    string reference;
    unordered_map<string, tuple<int,int>> preftable;
    int prefix_len;
    {
        std::ifstream is(preftab_path, ios::binary);
        cereal::BinaryInputArchive iarchive(is); // Create an input archive

        iarchive(reference, preftable, prefix_len); // Read the data from the archive
    }


    // read queries file
    string line;
    vector<string> queries;
    vector<string> names;

    ifstream MyReadFile(queries_path);
    // Use a while loop together with the getline() function to read the file line by line
    while (getline (MyReadFile, line)) {
        names.push_back(line.substr(1));
        getline(MyReadFile, line); // get the query
        // cout << "line: " << line << endl;
        queries.push_back(line);
        getline(MyReadFile, line); // newline
    }
    MyReadFile.close();



    vector<string> out_lines;
    bool verbose = false;
    bool verbose_2 = false;

    if (verbose) {
        cout << "reference: " << reference << endl;
        for (size_t i=0; i<csa.size(); i++) {
            // cout << i << " " << reference.substr(csa[i]) << endl;
            cout << i << " " << extract(csa,csa[i],csa.size()-1) << endl;
        }

        // cout << "queries: " << queries.size() << endl;
        cout << "query mode " << query_mode << endl;
        // printf("genome size=%lu, SA size=%lu\n", reference.length(), csa.size());

    }

    // critical section:
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    //     // auto occs = locate(csa, query);
    //     int k = count(csa, query);
    //     // cout << "k: " << k << endl; 
    //     // cout << "occs: " << occs << endl; 
        
    //     string out_line = names[i] + '\t' + to_string(k);
    // }

    // for (size_t i = 4; i < 5; i++) {
    // for (size_t i = 0; i < 100; i++) {
    for (size_t i = 0; i < queries.size(); i++) {
        string query = queries[i];

        int start_idx; int end_idx;

        // use prefix table?
        if (prefix_len == 0) {
            start_idx = binary_search_sa(reference, query, csa, "left", query_mode);
            end_idx = binary_search_sa(reference, query, csa, "right", query_mode) - 1;
        } else {
            string prefix = query.substr(0,prefix_len);
            auto [left_idx, right_idx] = preftable[prefix];

            if (verbose_2) {
                printf("query=%s, prefix=%s, new l=%d, new r=%d\n", 
                    query.c_str(), prefix.c_str(), left_idx, right_idx);
            }
            
            if (left_idx == right_idx) {
                start_idx = left_idx;
                end_idx = right_idx;
            } else {
                start_idx = binary_search_sa(
                    reference, query, csa, "left", query_mode,
                    prefix_len, left_idx, right_idx);
                end_idx = binary_search_sa(
                    reference, query, csa, "right", query_mode,
                    prefix_len, left_idx, right_idx) - 1;
            }
            
        }


        
        int k = end_idx - start_idx + 1;

        // get occurrences
        string occs[k];
        for (int j=start_idx; j<=end_idx; j++) {
            // cout << j << ": " << csa[j] << endl;
            occs[j-start_idx] = csa[j];
        }
        // auto occs = locate(csa, query);
         

        if (verbose) {
            cout << "query: " << query << endl;
            printf("start idx: %d, end idx: %d\n", start_idx, end_idx);
            cout << "k: " << k << endl; 
            cout << "occs: " << " "; 
            for (int j=0; j<=k; j++) {
                cout << csa[j] << " ";
            }
        }

        // write: query_name, k, hit_1, hit_2, hit_k
        string out_line = names[i] + '\t' + to_string(k);

        // out_lines.push_back(names[i]); 
        // out_line += to_string(k)
        for (auto occ : occs) {
            out_line += '\t';
            // out_line += to_string(occ);
            out_line += occ;
        }
        out_lines.push_back(out_line);
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    int time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    printf("genome size=%s, query size=%d, num queries %lu, time=%d ms\n", sa_path.c_str(), query_len, queries.size(), time);
    

    // Write to file
    FILE *outfile = fopen (output_path.c_str(), "w");
    for (auto line: out_lines)
        fprintf(outfile, line.c_str(), "+\n");
        // std::cout << line << ' ';





    // multiple references
    // int N = 2;
    // string references[N] = {"data/ecoli.fa", "data/celegans.fa"};
    // for (int i = 0; i < N; i++) {
    //     // cout << argv[i] << "\n";
    //     test_suffix_array(references[i]);
    // }
}
