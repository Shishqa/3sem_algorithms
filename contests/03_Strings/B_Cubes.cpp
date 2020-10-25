/* (c) Shishqa, 2020 */
/*==========================================================================*/

#include <cstdio>
#include <new>
#include <vector>

/*==========================================================================*/

using Char = int;
using String = std::vector<Char>;

/*==========================================================================*/

String make_expanded_str(const String& str, const Char& term_sym) {

    /* printf("old str:\t"); */
    /* for (const auto& elem : str) { */
    /*     printf("%d ", elem); */
    /* } */
    /* printf("\n"); */

    String new_str(2 * str.size() + 3);
    new_str[0] = -1;
    new_str[1] = term_sym;

    for (size_t i = 0; i < str.size(); ++i) {
        new_str[2 * i + 2] = str[i];
        new_str[2 * i + 3] = term_sym;
    }

    new_str[2 * str.size() + 2] = -2;

/*     printf("new str:\t"); */
/*     for (const auto& elem : new_str) { */
/*         printf("%d ", elem); */
/*     } */
/*     printf("\n"); */

    return new_str;
}

/*==========================================================================*/

std::vector<int> manachar(const String& str, const Char& term_sym) {

    String exp_str = make_expanded_str(str, term_sym);
    std::vector<int> poly_len(exp_str.size(), 0);

    int curr_center = 0,
        curr_right  = 0;
   
    for (int i = 1; i < exp_str.size() - 1; ++i) {

        int i_mirror = curr_center - (i - curr_center);

        if (curr_right > i) {
            poly_len[i] = std::min(curr_right - i, poly_len[i_mirror]);
        }

        while (exp_str[i + 1 + poly_len[i]] == exp_str[i - 1 - poly_len[i]]) {
            ++poly_len[i];
        }

        if (i + poly_len[i] > curr_right) {
            curr_center = i;
            curr_right = i + poly_len[i];
        }
    }

    return poly_len;
}

/*==========================================================================*/

int main(int argc, char *argv[]) {

    size_t line_size = 0,
           n_colors  = 0;

    scanf("%lu %lu", &line_size, &n_colors);

    String line(line_size); 
    for (auto& cube : line) {
        scanf("%d", &cube);
    }

    std::vector<int> poly_len = manachar(line, 0);

/*     printf("manachar:\t"); */
/*     for (const auto& elem : poly_len) { */
/*         printf("%d ", elem); */
/*     } */
/*     printf("\n"); */

    for (int i = poly_len.size() - 2; i > 1; --i) {
        /* for (int j = 0; j < poly_len[i]; ++j) { */
        /*     printf("%d ", line[(i - 1 - poly_len[i]) / 2 + j]); */
        /* } */
        if ((i - 1 - poly_len[i]) / 2 == 0 && poly_len[i] % 2 == 0) {
            printf("%d ", static_cast<int>(line.size()) - poly_len[i] / 2);
        }
    }
    printf("%lu\n", line.size());

    return 0;
}

/*==========================================================================*/
