/* (c) Shishqa, 2020 */
/*============================================================================*/

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

    explicit SuffixArray(const char* string, const size_t& len);

    const std::vector<Suffix>& array();

    SuffixArray() = delete;

private:

    const char* string;

    void sort_suffixes();

    static void count_sort(std::vector<Suffix>& input, 
                           std::vector<Suffix>& output,
                           const int& rk);

    std::vector<Suffix> answer;

};

/*============================================================================*/

int main() {
   
    char buffer[400001] = "";
    scanf("%s", buffer);

    SuffixArray suff_arr(buffer, strlen(buffer)); 

    for (auto& suff : suff_arr.array()) {
        printf("%d ", suff.index + 1);
    }

    return 0;
}

/*============================================================================*/

const std::vector<SuffixArray::Suffix>& SuffixArray::array() {
    return answer;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

SuffixArray::SuffixArray(const char* str, const size_t& len)
        : string(str) {

    answer.resize(len);

    for (size_t i = 0; i < answer.size(); ++i) {
        answer[i].index = i;
        answer[i].rank[0] = str[i] - 'a' + 1;
        answer[i].rank[1] = (i + 1 < answer.size() ? str[i + 1] - 'a' + 1 : 0); 
    }

    sort_suffixes();

    std::vector<int> where(answer.size(), 0);

    for (size_t h = 4; h < 2 * answer.size(); h *= 2) {

        int n_ranks = 1;
        int prev_rank = answer[0].rank[0];

        answer[0].rank[0] = n_ranks;
        where[answer[0].index] = 0;     

        for (size_t i = 1; i < answer.size(); ++i) {

            if (answer[i].rank[0] == prev_rank &&
                answer[i].rank[1] == answer[i - 1].rank[1]) {

                prev_rank = answer[i].rank[0];
                answer[i].rank[0] = n_ranks;

            } else {

                prev_rank = answer[i].rank[0];
                answer[i].rank[0] = ++n_ranks;

            }

            where[answer[i].index] = i;
        }

        for (size_t i = 0; i < answer.size(); ++i) {

            int next_half = answer[i].index + h / 2;
                
            answer[i].rank[1] = 0;
            if (next_half < answer.size()) {
                answer[i].rank[1] = answer[where[next_half]].rank[0];
            }
        }
        sort_suffixes();
    }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void SuffixArray::sort_suffixes() {

    std::vector<Suffix> tmp(answer.size());
       
    /*
    printf("arranged:\n");
    for (auto& suff : answer) {
        printf("%s - {%d, %d}\n", string + suff.index, suff.rank[0], suff.rank[1]);
    }
    printf("\n\n");
    */

    count_sort(answer, tmp, 1);
    count_sort(tmp, answer, 0);

    /*
    printf("sorted:\n");
    for (auto& suff : answer) {
        printf("%s - {%d, %d}\n", string + suff.index, suff.rank[0], suff.rank[1]);
    }
    printf("\n\n");
    */
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
