/* (c) Shishqa, 2020 */
/*============================================================================*/

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <string_view>
#include <algorithm>

/*############################################################################*/

#include <cstdio>
#include <vector>
 
template <class Key>
class SparseTable {
 
    typedef Key key_t;
 
public:
 
    key_t get_max(const size_t& l_idx, const size_t& r_idx);
 
    explicit SparseTable(const std::vector<key_t>& init_data);
 
    SparseTable() = delete;
 
    SparseTable(const SparseTable& other) = delete;
 
    ~SparseTable();
 
private:
 
    void fill_table(const std::vector<key_t>& init_data);
 
    key_t** table;
 
    const size_t size;
 
    size_t height;
    size_t* log2;
    size_t* pow2;
};
 
struct Key {
 
    size_t idx;
    int value;
 
    Key() = default;
 
    Key(const Key& other) = default;
 
    ~Key() = default;
};
 
bool operator<(const Key& key1, const Key& key2);

/*############################################################################*/

class SuffixArray {
public:

    struct Suffix {
        int index;
        int rank[2];
    };

    struct CommonPrefix {
        int index;
        int len;
    };

    explicit SuffixArray(const char* string, const size_t& len);

    void build_array();
    const std::vector<Suffix>& array();

    const Suffix& operator[](const size_t idx);

    void build_LCP();
    const std::vector<CommonPrefix>& LCP();

    SuffixArray() = delete;

private:

    const char* string;

    void sort_suffixes();

    static void count_sort(std::vector<Suffix>& input, 
                           std::vector<Suffix>& output,
                           const int& rk);

    bool arr_built;
    std::vector<Suffix> arr;

    bool lcp_built;
    std::vector<CommonPrefix> lcp;
};

/*============================================================================*/

int main() {
   
    char buffer[100001] = "";
    scanf("%s", buffer);

    size_t str_len = strnlen(buffer, sizeof(buffer));

    SuffixArray suff_arr(buffer, str_len); 

    printf("sorted:\n");
    for (auto& suff : suff_arr.array()) {
        printf("%s - {%d, %d}\n", buffer + suff.index, 
                                  suff.rank[0], 
                                  suff.rank[1]);
    }
    printf("\n\n");

    printf("lcp:\n");
    for (auto& cp : suff_arr.LCP()) {
        printf("%d - %d\n", cp.index, cp.len);
    }
    printf("\n\n");

    std::vector<Key> table_data(suff_arr.array().size());
    for (size_t i = 0; i < table_data.size(); ++i) {
        table_data[i].value = suff_arr.LCP()[i].len;
        table_data[i].idx = i;
    }


    return 0;
}

/*============================================================================*/

SuffixArray::SuffixArray(const char* str, const size_t& len)
        : string(str)
        , arr_built(false)
        , lcp_built(false) {
    arr.resize(len);
    lcp.resize(len);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

const std::vector<SuffixArray::Suffix>& SuffixArray::array() {

    if (!arr_built) {
        build_array();
    }

    return arr;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

const SuffixArray::Suffix& SuffixArray::operator[](const size_t idx) {
    return array()[idx];
}

/*============================================================================*/

void SuffixArray::build_array() {

    if (arr_built) {
        return;
    }

    for (size_t i = 0; i < arr.size(); ++i) {
        arr[i].index = i;
        arr[i].rank[0] = string[i] - 'a' + 1;
        arr[i].rank[1] = (i + 1 < arr.size() ? string[i + 1] - 'a' + 1 : 0); 
    }

    sort_suffixes();

    std::vector<int> where(arr.size(), 0);

    for (size_t h = 4; h < 2 * arr.size(); h *= 2) {

        int n_ranks = 1;
        int prev_rank = arr[0].rank[0];

        arr[0].rank[0] = n_ranks;
        where[arr[0].index] = 0;     

        for (size_t i = 1; i < arr.size(); ++i) {

            if (arr[i].rank[0] == prev_rank &&
                arr[i].rank[1] == arr[i - 1].rank[1]) {

                prev_rank = arr[i].rank[0];
                arr[i].rank[0] = n_ranks;

            } else {

                prev_rank = arr[i].rank[0];
                arr[i].rank[0] = ++n_ranks;

            }

            where[arr[i].index] = i;
        }

        for (size_t i = 0; i < arr.size(); ++i) {

            int next_half = arr[i].index + h / 2;
                
            arr[i].rank[1] = 0;
            if (next_half < arr.size()) {
                arr[i].rank[1] = arr[where[next_half]].rank[0];
            }
        }
        sort_suffixes();
    }

    arr_built = true;
}

/*============================================================================*/

void SuffixArray::sort_suffixes() {

    std::vector<Suffix> tmp(arr.size());
       
    count_sort(arr, tmp, 1);
    count_sort(tmp, arr, 0);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void SuffixArray::count_sort(std::vector<Suffix>& input, 
                             std::vector<Suffix>& output,
                             const int& rk) {

    std::vector<int> cnt(std::max(input.size() + 10, static_cast<size_t>(('z' - 'a') + 10)), 0);
    output.resize(input.size());

    for (size_t i = 0; i < input.size(); ++i) {
        cnt[input[i].rank[rk]]++; 
    }
   
    int count = 0;
    for (size_t i = 0; i < cnt.size(); ++i) {
        int tmp = cnt[i];
        cnt[i] = count;
        count += tmp;
    }
   
    for (int i = 0; i < input.size(); ++i) {
        output[cnt[input[i].rank[rk]]++] = input[i];
    }
}

/*============================================================================*/

const std::vector<SuffixArray::CommonPrefix>& SuffixArray::LCP() {

    if (!lcp_built) {
        build_LCP();
    }

    return lcp;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void SuffixArray::build_LCP() {

    if (lcp_built) {
        return;
    }

    std::vector<int> where(arr.size());
    for (size_t i = 0; i < arr.size(); ++i) {
        where[arr[i].index] = i;
    }

    int k = 0;
    for (int i = 0; i < arr.size(); ++i) {

        if (k > 0) {
            --k;
        }

        if (where[i] == 0) {
         
            lcp[0] = {arr[0].index, 0};
            k = 0;
            continue;

        } else {

            int j = arr[where[i] - 1].index;
            
            while (std::max(i + k, j + k) < arr.size() &&
                   string[i + k] == string[j + k]) {
                ++k;
            }

            lcp[where[i]] = {i, k};
        }
    }

    lcp_built = true;
}
/*############################################################################*/

bool operator<(const Key& key1, const Key& key2) {
 
    if (key1.value == key2.value) {
 
        return key1.idx < key2.idx;
 
    }
 
    return key1.value < key2.value;
}
 
template <class Key>
SparseTable<Key>::SparseTable(const std::vector<key_t>& init_data) :
        size(init_data.size()),
        height(0) {
 
    log2 = new size_t[size + 1];
 
    for (size_t i = 1, pow = 1; i <= size; ++i) {
 
        if (i == pow) {
            pow <<= 1;
            ++height;
        }
 
        log2[i] = height - 1;
    }
 
    pow2 = new size_t[height];
 
    for (size_t i = 0, pow = 1; i < height; ++i, pow <<= 1) {
        pow2[i] = pow;
    }
 
    table = new key_t* [height];
 
    for (size_t i = 0; i < height; ++i) {
        table[i] = new key_t[size];
    }
 
    fill_table(init_data);
}
 
template <class Key>
void SparseTable<Key>::fill_table(const std::vector<key_t>& init_data) {
 
    for (size_t j = 0; j < size; ++j) {
        table[0][j] = init_data[j];
    }
 
    for (size_t step = 1, i = 1; i < height; ++i, step <<= 1) {
        for (size_t j = 0; j + 2 * step <= size; ++j) {
 
            table[i][j] = std::max(table[i - 1][j], table[i - 1][j + step]);
 
        }
    }
}
 
template <class Key>
Key SparseTable<Key>::get_max(const size_t& l_idx, const size_t& r_idx) {
 
    auto degree = log2[r_idx - l_idx + 1];
    return std::max(table[degree][l_idx], table[degree][r_idx - pow2[degree] + 1]);
}
 
template <class Key>
SparseTable<Key>::~SparseTable() {
 
    for (size_t i = 0; i < height; ++i) {
        delete[] table[i];
    }
 
    delete[] table;
    delete[] log2;
    delete[] pow2;
}

/*============================================================================*/
