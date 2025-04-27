#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <limits>
#include <algorithm>
#include <omp.h>
#include <chrono>

using namespace std;
using namespace chrono;

double euclidean_distance(const pair<double, double>& a, const pair<double, double>& b) {
    return sqrt(pow(a.first - b.first, 2) + pow(a.second - b.second, 2));
}

vector<vector<double>> build_distance_matrix(const vector<pair<double, double>>& coords) {
    int N = coords.size();
    vector<vector<double>> mat(N, vector<double>(N, 0.0));

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            double dist = euclidean_distance(coords[i], coords[j]);
            mat[i][j] = mat[j][i] = dist;
        }
    }
    return mat;
}

double total_distance(const vector<int>& route, const vector<vector<double>>& mat) {
    double dist = 0.0;
    for (size_t i = 0; i < route.size() - 1; i++) {
        dist += mat[route[i]][route[i + 1]];
    }
    return dist;
}

vector<int> three_opt(vector<int> route, const vector<vector<double>>& mat) {
    int N = route.size();
    if (N < 6) return route;

    bool improved = true;
    double best_dist = total_distance(route, mat);
    vector<int> best_route = route;

    auto concat = [](const vector<int>& a, const vector<int>& b,
                     const vector<int>& c, const vector<int>& d) {
        vector<int> result;
        result.insert(result.end(), a.begin(), a.end());
        result.insert(result.end(), b.begin(), b.end());
        result.insert(result.end(), c.begin(), c.end());
        result.insert(result.end(), d.begin(), d.end());
        return result;
    };

    while (improved) {
        improved = false;

        #pragma omp parallel for collapse(3) schedule(dynamic)
        for (int i = 1; i < N - 4; ++i) {
            for (int j = i + 1; j < N - 3; ++j) {
                for (int k = j + 1; k < N - 1; ++k) {
                    vector<int> part1(route.begin(), route.begin() + i);
                    vector<int> part2(route.begin() + i, route.begin() + j);
                    vector<int> part3(route.begin() + j, route.begin() + k);
                    vector<int> part4(route.begin() + k, route.end());

                    if (part1.empty() || part2.empty() || part3.empty() || part4.empty()) continue;

                    vector<int> part2_rev = part2;
                    vector<int> part3_rev = part3;
                    reverse(part2_rev.begin(), part2_rev.end());
                    reverse(part3_rev.begin(), part3_rev.end());

                    vector<vector<int>> candidates = {
                        concat(part1, part2, part3, part4),
                        concat(part1, part2_rev, part3, part4),
                        concat(part1, part2, part3_rev, part4),
                        concat(part1, part2_rev, part3_rev, part4),
                        concat(part1, part3, part2, part4),
                        concat(part1, part3_rev, part2, part4),
                        concat(part1, part2, part4, part3),
                        concat(part1, part4, part3, part2)
                    };

                    for (const auto& cand : candidates) {
                        double cand_dist = total_distance(cand, mat);

                        #pragma omp critical
                        {
                            if (cand_dist < best_dist) {
                                best_dist = cand_dist;
                                best_route = cand;
                                improved = true;
                            }
                        }
                    }
                }
            }
        }

        if (improved) route = best_route;
    }

    return route;
}

int main() {
    ifstream file("tsp_70_1");
    int N;
    file >> N;

    vector<pair<double, double>> coords(N);
    for (int i = 0; i < N; i++) {
        file >> coords[i].first >> coords[i].second;
    }
    file.close();

    auto start_dist = high_resolution_clock::now();
    vector<vector<double>> dist_matrix = build_distance_matrix(coords);
    auto end_dist = high_resolution_clock::now();

    vector<int> initial_route(N);
    for (int i = 0; i < N; i++) initial_route[i] = i;

    auto start_opt = high_resolution_clock::now();
    vector<int> optimized_route = three_opt(initial_route, dist_matrix);
    auto end_opt = high_resolution_clock::now();

    double optimized_distance = total_distance(optimized_route, dist_matrix);

    cout << fixed << optimized_distance << " 1\n";
    for (int i : optimized_route) cout << i << " ";
    cout << endl;

    auto dist_time = duration_cast<milliseconds>(end_dist - start_dist).count();
    auto opt_time = duration_cast<milliseconds>(end_opt - start_opt).count();

    cout << "\n[Time] Distance matrix: " << dist_time << " ms\n";
    cout << "[Time] Optimization:    " << opt_time << " ms\n";

    return 0;
}