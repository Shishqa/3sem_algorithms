/* (c) Shishqa, 2020 */
/*============================================================================*/

#include <cassert>

#include <cstddef>
#include <cstdio>
#include <queue>
#include <string>
#include <string_view>
#include <unordered_map>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

class SuffixTree {
public:

    using Char = char;
    using String = std::string_view;

    struct Node {
       
        Node(Node* parent, int l, int r);
    
        ~Node();

        void attach(Node* node, Char ch);

        int edgeLen() const;

        /*##########################################*/
        std::unordered_map<Char, Node*> children;

        Node* parent;
        Node* suff_link;

        int edge_l;
        int edge_r;
        /*##########################################*/

    };

    struct SuffTerminal {
        Node* node;
        int idx;
    };

    explicit SuffixTree(const Char* str);

    void widen(size_t len);

    bool has(const Char* str, size_t len);

    void dump(const char* label);

    ~SuffixTree();

private:

    void dump(int iter);

    void dump();

    Node* split(Node* node, int idx);

    void extend(int idx);

    SuffTerminal getLink(const SuffTerminal& terminal);

    void walk(SuffTerminal& terminal, Char ch);

    bool canWalk(const SuffTerminal& terminal, Char ch);

    bool isEdgeExtendable(const SuffTerminal& terminal);

    /*######################################*/
    Node* root;

    SuffTerminal lastPending;
    Node* waitingForLink;

    const char* buf;
    size_t buf_len;
    /*######################################*/

    FILE* DUMP_STREAM;
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

int main(int argc, char *argv[]) {

    char buffer[500000] = "";
    int string_len = 0;
    scanf("%s%n", buffer, &string_len);

    buffer[string_len] = '$';

    SuffixTree tree(buffer); 
    tree.widen(string_len + 1);

    return 0;
}

/*############################################################################*/

bool SuffixTree::has(const Char* str, size_t len) {

    SuffTerminal finder = {root, -1};

    for (size_t i = 0; i < len; ++i) {

        if (!canWalk(finder, str[i])) {
            printf("failed at len %lu\n", i);
            return false;
        }
   
        walk(finder, str[i]);
    }

    return true;
}

/*############################################################################*/

bool SuffixTree::isEdgeExtendable(const SuffTerminal& terminal) 
{
    return terminal.node->edge_r > terminal.idx;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

bool SuffixTree::canWalk(const SuffTerminal& terminal, Char ch) 
{    
    if (isEdgeExtendable(terminal)) {    
        return buf[terminal.idx + 1] == ch;
    } 
    return terminal.node->children.count(ch);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void SuffixTree::walk(SuffTerminal& terminal, Char ch) 
{
    assert(canWalk(terminal, ch));

    if (isEdgeExtendable(terminal)) {
        ++terminal.idx; 
    } else {
        terminal.node = terminal.node->children[ch];
        terminal.idx  = terminal.node->edge_l;
    }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

SuffixTree::SuffTerminal SuffixTree::getLink(const SuffTerminal& terminal) 
{
    if (terminal.node == root) {
        return terminal;
    }

    SuffTerminal link = {};

    /*          edge_idx
     *          |
     *    "absbde"   
     * .->@--------->@
     *    +          ^terminal.node
     *    +
     *    +- - - - - - - - - -suff_link
     *    +
     *    +++           link
     *      V           |
     * ..-->@-->@--->...-->@
     *      "ab""s...  .e" ^link.node
     */

    int read_from = 0;

    if (terminal.node->parent == root) {
        link.node = root;
        link.idx  = -1;
        read_from = terminal.node->edge_l + 1;
    } else {
        link.node = terminal.node->parent->suff_link;
        link.idx  = link.node->edge_r;
        read_from = terminal.node->edge_l;
    }

    int num_char_left = terminal.idx - read_from + 1;
    while (num_char_left) {
    
        link.node = link.node->children[buf[read_from]];

        if (link.node->edgeLen() >= num_char_left) {
            link.idx = link.node->edge_l + num_char_left - 1; 
            break;
        }

        num_char_left -= link.node->edgeLen();
        read_from     += link.node->edgeLen();

    }

    return link;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void SuffixTree::extend(int pos) 
{
    while (!canWalk(lastPending, buf[pos])) {

        Node* list = new Node(nullptr, pos, buf_len - 1);

        if (isEdgeExtendable(lastPending)) {

            Node* split_node = split(lastPending.node, lastPending.idx);
            split_node->attach(list, buf[pos]);

            lastPending.node = split_node;

            if (waitingForLink) {
                waitingForLink->suff_link = split_node;
            }

            waitingForLink = split_node;

        } else {

            lastPending.node->attach(list, buf[pos]);

            if (waitingForLink) {
                waitingForLink->suff_link = lastPending.node;
                waitingForLink = nullptr;
            }
        }

        if (lastPending.node == root) {

            if (waitingForLink) {
                waitingForLink->suff_link = root;
                waitingForLink = nullptr;
            }

            return;
        }

        lastPending = getLink(lastPending);
    }

    if (waitingForLink) {
        
        if (lastPending.idx == lastPending.node->edge_r) {
            waitingForLink->suff_link = lastPending.node;
        } else {    
            waitingForLink->suff_link = root;
        }

        waitingForLink = nullptr;
    }

    walk(lastPending, buf[pos]);
}

/*============================================================================*/

SuffixTree::Node* SuffixTree::split(Node* node, int idx) {

    assert(node->edge_l <= idx && idx < node->edge_r);

    /*        (idx)
     *            |  (idx+1)
     *  (edge_l)  |  |        (edge_r)
     *  @---------->@---------->@
     *  ^[parent]   ^[new_node] ^[node]
     */

    auto new_node = new Node(node->parent, node->edge_l, idx); 

    new_node->children[buf[idx + 1]] = node;
    node->parent->children[buf[node->edge_l]] = new_node;

    node->edge_l = idx + 1;
    node->parent = new_node;

    return new_node;
}

/*############################################################################*/

SuffixTree::SuffixTree(const Char* str)
    : root(nullptr)
    , lastPending({})
    , waitingForLink(nullptr)
    , buf(str)
    , buf_len(0) {

    root = new Node(nullptr, -1, -1); 
    lastPending = {root, -1};

    DUMP_STREAM = fopen("SuffTree-dump.dot", "w");
    fprintf(DUMP_STREAM, "digraph SuffixTree {\n");
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void SuffixTree::widen(size_t len) {

    buf_len += len;

    for (int i = buf_len - len; i < buf_len; ++i) {
        extend(i);
        dump(i);
    }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

SuffixTree::~SuffixTree() {

    fprintf(DUMP_STREAM, "}\n");
    fclose(DUMP_STREAM);

    delete root;
}

/*============================================================================*/

SuffixTree::Node::Node(Node* parent, int l, int r) 
        : parent(parent)
        , suff_link(this)
        , edge_l(l)
        , edge_r(r)
        { }

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

SuffixTree::Node::~Node() {
    for (auto& child : children) {
        delete child.second;
    }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

int SuffixTree::Node::edgeLen() const {
    return edge_r - edge_l + 1;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void SuffixTree::Node::attach(Node *node, Char ch) {
    node->parent = this;
    children[ch] = node;
}

/*############################################################################*/

void SuffixTree::dump(const char* label) {
 
    fprintf(DUMP_STREAM, 
            "subgraph cluster_%s {\n"
            "\tgraph [color=black; label=\"%s\"];\n"
            "\tnode [shape=point];\n",
            label, label);

    dump();

    fprintf(DUMP_STREAM, "}\n");
      
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void SuffixTree::dump(int iter) {

    fprintf(DUMP_STREAM, 
            "subgraph cluster_%d {\n"
            "\tgraph [color=black; label=\"Iteration %d : %.*s\"];\n"
            "\tnode [shape=point];\n",
            iter, iter, iter, buf);

    dump();

    fprintf(DUMP_STREAM, "}\n");
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void SuffixTree::dump() {
    
    static int helper = 0;

        std::queue<Node*> to_visit;
    to_visit.push(root);

    while (!to_visit.empty()) {

        Node* target = to_visit.front();
        to_visit.pop();

        fprintf(DUMP_STREAM, 
                "\t\"node%d%p\" [color=%s, label=\"\"];\n",
                helper, reinterpret_cast<void*>(target),
                (target == root) ? "violet" : "black");

        fprintf(DUMP_STREAM, 
                "\t\"node%d%p\" -> \"node%d%p\" [color=red];\n",
                helper, reinterpret_cast<void*>(target),
                helper, reinterpret_cast<void*>(target->suff_link)
                );

        for (auto& child : target->children) {

            to_visit.push(child.second);

            fprintf(DUMP_STREAM, 
                    "\t\"node%d%p\" -> \"node%d%p\" [label=\"%.*s\"];\n",
                    helper, reinterpret_cast<void*>(target),
                    helper, reinterpret_cast<Node*>(child.second),
                    child.second->edgeLen(), 
                    buf + child.second->edge_l
                    );
        }
    }

    fprintf(DUMP_STREAM, 
            "\t\"lastPending%d\" [shape=box,color=yellow,label=\"lastPending: idx=%d\"];\n"
            "\t\"lastPending%d\" -> \"node%d%p\" [style=dotted];\n",
            helper, lastPending.idx - lastPending.node->edge_l,
            helper, helper, reinterpret_cast<void*>(lastPending.node)
            );

    if (waitingForLink) {
        fprintf(DUMP_STREAM, 
                "\t\"waitingForLink%d\" [shape=box,color=green,label=\"waitingForLink\"];\n"
                "\t\"waitingForLink%d\" -> \"node%d%p\" [style=dotted];\n",
                helper, helper, helper, reinterpret_cast<void*>(waitingForLink)
                );
    }

    ++helper;
}

/*============================================================================*/
