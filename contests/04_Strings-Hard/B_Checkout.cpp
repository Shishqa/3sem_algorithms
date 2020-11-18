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

/*============================================================================*/

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

    unsigned long long stat = 0;
    scanf("%llu", &stat);

    size_t str_len = strnlen(buffer, sizeof(buffer));

    SuffixArray suff_arr(buffer, str_len); 

    /*
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
    */

    unsigned long long cnt = 0;
    for (size_t i = 0; i < suff_arr.array().size(); ++i) {
        
        unsigned long long delta = str_len - 
                                   (suff_arr[i].index + suff_arr.LCP()[i].len);

        if (delta < stat - cnt) {
            cnt += delta;
            continue;
        }

        buffer[str_len - delta + (stat - cnt)] = 0;
        printf("%s\n", buffer + suff_arr[i].index);
        return 0;
    }

    printf("%s\n", buffer + suff_arr[suff_arr.array().size() - 1].index);
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

/*============================================================================*/
