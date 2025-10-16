#include <bits/stdc++.h>
using namespace std;

// Результат: минимальная стоимость и сам маршрут (включая возврат в 0)
struct Result {
    int cost;
    vector<int> tour;
};

/*
  tsp_held_karp:
  - dist[i][j] — стоимость/расстояние перехода из i в j
  - Идея: dp[mask][u] = минимальная стоимость пути, который
    посещает ровно вершины из множества mask и заканчивается в u.
  - mask — битовая маска посещённых городов (бит k == 1, если город k уже в пути).
*/
Result tsp_held_karp(const vector<vector<int>>& dist) {
    const int n = (int)dist.size();
    const int FULL = (1 << n);   // общее число масок
    const int INF = 1e9;

    // dp[mask][u] — лучшая цена; parent — для восстановления пути
    vector<vector<int>> dp(FULL, vector<int>(n, INF));
    vector<vector<int>> parent(FULL, vector<int>(n, -1));

    // База: находимся в городе 0 и посетили только его
    dp[1 << 0][0] = 0;

    // Перебираем все подмножества (которые включают 0-й город)
    for (int mask = 1; mask < FULL; ++mask) {
        if (!(mask & 1)) continue; // стартовый город обязан быть в наборе
        // Перебираем "последний" город u, в котором заканчивается путь множества mask
        for (int u = 0; u < n; ++u) if (mask & (1 << u)) {
            if (dp[mask][u] >= INF) continue; // недостижим
            // Пробуем перейти в любой не посещённый город v
            for (int v = 0; v < n; ++v) if (!(mask & (1 << v))) {
                int nmask = mask | (1 << v);              // добавили v в набор
                int cand  = dp[mask][u] + dist[u][v];     // цена, если идём u -> v
                if (cand < dp[nmask][v]) {                // улучшение?
                    dp[nmask][v] = cand;                  // запомнили лучшую цену
                    parent[nmask][v] = u;                 // и из какого u пришли
                }
            }
        }
    }

    // Завершаем цикл: из любого конечного u возвращаемся в 0
    int all = FULL - 1;            // маска "все города посещены"
    int best = INF, last = -1;     // лучшая цена и последний город перед возвратом в 0
    for (int u = 1; u < n; ++u) {
        int cand = dp[all][u] + dist[u][0]; // замкнули цикл
        if (cand < best) { best = cand; last = u; }
    }

    // Восстановление оптимального пути по parent-указателям
    vector<int> order;             // последовательность без начального 0
    int mask = all, cur = last;
    while (cur != 0) {
        order.push_back(cur);
        int p = parent[mask][cur]; // откуда пришли в cur
        mask ^= (1 << cur);        // "вычеркнули" cur из множества
        cur = p;
    }
    reverse(order.begin(), order.end()); // теперь порядок посещения

    // Формируем полный тур: 0 -> ... -> 0
    vector<int> tour; tour.push_back(0);
    tour.insert(tour.end(), order.begin(), order.end());
    tour.push_back(0);

    return {best, tour};
}

int main() {
    // Ввод: n и затем матрица n×n (целые стоимости)
    int n;
    if (!(cin >> n)) return 0;
    vector<vector<int>> dist(n, vector<int>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            cin >> dist[i][j];

    // Запуск алгоритма и вывод ответа
    auto res = tsp_held_karp(dist);
    cout << "Cost: " << res.cost << "\nTour: ";
    for (int v : res.tour) cout << v << ' ';
    cout << "\n";
    return 0;
}
