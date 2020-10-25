/* (c) Shishqa, 2020 */
/*==========================================================================*/

/*##########################################################################*/

// WARNING: this solution is not clean and can contain some small mistakes
//   > sry, I was short of time :c

/*##########################################################################*/

#include <cstdio>
#include <iostream>
#include <set>
#include <string>
#include <string_view>

#include <queue>
#include <vector>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

class Trie {
public:
   
    Trie();

    Trie(const std::vector<std::string_view>& vocabulary);

    ~Trie() = default;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    void add(const std::string_view& str);

    void dump();

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

private:

    struct Node {

        Node(const char& last_char) 
            : last_char(last_char)
            , is_terminal(false) {}

        ~Node();

        struct comparator {
            bool operator()(Node* const& a, Node* const& b) const {
                return a->last_char < b->last_char;
            }
        };
        std::set<Node*, comparator> go; 

        char last_char;
        bool is_terminal;
        std::vector<size_t> related_patterns;
    };

    Node root;
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void aho_corasick(const std::string_view& str, const Trie& vocab);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

constexpr size_t MAX_STRING_LENGTH = 1000005;

int main() {

    char string[MAX_STRING_LENGTH] = "";
    std::cin.getline(string, sizeof(string));

    size_t vocab_size = 0;
    std::cin >> vocab_size;
    std::cin.ignore();

    char vocab_buffer[MAX_STRING_LENGTH]  = "";

    std::vector<std::string_view> vocab;
    vocab.reserve(vocab_size);

    char* curr_word = vocab_buffer;
    for (size_t i = 0; i < vocab_size; ++i) {
        std::cin.getline(curr_word, 
                vocab_buffer + sizeof(vocab_buffer) - curr_word - 1);

        vocab.emplace_back(curr_word);

        curr_word += vocab[i].size();
    }

    Trie trie(vocab);

    trie.dump();

    return 0;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Trie::Trie(const std::vector<std::string_view>& vocabulary)
        : root(0) {

    for (const auto& elem : vocabulary) {
        add(elem);
    }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Trie::Trie()
        : root(0) {}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void Trie::add(const std::string_view& str) {

    //std::cout << "add " << str << "\n";

    Node* curr_node = &root;

    for (const auto& character : str) {
        bool found_way = false;
        for (auto& node : curr_node->go) {
            if (character == node->last_char) {
                curr_node = node;
                found_way = true;
                break;
            }
        }
        if (!found_way) {
            auto new_node = new Node(character);
            curr_node->go.insert(new_node);
            curr_node = new_node;
        }
    }

    curr_node->is_terminal = true;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Trie::Node::~Node() {
    for (auto& node : go) {
        delete node;
    }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void Trie::dump() {

    FILE* file = stdout;

    fprintf(file, "digraph G {\n");

    std::queue<Node*> to_visit;
    to_visit.push(&root);

    while (!to_visit.empty()) {

        Node* target = to_visit.front();
        to_visit.pop();

        fprintf(file, "\tnode%p [label=\"\", color=\"%s\"];\n",
                target, (target->is_terminal ? "red" : "black"));

        for (auto& node : target->go) {
            
            fprintf(file, "\tnode%p -> node%p [label=\"%c\"];\n",
                    target, node, node->last_char);

            to_visit.push(node);
        }
    }

    fprintf(file, "\n}\n");
}

/*==========================================================================*/
