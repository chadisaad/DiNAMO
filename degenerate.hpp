#if !defined(__DEGENERATE_HPP_INCLUDED__)

    #define  __DEGENERATE_HPP_INCLUDED__

    //=====================
    //     Dependencies
    //=====================

    #include <iostream>

    #include <string>
    using std::string;

    #include <vector>
    using std::vector;

    #include <unordered_set>
    using std::unordered_set;

    #include <utility>
    using std::pair;
    using std::make_pair;

    //=====================
    //     Libs needed
    //=====================

    #include "lib/sparsepp.h"
    using spp::sparse_hash_map;

    //=====================

    string find_neighbor_motifs(sparse_hash_map<char, pair<string, int>> &, sparse_hash_map<char, pair<string, int>> &, const string &, unsigned int);

    void degenerate(sparse_hash_map<string, pair<int, int>> &, sparse_hash_map<string, pair<int, int>> &, unsigned int);

#endif
