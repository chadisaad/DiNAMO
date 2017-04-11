#include "lib/sparsepp.h"
using spp::sparse_hash_map;

#include <utility>
using std::pair;

#include "optionsParser.hpp"
#include "hash.hpp"
#include "degenerate.hpp"


int main (int argc, char **argv) {

    InputParser input(argc, argv);

    /* Parsing filename */

    const string &filename = input.getCmdOption("-f");
    if (filename.empty()){
        cerr << "You did not input the file name, try adding \"-f path/to/your/file\" to the end of your command line" << endl;
        exit(EXIT_FAILURE);
    }

    /* Parsing kmere's size */

    const string &km_string = input.getCmdOption("-k");
    if (km_string.empty()){
        cerr << "Please specify a k-mer size putting \"-k size\" to the end of your command line" << endl;
        exit(EXIT_FAILURE);
    }

    int signed_k;
    try {
      signed_k = stoi(km_string);
    } catch (std::invalid_argument& e) {
        cerr << "The k-mer size you input (" << km_string << ") could not be converted to an int." << endl;
        exit(EXIT_FAILURE);
    } catch (std::out_of_range& e) {
        cerr << "The k-mer size you input (" << km_string << ") is too big !" << endl;
        cerr << "Note that most of the k-mer will be unique when the size is >= 15" << endl;
        exit(EXIT_FAILURE);
    }
    if (signed_k < 2) {
        cerr << "The motif size must be greater than or equal to 2 !" << endl;
        exit(EXIT_FAILURE);
    }

    unsigned int k = signed_k;
    unsigned int d = k;

    if(input.cmdOptionExists("-d")) {

        const string &degeneration_level = input.getCmdOption("-d");
        int signed_d;
        if (degeneration_level.empty()) {
            cerr << "You specified the option -d (limit degeneration) but did not input a value." << endl;
            cerr << "Option usage : append \"-d X\" to the end of your command line." << endl;
            cerr << "Description : Limits the degeneration to at most X positions per motif. Please note that X cannot be greater than k." << endl;
            exit(EXIT_FAILURE);
        }
        else {
            try {
                signed_d = stoi(degeneration_level);
            } catch (std::invalid_argument &e) {
                cerr << "The degeneration limit you entered (" << degeneration_level << ") could not be converted to an int." << endl;
                exit(EXIT_FAILURE);
            } catch (std::out_of_range &e) {
                cerr << "The degeneration limit you entered (" << degeneration_level << ") is way too big !" << endl;
                cerr << "Please note that at most k positions can be degenerated (k being the motif size you chose)." << endl;
                exit(EXIT_FAILURE);
            }
            if (signed_d < 0 || signed_d > signed_k) {
                cerr << "The degeneration limit must be positive, and <= k ! (k being the motif size you chose)" << endl;
                exit(EXIT_FAILURE);
            }
            d = signed_d;
        }
    }

    //vector of pointers to hash_map
    vector<sparse_hash_map<string, pair<int, int>> *> hash_map_holder(d+1);

    //0_degree_motifs_hash_map
    hash_map_holder[0] = new sparse_hash_map<string, pair<int, int>>();

    //counting k-mers
    fill_hash_map(*hash_map_holder[0], filename, k);

    // for(auto const &it : *hash_map_holder[0]) {
    //     std::cout << it.first << "\t" << it.second.first << endl;
    // }
    // delete(hash_map_holder[0]);

    for (unsigned int i=0; i < d; i++) {
        hash_map_holder[i+1] = new sparse_hash_map<string, pair<int, int>>();
        degenerate(*(hash_map_holder[i]), *(hash_map_holder[i+1]), k);
    }

    for (unsigned int i=0; i <= d; i++) {
        for (auto const &it : *hash_map_holder[i]) {
            std::cout << it.first << "\t" << it.second.first << endl;
        }
    }

    for (auto const pointer : hash_map_holder) {
        delete(pointer);
    }
}