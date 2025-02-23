#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <limits>
#include <algorithm>

using namespace std;

double euclidean_distance(const pair<double, double>& a, const pair<double, double>& b) {
    return sqrt(pow(a.first - b.first, 2) + pow(a.second - b.second, 2));
}

vector<vector<double>> build_distance_matrix(const vector<pair<double, double>>& coords) {
    int N = coords.size();
    vector<vector<double>> mat(N, vector<double>(N, 0.0));

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

vector<int> two_opt(vector<int> route, const vector<vector<double>>& mat) {
    bool improved = true;
    int N = route.size();

    while (improved) {
        improved = false;
        for (int i = 1; i < N - 2; i++) {
            for (int j = i + 1; j < N - 1; j++) {
                double old_dist = mat[route[i - 1]][route[i]] + mat[route[j]][route[j + 1]];
                double new_dist = mat[route[i - 1]][route[j]] + mat[route[i]][route[j + 1]];

                if (new_dist < old_dist) {
                    reverse(route.begin() + i, route.begin() + j + 1);
                    improved = true;
                }
            }
        }
    }
    return route;
}

vector<int> three_opt(vector<int> route, const vector<vector<double>>& mat) {
    int N = route.size();
    bool improved = true;
    double best_dist = total_distance(route, mat);

    while (improved) {
        improved = false;
        vector<int> best_route = route;

        for (int i = 1; i < N - 4; i++) {
            for (int j = i + 1; j < N - 3; j++) {
                for (int k = j + 1; k < N - 1; k++) {
                    if (i < N && j < N && k < N) {
                        vector<int> part1(route.begin(), route.begin() + i);
                        vector<int> part2(route.begin() + i, route.begin() + j);
                        vector<int> part3(route.begin() + j, route.begin() + k);
                        vector<int> part4(route.begin() + k, route.end());

                        if (part1.empty() || part2.empty() || part3.empty() || part4.empty()) {
                            continue;
                        }

                        vector<int> part2_rev(part2);
                        reverse(part2_rev.begin(), part2_rev.end());
                        vector<int> part3_rev(part3);
                        reverse(part3_rev.begin(), part3_rev.end());

                        vector<vector<int>> possible_routes = {
                                part1, part2, part3, part4, // Оригинал
                                part1, part2_rev, part3, part4,  // Реверс 2-го
                                part1, part2, part3_rev, part4,  // Реверс 3-го
                                part1, part2_rev, part3_rev, part4,  // Реверс 2-го и 3-го
                                part1, part3, part2, part4,  // Меняем 2-й и 3-й
                                part1, part3_rev, part2, part4,  // Меняем и реверс 3-го
                                part1, part2, part4, part3,  // Меняем 3-й и 4-й
                                part1, part4, part3, part2   // Полностью переворачиваем порядок
                        };

                        for (auto& new_route : possible_routes) {
                            if (new_route.empty()) continue;

                            double new_dist = total_distance(new_route, mat);
                            if (new_dist < best_dist) {
                                best_dist = new_dist;
                                best_route = new_route;
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
    ifstream file("tsp_5_1");
    int N;
    file >> N;

    vector<pair<double, double>> coords(N);
    for (int i = 0; i < N; i++) {
        file >> coords[i].first >> coords[i].second;
    }
    file.close();

    vector<vector<double>> dist_matrix = build_distance_matrix(coords);

    vector<int> initial_route(N);
    for (int i = 0; i < N; i++) initial_route[i] = i;

    vector<int> optimized_route = three_opt(initial_route, dist_matrix);
    double optimized_distance = total_distance(optimized_route, dist_matrix);

    cout << fixed << optimized_distance << " 1\n";
    for (int i : optimized_route) cout << i << " ";
    cout << endl;

    return 0;
}
