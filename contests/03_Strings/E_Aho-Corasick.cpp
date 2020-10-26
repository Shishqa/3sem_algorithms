/* (c) Shishqa, 2020 */
/*==========================================================================*/

/*##########################################################################*/

// WARNING: this solution is not clean and can contain some small mistakes
//   > sry, I was short of time :c

/*##########################################################################*/

#include <fstream>
#include <iostream>

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

private:

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

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

constexpr size_t MAX_STRING_LENGTH = 1000005;

int main() {

    char string[MAX_STRING_LENGTH] = "";
    std::cin.getline(string, sizeof(string));

    size_t vocab_size = 0;
    std::cin >> vocab_size;
    std::cin.ignore();

    char vocab_buffer[MAX_STRING_LENGTH]  = "";

    Trie vocabulary;

    char* curr_word = vocab_buffer;
    for (size_t i = 0; i < vocab_size; ++i) {

        std::cin.getline(curr_word, 
                vocab_buffer + sizeof(vocab_buffer) - curr_word - 1);
        std::string_view word(curr_word, strlen(curr_word)); 

        vocabulary.add(word);

        curr_word += word.size();
    }

    std::vector<std::set<size_t>> oc_str = 
        vocabulary.aho_corasick(std::string_view(string, strnlen(string, sizeof(string))));

    //vocabulary.dump(".dump-aho");

    for (const auto& oc : oc_str) {
        std::cout << oc.size() << " ";
        for (const auto& elem : oc) {
            std::cout << elem << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

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
