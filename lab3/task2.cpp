#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <set>
#include <omp.h>

using namespace std;
typedef long double ld;

struct MatrixPC {
    int n;
    vector<vector<ld>> data;
    MatrixPC(int size) : n(size), data(size, vector<ld>(size, 0.0)) {
        for (int i = 0; i < n; ++i) data [i][i] = 1.0;
    }
};

void fillMatrix(MatrixPC &mpc) {
    cout << "введіть елементи" << endl;
    for (int i = 0; i < mpc.n; ++i) {
        for (int j = i + 1; j < mpc.n; ++j) {
            cout << "A[" << i + 1 << "][" << j + 1 << "]:";
            ld value;
            cin >> value;
            if (value > 0) {
                mpc.data[i][j] = value;
                mpc.data[j][i] = 1.0/value;
            } else {
                mpc.data[i][j] = 0;
                mpc.data[j][i] = 0;
            }
        }
    }
}

vector<pair<int, int>> getPruferEdges(const vector<int>& P, int n) {
    vector<pair<int, int>> edges;
    set<int> V;

    for (int i = 0; i < n; ++i) V.insert(i);

    int p_index = 0;
    int p_size = P.size();

    while (p_index < p_size) {
        int u = P[p_index];
        int v = -1;

        for (int x : V) {
            bool found = false;
            for (int i = p_index; i < p_size; ++i) {
                if (P[i] == x) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                v = x;
                break;
            }
        }

        edges.push_back({u, v});
        p_index++;
        V.erase(v);
    }

    auto it = V.begin();
    int v1 = *it;
    int v2 = *(++it);
    edges.push_back({v1, v2});

    return edges;
}

bool isTreeValid(const vector<pair<int, int>>& edges, const MatrixPC& mpc) {
    for (auto& edge : edges) {

        if (mpc.data[edge.first][edge.second] == 0) return false;
    }
    return true;
}

vector<ld> getWeightsForTree(const vector<pair<int, int>>& edges, const MatrixPC& mpc) {
    int n = mpc.n;
    vector<ld> weights(n, 1.0);
    vector<bool> visited(n, false);

    visited[0] = true;
    for (int step = 0; step < n; ++step) {
        for (auto& edge : edges) {
            int u = edge.first;
            int v = edge.second;
            if (visited[u] && !visited[v]) {
                weights[v] = weights[u] / mpc.data[u][v];
                visited[v] = true;
            } else if (visited[v] && !visited[u]) {
                weights[u] = weights[v] * mpc.data[u][v];
                visited[u] = true;
            }
        }
    }

    ld sum = 0;
    for (int i = 0; i < n; ++i) sum += weights[i];
    for (int i = 0; i < n; ++i) weights[i] /= sum;

    return weights;
}

vector<int> indexToPrufer(long long index, int n) {
    int len = n -2;
    vector<int> P(len);
    for (int i = len - 1; i >= 0; --i) {
        P[i] = index % n;
        index /= n;
    }
    return P;
}

vector<ld> calculatePriorityVector(const MatrixPC& mpc, bool useGeometric, int& validTreesCount) {
    int n = mpc.n;
    long long total_combinations = 1;
    for (int i = 0; i < n - 2; ++i) {
        total_combinations *= n;
    }


    ld accumulated_weights[15] = {0.0};
    validTreesCount = 0;

#pragma omp parallel for reduction(+:validTreesCount, accumulated_weights[:n])
    for (long long i = 0; i < total_combinations; ++i) {
        vector<int> P = indexToPrufer(i, n);
        auto edges = getPruferEdges(P, n);

        if (isTreeValid(edges, mpc)) {
            vector<ld> treeWeights = getWeightsForTree(edges, mpc);
            validTreesCount++;
            for (int j = 0; j < n; ++j) {
                if (useGeometric) {
                    accumulated_weights[j] += log(treeWeights[j]);
                } else {
                    accumulated_weights[j] += treeWeights[j];
                }
            }
        }
    }

    vector<ld> final_vector(n);
    if (validTreesCount > 0) {
        ld sum_for_normalization = 0;
        for (int j = 0; j < n; ++j) {
            if (useGeometric) {
                final_vector[j] = exp(accumulated_weights[j] / validTreesCount);
            } else {
                final_vector[j] = accumulated_weights[j] / validTreesCount;
            }
            sum_for_normalization += final_vector[j];
        }

        for (int j = 0; j < n; ++j) {
            final_vector[j] /= sum_for_normalization;
        }
    }
    return final_vector;
}

void runDiagnosticTest() {
    cout << "--- ЗАПУСК ДІАГНОСТИЧНОГО ТЕСТУ ---" << endl;
    int n = 3;
    MatrixPC testMatrix(n);
    // 1 2 4
    // 0.5 1 2
    // 0.25 0.5 1
    testMatrix.data[0][1] = 2.0;  testMatrix.data[1][0] = 1.0/2.0;
    testMatrix.data[0][2] = 4.0;  testMatrix.data[2][0] = 1.0/4.0;
    testMatrix.data[1][2] = 2.0;  testMatrix.data[2][1] = 1.0/2.0;

    int trees = 0;
    vector<ld> result = calculatePriorityVector(testMatrix, false, trees);

    cout << "Тестова матриця 3x3. Очікувана кількість дерев: 3^1 = 3" << endl;
    cout << "Знайдено дерев: " << trees << endl;
    cout << "Вектор пріоритетів: ";
    for(auto w : result) cout << fixed << setprecision(4) << w << " ";
    cout << "\n----------------------------------" << endl;
}

int main() {
    setlocale(LC_ALL, "Ukrainian");
    runDiagnosticTest();

    int n, meanChoice;
    cout << "\n--- ОСНОВНИЙ РОЗРАХУНОК ---" << endl;
    cout << "Введіть кількість об'єктів (n): ";
    cin >> n;

    MatrixPC mpc(n);
    fillMatrix(mpc);

    cout << "Оберіть метод усереднення:\n0 - Середнє арифметичне\n1 - Середнє геометричне\nВибір: ";
    cin >> meanChoice;

    bool useGeometric = (meanChoice == 1);
    int validTrees = 0;

    auto start = chrono::high_resolution_clock::now();

    vector<ld> weights = calculatePriorityVector(mpc, useGeometric, validTrees);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    if (validTrees > 0) {
        cout << "\nЗнайдено валідних дерев: " << validTrees << endl;
        cout << "Підсумковий вектор пріоритетів (" << (useGeometric ? "геометричне" : "арифметичне") << "):" << endl;

        for (int i = 0; i < n; ++i) {
            cout << "w[" << i + 1 << "] = " << fixed << setprecision(4) << weights[i] << endl;
        }
    } else {
        cout << "Валідних дерев не знайдено (граф матриці незв'язний)." << endl;
    }

    cout << "\nЧас виконання (парелельно): " << duration.count() << " сек." << endl;

    return 0;
}