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

    explicit SuffixTree(const Char* str, size_t len);

    ~SuffixTree();

private:

    void dump(FILE* fstream, int iter);

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

    unsigned long long n_leaves;
    unsigned long long n_substrings;

    const char* buf;
    size_t buf_len;
    /*######################################*/
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

int main(int argc, char *argv[]) {

    char buffer[500000] = "";
    int string_len = 0;
    scanf("%s%n", buffer, &string_len);

    buffer[string_len] = '$';

    SuffixTree tree(buffer, string_len + 1); 

    return 0;
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

        if (isEdgeExtendable(lastPending)) {

            Node* split_node = split(lastPending.node, lastPending.idx);
            
            split_node->children[buf[pos]] = 
                new Node(split_node, pos, buf_len);

            lastPending.node = split_node;

            if (waitingForLink) {
                waitingForLink->suff_link = split_node;
            }

            waitingForLink = split_node;

        } else {

            lastPending.node->children[buf[pos]] = 
                new Node(lastPending.node, pos, buf_len);

            if (waitingForLink) {
                waitingForLink->suff_link = lastPending.node;
                waitingForLink = nullptr;
            }
        }

        ++n_leaves;

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

SuffixTree::SuffixTree(const Char* str, size_t len)
    : root(nullptr)
    , lastPending({})
    , waitingForLink(nullptr)
    , buf(str)
    , buf_len(len) {

    root = new Node(nullptr, -1, -1); 

    //FILE* fstream = fopen("SuffTree-dump.dot", "w");

    //fprintf(fstream, "digraph SuffixTree {\n");

    lastPending = {root, -1};
    for (int i = 0; i < len; ++i) {
        //dump(fstream, i);
        n_substrings += n_leaves;
        extend(i);
        if (i > 0) {
            printf("%llu\n", n_substrings);
        }
    }
    //dump(fstream, len);

    //fprintf(fstream, "}\n");
    //fclose(fstream);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

SuffixTree::~SuffixTree() {
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

/*############################################################################*/

void SuffixTree::dump(FILE* fstream, int iter) {
    
    fprintf(fstream, 
            "subgraph cluster_%d {\n"
            "\tgraph [color=black; label=\"Iteration %d : %.*s\"];\n"
            "\tnode [shape=point];\n",
           iter, iter, iter, buf);

    std::queue<Node*> to_visit;
    to_visit.push(root);

    while (!to_visit.empty()) {

        Node* target = to_visit.front();
        to_visit.pop();

        fprintf(fstream, 
                "\t\"node%d%p\" [color=%s, label=\"\"];\n",
                iter, reinterpret_cast<void*>(target),
                (target == root) ? "violet" : "black");

        fprintf(fstream, 
                "\t\"node%d%p\" -> \"node%d%p\" [color=red];\n",
                iter, reinterpret_cast<void*>(target),
                iter, reinterpret_cast<void*>(target->suff_link)
                );

        for (auto& child : target->children) {

            to_visit.push(child.second);

            fprintf(fstream, 
                    "\t\"node%d%p\" -> \"node%d%p\" [label=\"%.*s\"];\n",
                    iter, reinterpret_cast<void*>(target),
                    iter, reinterpret_cast<Node*>(child.second),
                    child.second->edgeLen(), 
                    buf + child.second->edge_l
                    );
        }
    }

    fprintf(fstream, 
            "\t\"lastPending%d\" [shape=box,color=yellow,label=\"lastPending: idx=%d\"];\n"
            "\t\"lastPending%d\" -> \"node%d%p\" [style=dotted];\n",
            iter, lastPending.idx - lastPending.node->edge_l,
            iter, iter, reinterpret_cast<void*>(lastPending.node)
            );

    if (waitingForLink) {
        fprintf(fstream, 
                "\t\"waitingForLink%d\" [shape=box,color=green,label=\"waitingForLink\"];\n"
                "\t\"waitingForLink%d\" -> \"node%d%p\" [style=dotted];\n",
                iter, iter, iter, reinterpret_cast<void*>(waitingForLink)
                );
    }

    fprintf(fstream, 
            "}\n");
}

/*============================================================================*/
