/* (c) Shishqa, 2020 */
/*==========================================================================*/

#include <iostream>
#include <string>
#include <vector>

/*==========================================================================*/

std::vector<size_t> prefix_function(const std::string& str) {

    std::vector<size_t> pf(str.size(), 0);

    for (size_t i = 1; i < pf.size(); ++i) {

        size_t k = pf[i - 1];
        while (k > 0 && str[k] != str[i]) {
            k = pf[k - 1];
        }
        if (str[i] == str[k]) {
            ++k;
        }

        pf[i] = k;
    }

    return pf;
}

/*==========================================================================*/

int main(int argc, char *argv[]) {

    std::string s = "", t = "";

    std::cin >> s >> t;
    const size_t t_size = t.size();

    std::vector<size_t> pf = prefix_function(t + "#" + s);

    for (size_t i = t.size() + 1; i < pf.size(); ++i) {
        if (t_size == pf[i]) {
            std::cout << i - 2 * t_size << ' ';
        }
    }

    return 0;
}

/*==========================================================================*/
