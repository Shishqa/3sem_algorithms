/* (c) Shishqa, 2020 */
/*============================================================================*/

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <utility>
#include <vector>
#include <string_view>
#include <stack>

/*============================================================================*/

/* 2 sem -- Histogram problem */

typedef unsigned long long uint_ll;

struct MaxArea {
    size_t idx;
    uint_ll area;
};

void set_border(std::stack <std::pair <size_t, uint_ll>>& stk,
				uint_ll curr_height, size_t curr_index, size_t index_to_set,
				std::vector <int>& possible_border);

void fill_nil_borders(std::stack <std::pair <size_t, uint_ll>>& stk,
					  size_t nil_value, std::vector <int>& possible_border);

MaxArea max_area(const std::vector<uint_ll>& arr);

/*============================================================================*/

class SuffixArray {
public:

    struct Suffix {
        int index;
        int rank[2];
    };

    using CommonPrefix = uint_ll;

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
   
    size_t str_len = 0;
    scanf("%lu %*d", &str_len);

    int value = 0;
    char buffer[150001] = "";
    for (size_t i = 0; i < str_len; ++i) {
        scanf("%d", &value); 
        buffer[i] = ('0' - 1) + value;
    }

    SuffixArray suff_arr(buffer, str_len); 

    MaxArea max_ar = max_area(suff_arr.LCP());

    if (max_ar.area < str_len) {

        printf("%lu\n%lu\n", str_len, str_len);
        for (size_t i = 0; i < str_len; ++i) {
            printf("%d ", buffer[i] + 1 - '0');
        }
        printf("\n");

        return 0;
    }

    printf("%llu\n%llu\n", max_ar.area, suff_arr.LCP()[max_ar.idx]);
    for (size_t i = 0; i < suff_arr.LCP()[max_ar.idx]; ++i) {
        printf("%d ", *(buffer + suff_arr[max_ar.idx].index + i) + 1 - '0');
    }
    printf("\n");

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
        arr[i].rank[0] = string[i] - '0' + 1;
        arr[i].rank[1] = (i + 1 < arr.size() ? string[i + 1] - '0' + 1 : 0); 
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

    std::vector<int> cnt(std::max(input.size() + 10, static_cast<size_t>(11)), 0);
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

    if (!arr_built) {
        build_array();   
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
         
            lcp[0] = 0;
            k = 0;
            continue;

        } else {

            int j = arr[where[i] - 1].index;
            
            while (std::max(i + k, j + k) < arr.size() &&
                   string[i + k] == string[j + k]) {
                ++k;
            }

            lcp[where[i]] = k;
        }
    }

    lcp_built = true;
}

/*============================================================================*/

void set_border(std::stack <std::pair <size_t, uint_ll>>& stk,
				uint_ll curr_height, size_t curr_index, size_t index_to_set,
				std::vector <int>& possible_border)
{
	while (!stk.empty() && stk.top().second > curr_height)
	{
		possible_border[stk.top().first] = index_to_set;
		stk.pop();
	}
	stk.push(std::make_pair(curr_index, curr_height));
}
 
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void fill_nil_borders(std::stack <std::pair <size_t, uint_ll>>& stk,
					  size_t nil_value, std::vector <int>& possible_border)
{
	while (!stk.empty())
	{
		possible_border[stk.top().first] = nil_value;
		stk.pop();
	}
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

MaxArea max_area(const std::vector<uint_ll>& arr)
{
	std::stack <std::pair <size_t, uint_ll>> stk_lefts  = {};
	std::stack <std::pair <size_t, uint_ll>> stk_rights = {};
 
	std::vector <int> right_possible_border(arr.size());
	std::vector <int> left_possible_border(arr.size());
 
	for (size_t i = 0, j = arr.size() - 1; i < arr.size(); ++i, --j)
	{
		set_border(stk_lefts,  arr[i], i, i - 1,          right_possible_border);
		set_border(stk_rights, arr[j], j, arr.size() - i, left_possible_border);
	}
 
	fill_nil_borders(stk_lefts,  arr.size() - 1, right_possible_border);
	fill_nil_borders(stk_rights, 0,              left_possible_border);
 
    MaxArea max_area = {0, 0};
	uint_ll curr_area = 0;
 
	for (size_t i = 0; i < arr.size(); ++i)
	{
		curr_area = arr[i] * (right_possible_border[i] - left_possible_border[i] + 2);
        if (curr_area > max_area.area) {
            max_area = {i, curr_area};
        }
	}
 
	return max_area;
}

/*============================================================================*/
