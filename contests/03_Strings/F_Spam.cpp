/* (c) Shishqa, 2020 */
/*==========================================================================*/

/*##########################################################################*/

// WARNING: this solution is not clean and can contain some small mistakes
//   > sry, I was short of time :c

/*##########################################################################*/

#include <bits/stdint-uintn.h>
#include <utility>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <cstdint>

#include <set>
#include <sys/types.h>
#include <unordered_set>
#include <unordered_map>

#include <cstring>
#include <string_view>

#include <queue>
#include <vector>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

constexpr uint64_t MOD = 10000000000;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

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

        std::vector<uint64_t> ans;
    };

    Node* get_suff_link(Node* node);
    
    Node* get_link(Node* node, const Char& c);

    Node* get_up(Node* node);

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    Node* root = nullptr;

    std::vector<String> vocabulary;
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

class SpamResearch : public Trie {
public:

    SpamResearch(uint64_t investigated_len) 
        : len(investigated_len) 
    {}

    uint64_t ans();

private:

    void get_pure_terminals();

    const uint64_t len;
    std::unordered_set<Node*> pure_terminals; 

};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

constexpr size_t MAX_STRING_LENGTH = 10005;

int main() {

    size_t investigated_len = 0;
    std::cin >> investigated_len;

    size_t vocab_size = 0;
    std::cin >> vocab_size;
    std::cin.ignore();

    char vocab_buffer[MAX_STRING_LENGTH]  = "";

    SpamResearch research(investigated_len);

    char* curr_word = vocab_buffer;
    for (size_t i = 0; i < vocab_size; ++i) {

        std::cin.getline(curr_word, 
                vocab_buffer + sizeof(vocab_buffer) - curr_word - 1);
        std::string_view word(curr_word, strlen(curr_word)); 

        research.add(word);

        curr_word += word.size();
    }

    std::cout << research.ans() << std::endl;

    research.dump(".aho");

    return 0;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

uint64_t SpamResearch::ans() {

    get_pure_terminals();

    std::vector<uint64_t> ans(len + 1);
    ans[0] = 1;

    std::vector<uint64_t> helper(len + 1);
    helper[0] = 1;

    for (int i = 1; i <= len; ++i) {
  
        helper[i] = (helper[i - 1] * 3) % MOD;

        root->ans[i] = root->ans[i - 1] * 3;

        std::queue<std::pair<Node*, int>> to_visit;
        to_visit.push({root, 0});

        while (!to_visit.empty()) {
            
            auto target = to_visit.front();
            to_visit.pop();

            if (target.second > i) {
                continue;
            }

            if (target.first->is_terminal) {

                Node* curr_node = target.first;

                while (get_suff_link(curr_node) != root) {
                    get_suff_link(curr_node)->ans[i] -= 1;
                    curr_node = get_suff_link(curr_node);
                }
                get_suff_link(curr_node)->ans[i] -= 1;

                continue;
            }

            for (auto& node : target.first->children) {
                to_visit.push({node.second, target.second + 1});
            }

            if (target.first == root) {
                continue;
            }

            target.first->ans[i] = target.first->parent->ans[i - 1];
        }
    }

    std::cout << root->ans[len] << std::endl;

    return (helper[len] + MOD - root->ans[len]) % MOD;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void SpamResearch::get_pure_terminals() {
    
    std::queue<Node*> to_visit;
    to_visit.push(root);

    root->ans.resize(len + 1);
    root->ans[0] = 1;

    while (!to_visit.empty()) {

        Node* target = to_visit.front();
        to_visit.pop();

        target->ans.resize(len + 1);

        for (auto& node : target->children) {
    
            if (node.second->is_terminal && get_up(node.second) == root) {
                pure_terminals.insert(node.second);
            }

            to_visit.push(node.second);
        }
    }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

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
            file << vocabulary[*target->related_patterns.begin()] << "|";
        }
        for (const auto& elem : target->ans) {
            file << elem << " | ";
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
