import java.io.*;
import java.util.*;

public class Main {

    // Обёртка для результата
    static class Result {
        int cost;
        List<Integer> tour;
        Result(int c, List<Integer> t) { cost = c; tour = t; }
    }

    /*
      Реализация Held–Karp:
      dp[mask][u] — минимальная стоимость, чтобы посетить "mask"
      и закончить в u. parent — для восстановления пути.
    */
    static Result tspHeldKarp(int[][] dist) {
        int n = dist.length;
        int FULL = 1 << n;
        int INF = 1_000_000_000;

        int[][] dp = new int[FULL][n];
        int[][] parent = new int[FULL][n];
        for (int[] row : dp) Arrays.fill(row, INF);
        for (int[] row : parent) Arrays.fill(row, -1);

        // База: в наборе только город 0, стоимость 0
        dp[1][0] = 0;

        // Перебор подмножеств и переходов
        for (int mask = 1; mask < FULL; mask++) {
            if ((mask & 1) == 0) continue; // без стартового города пропускаем
            for (int u = 0; u < n; u++) if ((mask & (1 << u)) != 0) {
                if (dp[mask][u] >= INF) continue;
                for (int v = 0; v < n; v++) if ((mask & (1 << v)) == 0) {
                    int nmask = mask | (1 << v);           // добавили v
                    int cand = dp[mask][u] + dist[u][v];   // переход u->v
                    if (cand < dp[nmask][v]) {             // улучшили?
                        dp[nmask][v] = cand;
                        parent[nmask][v] = u;               // запомнили предка
                    }
                }
            }
        }

        // Замыкаем цикл в 0 и выбираем лучшее окончание
        int all = FULL - 1, best = INF, last = -1;
        for (int u = 1; u < n; u++) {
            int cand = dp[all][u] + dist[u][0];
            if (cand < best) { best = cand; last = u; }
        }

        // Восстановление пути (обратным проходом по parent)
        LinkedList<Integer> order = new LinkedList<>();
        int mask = all, cur = last;
        while (cur != 0) {
            order.addFirst(cur);        // вставляем в начало, чтобы не реверсить
            int p = parent[mask][cur];  // предок
            mask ^= (1 << cur);         // убрали текущий из маски
            cur = p;
        }

        // Полный тур: 0 ... 0
        ArrayList<Integer> tour = new ArrayList<>();
        tour.add(0); tour.addAll(order); tour.add(0);

        return new Result(best, tour);
    }

    public static void main(String[] args) throws Exception {
        // Чтение входа: n и одна строка на каждую строку матрицы
        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        int n = Integer.parseInt(br.readLine().trim());
        int[][] dist = new int[n][n];
        for (int i = 0; i < n; i++) {
            String[] parts = br.readLine().trim().split("\\s+");
            for (int j = 0; j < n; j++) dist[i][j] = Integer.parseInt(parts[j]);
        }

        // Запуск и печать результата
        Result r = tspHeldKarp(dist);
        System.out.println("Cost: " + r.cost);
        System.out.println("Tour: " + r.tour);
    }
}
