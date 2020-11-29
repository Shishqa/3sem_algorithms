/* (c) Shishqa, 2020 */
/*============================================================================*/

#include <cassert>

#include <stack>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <string_view>
#include <algorithm>

#ifdef VERBOSE
#define V( smth ) smth
#else
#define V( smth )
#endif

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

class SuffixArray {
public:

    struct Suffix {
        long long index;
        long long rank[2];
    };

    struct CommonPrefix {
        long long index;
        long long len;
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
                           const long long& rk);

    bool arr_built;
    std::vector<Suffix> arr;

    bool lcp_built;
    std::vector<CommonPrefix> lcp;
};

/*============================================================================*/

int main() {
   
    char buffer[700005] = "";
    scanf("%s", buffer);

    long long str_len = strnlen(buffer, sizeof(buffer));

    SuffixArray suff_arr(buffer, str_len); 

    V(
    printf("sorted:\n");
    for (auto& suff : suff_arr.array()) {
        printf("%s - {%lld, %lld}\n", buffer + suff.index, 
                                  suff.rank[0], 
                                  suff.rank[1]);
    }
    printf("\n\n");

    printf("lcp:\n");
    for (auto& cp : suff_arr.LCP()) {
        printf("%lld - %lld\n", cp.index, cp.len);
    }
    printf("\n\n");
    )

    long long ans = str_len;

    std::stack<long long> last;

    std::vector<long long> L(suff_arr.LCP().size(), 10000000LL);
    std::vector<long long> R(suff_arr.LCP().size(), -10000000LL);
    
    long long lcp = suff_arr.LCP()[0].len;

    for (size_t i = 1; i < suff_arr.LCP().size(); ++i) {

        V(
        printf("================================\n");
        printf("lcp=%lld\n", lcp);

        printf("L: ");
        for (long long k = 0; k <= lcp; ++k) {
            printf("%lld ", L[k]);
        }
        printf("\n");

        printf("R: ");
        for (long long k = 0; k <= lcp; ++k) {
            printf("%lld ", R[k]);
        }
        printf("\n");

        printf("new-lcp=%lld\n", suff_arr.LCP()[i].len);
        )

        if (suff_arr.LCP()[i].len > lcp) {
    
            last.push(lcp);
            lcp = suff_arr.LCP()[i].len;

            L[lcp] = suff_arr.LCP()[i - 1].index;            
            R[lcp] = suff_arr.LCP()[i - 1].index;            
             
            L[lcp] = std::min(L[lcp], suff_arr.LCP()[i].index);
            R[lcp] = std::max(R[lcp], suff_arr.LCP()[i].index);
       
        } else if (suff_arr.LCP()[i].len < lcp) {

            long long res = R[lcp] - L[lcp] + lcp + lcp * lcp;
            if (ans < res) {
                ans = res;
            }
            V(
            printf("res:: lcp=%lld (%lld;%lld) -> %lld\n",
                    lcp, L[lcp], R[lcp], res);
            ) 

            while (!last.empty() && last.top() > suff_arr.LCP()[i].len) {
                
                long long k = last.top();
                last.pop();

                L[k] = std::min(L[k], L[lcp]);
                R[k] = std::max(R[k], R[lcp]);

                if (R[lcp] == 10000000LL || L[lcp] == -10000000LL) {
                    abort();
                }

                long long res = R[k] - L[k] + k + k * k;
                if (ans < res) {
                    ans = res;
                }
    
                V(
                printf("res:: lcp=%lld (%lld;%lld) -> %lld\n",
                        lcp, L[lcp], R[lcp], res);
                )

                lcp = k;
            }

            if (last.top() == suff_arr.LCP()[i].len) {
                
                last.pop();
            
            } else {
                
                L[suff_arr.LCP()[i].len] = L[lcp];
                R[suff_arr.LCP()[i].len] = R[lcp];
            }

            lcp = suff_arr.LCP()[i].len;

        } else {
            
            L[lcp] = std::min(L[lcp], suff_arr.LCP()[i].index);
            R[lcp] = std::max(R[lcp], suff_arr.LCP()[i].index);

        }

        V(
        printf("================================\n");
        )
    }
 
    while (!last.empty()) {
                
        long long k = last.top();
        last.pop();

        L[k] = std::min(L[k], L[lcp]);
        R[k] = std::max(R[k], R[lcp]);

                if (R[lcp] == 10000000LL || L[lcp] == -10000000LL) {
                    abort();
                }

        long long res = R[lcp] - L[lcp] + lcp + lcp * lcp;
        if (ans < res) {
            ans = res;
        }
   
        V(
        printf("res:: lcp=%lld (%lld;%lld) -> %lld\n",
                lcp, L[lcp], R[lcp], res);
        )    
    
        lcp = k;
    }

    printf("%lld\n", ans);

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

    std::vector<long long> where(arr.size(), 0);

    for (size_t h = 4; h < 2 * arr.size(); h *= 2) {

        long long n_ranks = 1;
        long long prev_rank = arr[0].rank[0];

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

            long long next_half = arr[i].index + h / 2;
                
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
                             const long long& rk) {

    std::vector<long long> cnt(std::max(input.size() + 10, static_cast<size_t>(('z' - 'a') + 10)), 0);
    output.resize(input.size());

    for (size_t i = 0; i < input.size(); ++i) {
        cnt[input[i].rank[rk]]++; 
    }
   
    long long count = 0;
    for (size_t i = 0; i < cnt.size(); ++i) {
        long long tmp = cnt[i];
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

    if (!arr_built) {
        build_array();
    }

    std::vector<long long> where(arr.size());
    for (size_t i = 0; i < arr.size(); ++i) {
        where[arr[i].index] = i;
    }

    long long k = 0;
    for (int i = 0; i < arr.size(); ++i) {

        if (k > 0) {
            --k;
        }

        if (where[i] == 0) {
         
            lcp[0] = {arr[0].index, 0};
            k = 0;
            continue;

        } else {

            long long j = arr[where[i] - 1].index;
            
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
