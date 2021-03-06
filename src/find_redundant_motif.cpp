#include "find_redundant_motif.hpp"

bool motifs_match(const string &motif1, const string &motif2, unsigned int l) {
    if(motif1 == motif2)
        return true;
    else
        for (unsigned int pos=0; pos < l; pos++) {
            if (motif1[pos] == motif2[pos])
                continue;
            auto const &motif1_successors = iupac_to_successors[motif1[pos]];
            if (find(motif1_successors.begin(), motif1_successors.end(), motif2[pos]) != motif1_successors.end())
                continue;
            auto const &motif2_successors = iupac_to_successors[motif2[pos]];
            if (find(motif2_successors.begin(), motif2_successors.end(), motif1[pos]) != motif2_successors.end())
                continue;
            return false;
        }
        return true;
}

void filter_redundant_motif(vector<pair<const string, Node *> *> &remaining_hash_map_entries, unsigned int l) {

    unsigned int side_addition_limit = ceil(l / 2.0);
    //resetting node state
    for (auto const &entry : remaining_hash_map_entries)
        entry->second->reset_state();

    //finding nodes that were not processed yet
    auto entry_it = remaining_hash_map_entries.begin();
    while (entry_it != remaining_hash_map_entries.end()) {

        (*entry_it)->second->validate();
        string base = (*entry_it)->first;
        unsigned int letter_added_left = 0;
        unsigned int letter_added_right= 0;

        //getting into the loop
        bool found_motif = true;

        //while we still have motifs that overlap with the base
        while (found_motif) {
            found_motif = false;
            //for each entry
            for (auto &entry : remaining_hash_map_entries) {
                //that isn't tagged
                if(entry->second->get_state() != unvisited)
                    continue;

                //compute reverse_complement
                vector<string> motif_and_rc = {entry->first, reverse_complement(entry->first)};

                for (auto const &motif : motif_and_rc) {
                    /* motif      |------|-|
                       base [...]-|------| */
                    if (letter_added_right < side_addition_limit &&
                        motifs_match(string(base, base.size() - (l-1), l-1), string(motif, 0, l-1), l-1)) {
                        base.push_back(motif[l-1]);
                        found_motif = true;
                        letter_added_right++;
                    }
                    /* motif |-|-----|
                       base    |-----|-[...] */
                    else if (letter_added_left < side_addition_limit &&
                             motifs_match(string(base, 0, l-1), string(motif, 1, l-1), l-1)) {
                        base.insert(0, 1, motif[0]);
                        found_motif = true;
                        letter_added_left++;
                    }
                    else {
                        unsigned int pos = 0;
                        while ((pos + l) < base.size()) {
                            if(motifs_match(string(base, pos, l), motif, l)) {
                                found_motif = true;
                            }
                            pos++;
                        }
                    }
                    if(found_motif) {
                        //std::clog << (*entry_it)->first << " " << motif << "\tbase: " << base << std::endl;
                        entry->second->suppress();
                        break;
                    }
                }
                if (found_motif)
                    break;
            }
        }
        entry_it = find_if(remaining_hash_map_entries.begin(),
                           remaining_hash_map_entries.end(),
                           [](const pair<const string, Node *> *entry) -> bool
                            { return entry->second->get_state() == unvisited; });
    }
    remaining_hash_map_entries.erase(
        remove_if(remaining_hash_map_entries.begin(),
                  remaining_hash_map_entries.end(),
                  [](auto &entry)
                    {return entry->second->get_state() != validated;}),
        remaining_hash_map_entries.end());
}
