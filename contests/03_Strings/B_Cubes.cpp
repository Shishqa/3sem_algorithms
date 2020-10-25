/* (c) Shishqa, 2020 */
/*==========================================================================*/

#include <cstdio>
#include <string>
#include <vector>

#include <iostream>


/*==========================================================================*/

std::vector<int> z_function(const std::vector<int>& line) {

    std::vector<int> zf(line.size(), 0);

    int left  = 0,
        right = 0;

    for (int i = 1; i < zf.size(); ++i) {

        zf[i] = std::max(0, std::min(right - i, zf[i - left]));

        while (i + zf[i] < zf.size() && line[zf[i]] == line[i + zf[i]]) {
            ++zf[i];
        }
        if (i + zf[i] > right) {

            left = i;
            right = i + zf[i];

        }

    }

    return zf;
}

/*==========================================================================*/

int main(int argc, char *argv[]) {

    size_t line_size = 0,
           n_colors  = 0;

    scanf("%lu %lu", &line_size, &n_colors);

    std::vector<int> line(line_size); 
    for (auto& cube : line) {
        scanf("%d", &cube);
    }

    const std::vector<int> zf = z_function(line);

    for (int i = line_size - 1; i >= 0; --i) {

        if (zf[i])

    }

    return 0;
}

/*==========================================================================*/
