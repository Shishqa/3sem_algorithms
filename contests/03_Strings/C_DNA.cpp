/* (c) Shishqa, 2020 */
/*==========================================================================*/

/*##########################################################################*/

// WARNING: this solution is not clean and can contain some small mistakes
//   > sry, I was short of time :c

/*##########################################################################*/

#ifdef DEBUG
#define LOG( smth ) smth
#else 
#define LOG( smth ) 
#endif

#include <fstream>
#include <iostream>
#include <memory>

#include <ostream>
#include <set>
#include <unordered_set>
#include <unordered_map>

#include <cstring>
#include <string_view>

#include <queue>
#include <vector>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

class Trie {
public:

    using Char = char;
    using String = std::string_view;
   
    Trie() {
        root = new Node(0, nullptr);
    }

    ~Trie() {
        delete root;
    }

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    void add(const std::string_view& str);

    void dump(const char* filename);

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    std::vector<std::set<size_t>> aho_corasick(const String& str);

protected:

    struct Node {

        Node(const Char& last_char, Node* parent) 
            : last_char(last_char)
            , is_terminal(false) 
            , parent(parent) {}

        ~Node();

        std::unordered_map<Char, Node*> children; 
        std::unordered_map<Char, Node*> go;

        bool is_terminal;
        
        Node* parent = nullptr;
        Node* suffix_link = nullptr;
        Node* short_suffix_link = nullptr;
        
        Char last_char;
        std::unordered_set<size_t> related_patterns;
    };

    Node* get_suff_link(Node* node);
    
    Node* get_link(Node* node, const Char& c);

    Node* get_up(Node* node);

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    Node* root = nullptr;

    std::vector<String> vocabulary;
};

struct DnaResearch : private Trie {

    DnaResearch() 
            : Trie() 
            , id_helper(0)
    {}

    int id_helper;

    struct DnaChain {
        int id;
        std::string_view x;
        int start;
        int curr_pos;
        Node* curr_node;
    };

    std::unordered_map<Node*, std::unordered_set<DnaChain*>> unsolved_dna_chains;

    std::unordered_set<int> addGene(const std::string_view& gene, int start);

    std::unordered_set<int> addDnaChain(const std::string_view& chain, int start);

    std::unordered_set<int> solve(const std::unordered_set<DnaChain*>& to_solve);
};

std::ostream& operator<<(std::ostream& out, const DnaResearch::DnaChain& chain) {
  
    for (size_t i = 0; i < chain.x.size(); ++i) {
        out << "=";
    }

    out << std::endl << chain.x << std::endl;

    for (size_t i = 0; i < chain.start % chain.x.size(); ++i) out << " ";
    out << "^\n";

    for (size_t i = 0; i < chain.curr_pos % chain.x.size(); ++i) out << " ";
    out << (chain.curr_pos - chain.start == chain.x.size() ? "$" : "^") << "\n";

    for (size_t i = 0; i < chain.x.size(); ++i) {
        out << "=";
    }

    return out;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

constexpr size_t MAX_STRING_LENGTH = 2000005;

int main() {

    size_t n_operations = 0;
    std::cin >> n_operations;
    std::cin.ignore();

    char vocab_buffer[MAX_STRING_LENGTH]  = "";

    DnaResearch research;

    int last_solved = 0;

    char op = 0;
    char* curr_word = vocab_buffer;
    for (size_t i = 0; i < n_operations; ++i) {

        std::cin >> op;
        std::cin.ignore();
        std::cin.getline(curr_word, 
                vocab_buffer + sizeof(vocab_buffer) - curr_word - 1);
        std::string_view word(curr_word, strlen(curr_word));  

        std::unordered_set<int> solved;
        if (op == '+') {
            solved = research.addGene(word, last_solved);
        } else {
            solved = research.addDnaChain(word, last_solved);
        }

        std::cout << solved.size() << " ";
        for (auto& solved_id : solved) {
            std::cout << solved_id << " ";
        }
        std::cout << "\n";

        last_solved = solved.size();

        LOG(std::cout << "################################################\n");

        curr_word += word.size() + 1;
    }

    return 0;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

std::unordered_set<int>
DnaResearch::addGene(const std::string_view &gene, int start) {

    LOG(std::cout << "\n\nadd gene:(" << start << "): ");
    for (int i = start; i < start + gene.size(); ++i) {
        LOG(std::cout << gene[i % gene.size()]);
    }
    LOG(std::cout << "\n");

    Node* curr_node = root;
    std::unordered_set<DnaChain*> to_solve;

    for (int i = start; i < start + gene.size(); ++i) {
       
        Char character = gene[i % gene.size()];

        if (!curr_node->children.count(character)) {

            auto new_node = new Node(character, curr_node);
            curr_node->children[character] = new_node;

            std::unordered_set<DnaChain*> to_erase;

            for (auto& chain : unsolved_dna_chains[curr_node]) {
                if (character == chain->x[chain->curr_pos % chain->x.size()]) {
                    to_solve.insert(chain);
                    to_erase.insert(chain);
                }
            }

            for (auto& chain : to_erase) {
                unsolved_dna_chains[curr_node].erase(chain);
            }
        }

        curr_node = curr_node->children[character];
    }

    curr_node->is_terminal = true;

    vocabulary.emplace_back(gene);

    return solve(to_solve);    
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

std::unordered_set<int>
DnaResearch::addDnaChain(const std::string_view &chain, int start) {

    LOG(std::cout << "\n\nadd chain:(" << start << "): ");
    for (int i = start; i < start + chain.size(); ++i) {
        LOG(std::cout << chain[i % chain.size()]);
    }
    LOG(std::cout << "\n");

    auto new_chain = new DnaChain{++id_helper, chain, start, start, root};

    std::unordered_set<DnaChain*> to_solve;
    to_solve.insert(new_chain);

    return solve(to_solve);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

std::unordered_set<int> 
DnaResearch::solve(const std::unordered_set<DnaChain*>& to_solve) {

    std::unordered_set<int> solved;

    for (auto& chain : to_solve) {

        LOG(std::cout << "Solving:\n" << *chain << "\n");

        while (chain->curr_pos < chain->start + chain->x.size() &&
               chain->curr_node->children.count(chain->x[chain->curr_pos % chain->x.size()])) {

            chain->curr_node = chain->curr_node->children[chain->x[chain->curr_pos % chain->x.size()]]; 
            chain->curr_pos++;

            if (chain->curr_node->is_terminal) {
                chain->curr_node = root;
            }
        }

        LOG(std::cout << "Result:\n" << chain << "\n\n");

        if (chain->curr_pos == chain->start + chain->x.size()) {

            if (chain->curr_node == root) {
                solved.insert(chain->id); 
            }

            delete chain;
        
        } else {

            unsolved_dna_chains[chain->curr_node].insert(chain);

        }
    }

    return solved;
}

/*============================================================================*/

std::vector<std::set<size_t>> Trie::aho_corasick(const String& str) {

    std::vector<std::set<size_t>> occurrences(vocabulary.size()); 

    Node* curr_node = root;

    for (size_t i = 0; i < str.size(); ++i) {

        curr_node = get_link(curr_node, str[i]);

        Node* check = curr_node;
        while (root != check) {
            
            if (check->is_terminal) {
                for (const auto& p : check->related_patterns) {
                    occurrences[p].insert(i + 2 - vocabulary[p].size());
                }
            }

            check = get_up(check);
        }
    }

    return occurrences;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Trie::Node* Trie::get_link(Node* node, const Char& c) {

    if (!node->go.count(c)) {
        if (node->children.count(c)) {
        
            node->go[c] = node->children[c];
    
        } else if (root == node) {

            node->go[c] = root;
    
        } else {

            node->go[c] = get_link(get_suff_link(node), c);
        }
    }

    return node->go[c];
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Trie::Node* Trie::get_suff_link(Node* node) {

    if (!node->suffix_link) {
        if (root == node || root == node->parent) {
            
            node->suffix_link = root;
        
        } else {

            node->suffix_link = 
                get_link(get_suff_link(node->parent), node->last_char);
        }
    }

    return node->suffix_link;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Trie::Node* Trie::get_up(Node* node) {

    if (!node->short_suffix_link) {
        if (get_suff_link(node)->is_terminal) {
        
            node->short_suffix_link = get_suff_link(node);
        
        } else if (root == get_suff_link(node)) {

            node->short_suffix_link = root;
            
        } else {

            node->short_suffix_link = get_up(get_suff_link(node));
        }
    }

    return node->short_suffix_link;
}

/*==========================================================================*/

void Trie::add(const String& str) {

    Node* curr_node = root;

    for (const auto& character : str) {
       
        if (!curr_node->children.count(character)) {

            auto new_node = new Node(character, curr_node);
            curr_node->children[character] = new_node;
        }

        curr_node = curr_node->children[character];
    }

    curr_node->is_terminal = true;
    curr_node->related_patterns.insert(vocabulary.size());

    vocabulary.emplace_back(str);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Trie::Node::~Node() {
    for (auto& node : children) {
        delete node.second;
    }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void Trie::dump(const char* filename) {

    std::ofstream file;
    file.open(filename);

    file << "digraph G {\n";

    std::queue<Node*> to_visit;
    to_visit.push(root);

    while (!to_visit.empty()) {

        Node* target = to_visit.front();
        to_visit.pop();

        file << "\tnode" << target << " [label=\"";
        if (target->is_terminal) {
            file << vocabulary[*target->related_patterns.begin()];
        }
        file << "\", color=\"" << (target->is_terminal ? "red" : "black") << "\"];\n";

        for (auto& node : target->children) {
           
            file << "\tnode" << target << " -> " << "node" << node.second <<
                    " [label=\"" << node.second->last_char << "\"];\n";

            to_visit.push(node.second);
        }

        if (target->suffix_link) {
            file << "\tnode" << target << " -> " << "node" << target->suffix_link <<
                    " [style=\"dotted\"];\n";
        }
    }

    file << "\n}\n";

    file.close();
}

/*==========================================================================*/
