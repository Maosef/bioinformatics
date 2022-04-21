
#include <iostream>
#include <stdio.h>
#include <math.h>

#include <sdsl/suffix_arrays.hpp>
// #include <sdsl/bit_vectors.hpp>

#include <chrono>


using namespace std;
using namespace sdsl;



int main(int argc, char *argv[]){

    int numQueries = 10000;
    // int queryLength = 21;
    // int numQueries = 10;
    // int queryLength = 4;

    // reference, output, preftab (optional)
    string reference_path = argv[1];
    int queryLength = stoi(argv[2]);

    // Create a text string, which is used to output the text file
    string line;
    string reference;
    // Read from the text file
    ifstream MyReadFile(reference_path);
    getline (MyReadFile, line); // skip first line
    // Use a while loop together with the getline() function to read the file line by line
    while (getline (MyReadFile, line)) {
        // cout << myText;
        reference += line;
    }
    MyReadFile.close();


    cout << "length: " << reference.length() << " bytes.\n";

    // reference lengths
    // int lengths[5] = {1000, 10000, 100000, 1000000, 10000000, 100000000}
    int lengths[5] = {1000, 10000, 100000, 1000000, 10000000};
    // int lengths[1] = {10};

    for (int length : lengths) {
        
        // ofstream myfile;
        // myfile.open ((to_string(length) + ".fasta").c_str());
        // myfile << "Writing this to a file.\n";
        // myfile.close();


        FILE *outfile = fopen ((to_string(length) + ".fa").c_str(), "w");

        fprintf(outfile, "> reference %d\n", length);
        string substring = reference.substr(0,length);
        fprintf(outfile, "%s\n", substring.c_str());
        
        // fprintf(outfile, "+\n");

        int ref_length = substring.length();
        cout << "Constructed reference, length=" << ref_length << endl;

        

        // make queries file
        cout << "Creating random " << queryLength << "-mers" << endl;
        // Create queries as random kmers from the genome
        vector<string> queries(numQueries, "");
        vector<long long> kmers = vector<long long>(numQueries, 0);
        vector<size_t> idxs = vector<size_t>(numQueries, 0);
        for(int i = 0; i<numQueries; i++)
        {
            idxs[i] = rand() % (ref_length - queryLength);
            queries[i] = substring.substr(idxs[i], queryLength);
            // kmers[i] = sap.kmerizeAdjusted(queryLength, queries[i]);
        }

        // Write the queries to a file
        FILE *query_file = fopen ((to_string(length) + ".queries.fa").c_str(), "w");
        for(int i = 0; i < numQueries; i++)
        {
            fprintf(query_file, "> read%d\n", i+1);
            fprintf(query_file, "%s\n", queries[i].c_str());
            // fprintf(outfile, "+\n");
            
            // for(int j = 0; j<queryLength; j++) fprintf(outfile, "9");
            fprintf(query_file, "\n");
        }
        cout << "Constructed queries" << endl;
    }
}
    


    
    


