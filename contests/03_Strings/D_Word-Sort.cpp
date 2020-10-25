/* (c) Shishqa, 2020 */
/*==========================================================================*/

/*##########################################################################*/

// WARNING: this solution is not clean and can contain some small mistakes
//   > sry, I was short of time :c

/*##########################################################################*/

#include <cstdio>
#include <iostream>
#include <set>
#include <string_view>

#include <queue>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

class Trie {
public:
   
    Trie() : root(0, 1, true, "wq") {}
    ~Trie() = default;

    void add(const std::string_view& str);

    void dump();

    //bool has(const std::string_view& str);
    //bool remove(const std::string_view& str);

    struct Node {
        Node(const char& last_c, const size_t& cnt,
             const bool& terminal,
             const std::string_view& s) 
            : cnt(cnt)
            , last_char(last_c)
            , is_terminal(terminal)
            , str(s) {}

        ~Node();

        struct comparator {
            bool operator()(Node* const& a, Node* const& b) const {
                return a->last_char < b->last_char;
            }
        };

        std::set<Node*, comparator> go; 
        size_t cnt;
        char last_char;
        bool is_terminal;
        std::string_view str;
    };

    Node root;
};

void traverse(Trie::Node* node, char*& str, size_t& len);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

constexpr size_t MAX_STRING_LENGTH = 1000005;

int main() {

    char string[MAX_STRING_LENGTH] = "";

    std::cin.getline(string, MAX_STRING_LENGTH);
    std::string_view str(string);

    Trie trie;

    for (size_t i = 0; i < str.size(); ++i) {
        if ('.' == str[i]) {
            continue;
        }

        size_t next_dot = str.find(".", i);

        std::string_view curr_str(
                string + i, 
                (std::string_view::npos == next_dot ? str.size() : next_dot) - i
                );
        trie.add(curr_str);
        i += curr_str.size();
    }

    char* strr = string;
    size_t len = str.size();

    traverse(&trie.root, strr, len);
    while (*strr && *strr == '.') {
        strr++;
        std::cout << '.';
    }

    return 0;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void traverse(Trie::Node* node, char*& str, size_t& len) {
    
    for (auto& target : node->go) {

        if (target->is_terminal) {
            for (size_t i = 0; i < target->cnt; ++i) { 
                while (*str && *str == '.') {
                    str++;
                    std::cout << '.';
                }    
                std::cout << target->str;
                while (*str && *str != '.') {
                    ++str;
                }
            }
        }

        traverse(target, str, len);
    }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void Trie::add(const std::string_view& str) {

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
            auto new_node = new Node(character, 0, false, 
                                     std::string_view(""));
            curr_node->go.insert(new_node);
            curr_node = new_node;
        }
    }

    if (curr_node->is_terminal) {
        curr_node->cnt++;
        return;
    }

    curr_node->is_terminal = true;
    curr_node->cnt = 1;
    curr_node->str = std::string_view(str);
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

        for (auto& node : target->go) {
            
            fprintf(file, "\tnode%p -> node%p [label=\"%c | %s\", color=%s];\n",
                    target, node, node->last_char, 
                    (node->is_terminal ? node->str.data() : "not_terminal"),
                    (node->is_terminal ? "red" : "black"));

            to_visit.push(node);
        }
    }

    fprintf(file, "\n}\n");
}

/*==========================================================================*/
