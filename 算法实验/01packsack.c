#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#define number 1000
#define capacity 10000
double* weight;
double* value;

void init() {
    weight = malloc(sizeof(double) * number);
    value = malloc(sizeof(double) * number);
    srand(time(NULL));
    for (int i = 0; i < number; i++) {
        weight[i] = (rand() % 9901 + 100) / 100.0;
        value[i] = (rand() % 90001 + 10000) / 100.0;
    }
}
void output() {
    for (int i = 0; i < number; i++) {
        printf("%.2lf", weight[i]);
        printf(" ");
    }
    printf("\n");
    for (int i = 0; i < number; i++) {
        printf("%.2lf", value[i]);
        printf(" ");
    }
}

// 暴力法
void BruteForce() {
    clock_t start, end;
    double time_use;
    start = clock();

    double now_weight = 0;
    double now_value = 0;
    double max_value = 0;
    // 当前选择状态
    int* state = malloc(sizeof(int) * number);
    int* best = malloc(sizeof(int) * number);

    for (int i = 0; i < (1 << number); i++) {
        now_weight = 0;
        now_value = 0;

        // 生成当前选择状态
        for (int j = 0; j < number; j++) {
            state[j] = (i >> j) & 1;
        }

        // 计算当前选择的总重量和总价值
        for (int j = 0; j < number; j++) {
            if (state[j] == 1) {
                now_weight += weight[j];
                now_value += value[j];
            }
        }

        // 更新最优解
        if (now_weight <= capacity && now_value > max_value) {
            max_value = now_value;
            memcpy(best, state, sizeof(int) * number);  // 更新最优解
        }
    }
    // 输出
    FILE* fp = fopen("bruteforce_result.csv", "w");
    if (fp == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "物品编号,物品重量,物品价值\n");
    for (int i = 0; i < number; i++) {
        if (best[i] == 1) {
            fprintf(fp, "%d,%.2lf,%.2lf\n", i + 1, weight[i], value[i]);
        }
    }
    printf("最大价值为：%.2lf\n", max_value);
    fprintf(fp, "总价值,%.2lf\n", max_value);
    fclose(fp);

    free(state);
    free(best);

    end = clock();
    time_use = ((double)(end - start)) * 1000 / CLOCKS_PER_SEC;
    printf("暴力法执行时间: %.2f ms\n", time_use);
}

// 动态规划
void DPfcn() {
    clock_t start, end;
    double time_use;
    start = clock();
    // 动态规划数组dp[i][j]表示前i个物品，容量为j时的最大价值
    double** dp = (double**)malloc((number + 1) * sizeof(double*));
    for (int i = 0; i <= number; i++) {
        dp[i] = (double*)malloc((capacity + 1) * sizeof(double));
        memset(dp[i], 0, (capacity + 1) * sizeof(double));
    }

    // 选择路径记录数组choice[i][j]记录容量为j时是否选择了第i个物品
    int** choice = (int**)malloc((number + 1) * sizeof(int*));
    for (int i = 0; i <= number; i++) {
        choice[i] = (int*)malloc((capacity + 1) * sizeof(int));
        memset(choice[i], 0, (capacity + 1) * sizeof(int));
    }

    // 动态规划过程
    for (int i = 1; i <= number; i++) {
        for (int j = 0; j <= capacity; j++) {
            dp[i][j] = dp[i - 1][j]; // 不选第i个物品
            if (j >= weight[i - 1]) {
                double select_value = dp[i - 1][j - (int)weight[i - 1]] + value[i - 1];
                if (select_value > dp[i][j]) {
                    dp[i][j] = select_value;
                    choice[i][j] = 1; // 选择该物品
                }
            }
        }
    }

    // 回溯找出选择的物品
    double current_weight = capacity;
    int* selected = (int*)calloc(number, sizeof(int)); // 记录每个物品是否被选择

    for (int i = number; i >= 1; i--) {
        if (choice[i][(int)current_weight] == 1) {
            selected[i - 1] = 1; // 第i-1个物品被选择
            current_weight -= weight[i - 1]; // 减去当前物品的重量
        }
    }

    // 输出
    FILE* fp = fopen("dp_result.csv", "w");
    if (fp == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "物品编号,物品重量,物品价值\n");
    for (int i = 0; i < number; i++) {
        if (selected[i]) {
            fprintf(fp, "%d,%.2lf,%.2lf\n", i + 1, weight[i], value[i]);
        }
    }
    printf("最大价值为：%.2lf\n", dp[number][capacity]);
    fprintf(fp, "总价值,%.2lf\n", dp[number][capacity]);
    fclose(fp);

    for (int i = 0; i <= number; i++) {
        free(dp[i]);
        free(choice[i]);
    }
    free(dp);
    free(choice);
    free(selected);

    end = clock();
    time_use = ((double)(end - start)) * 1000 / CLOCKS_PER_SEC;
    printf("动态规划执行时间: %.2f ms\n", time_use);
}

// 贪心
void greed() {
    clock_t start, end;
    double time_use;
    start = clock();
    // 计算价值密度并存储物品索引
    double* density = malloc(sizeof(double) * number);
    int* indices = malloc(sizeof(int) * number);

    for (int i = 0; i < number; i++) {
        density[i] = value[i] / weight[i];
        indices[i] = i;
    }

    // 按价值密度降序排序，同时交换物品索引
    for (int i = 0; i < number - 1; i++) {
        for (int j = 0; j < number - i - 1; j++) {
            if (density[j] < density[j + 1]) {
                // 交换密度
                double tempDensity = density[j];
                density[j] = density[j + 1];
                density[j + 1] = tempDensity;
                // 交换索引
                int tempIndex = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = tempIndex;
            }
        }
    }

    // 贪心选择
    double current_weight = 0.0;
    double total_value = 0.0;
    int* selected = calloc(number, sizeof(int)); // 记录选择的物品

    for (int i = 0; i < number; i++) {
        int idx = indices[i];
        if (current_weight + weight[idx] <= capacity) {
            selected[idx] = 1;
            current_weight += weight[idx];
            total_value += value[idx];
        }
    }

    // 输出
    FILE* fp = fopen("greed_result.csv", "w");
    if (fp == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "物品编号,物品重量,物品价值\n");
    for (int i = 0; i < number; i++) {
        if (selected[i]) {
            fprintf(fp, "%d,%.2lf,%.2lf\n", i + 1, weight[i], value[i]);
        }
    }
    printf("最大价值为：%.2lf\n", total_value);
    fprintf(fp, "总价值,%.2lf\n", total_value);
    fclose(fp);

    free(density);
    free(indices);
    free(selected);

    end = clock();
    time_use = ((double)(end - start)) * 1000 / CLOCKS_PER_SEC;
    printf("贪心法执行时间: %.2f ms\n", time_use);
}

// 回溯
double max_value_bt = 0.0;
int* best_selected_bt;
// 计算上界
double bound(int i, double cw, double cv) {
    double bound_val = cv;
    double remaining = capacity - cw;
    while (i < number && remaining > 0) {
        if (weight[i] <= remaining) {
            bound_val += value[i];
            remaining -= weight[i];
        }
        else {
            bound_val += value[i] * (remaining / weight[i]);
            remaining = 0;
        }
        i++;
    }
    return bound_val;
}
// 回溯递归函数
void backtrack(int i, double cw, double cv, int* selected) {
    // 到达叶节点，更新最优解
    if (i == number || cw >= capacity) {
        if (cv > max_value_bt) {
            max_value_bt = cv;
            memcpy(best_selected_bt, selected, sizeof(int) * number);
        }
        return;
    }

    // 计算上界
    double upper_bound = bound(i, cw, cv);

    // 剪枝：如果上界小于当前最大价值，则返回
    if (upper_bound <= max_value_bt) {
        return;
    }

    // 不选当前物品
    backtrack(i + 1, cw, cv, selected);

    // 选当前物品
    if (cw + weight[i] <= capacity) {
        selected[i] = 1;
        backtrack(i + 1, cw + weight[i], cv + value[i], selected);
        selected[i] = 0; // 回溯
    }
}
// 回溯算法主函数
void backtrace() {
    clock_t start, end;
    double time_use;
    start = clock();

    max_value_bt = 0.0;
    best_selected_bt = malloc(sizeof(int) * number);
    memset(best_selected_bt, 0, sizeof(int) * number);

    // 初始化当前选择状态数组
    int* selected = malloc(sizeof(int) * number);
    memset(selected, 0, sizeof(int) * number);

    // 计算价值密度并排序
    double* density = malloc(sizeof(double) * number);
    int* indices = malloc(sizeof(int) * number);

    for (int i = 0; i < number; i++) {
        density[i] = value[i] / weight[i];
        indices[i] = i;
    }

    // 按价值密度降序排序
    for (int i = 0; i < number - 1; i++) {
        for (int j = 0; j < number - i - 1; j++) {
            if (density[j] < density[j + 1]) {
                // 交换密度
                double tempDensity = density[j];
                density[j] = density[j + 1];
                density[j + 1] = tempDensity;
                // 交换索引
                int tempIndex = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = tempIndex;
            }
        }
    }

    // 重新排列物品重量和价值
    double* sorted_weight = malloc(sizeof(double) * number);
    double* sorted_value = malloc(sizeof(double) * number);

    for (int i = 0; i < number; i++) {
        sorted_weight[i] = weight[indices[i]];
        sorted_value[i] = value[indices[i]];
    }

    // 保存原始指针并替换为排序后的数组
    double* orig_weight = weight;
    double* orig_value = value;
    weight = sorted_weight;
    value = sorted_value;

    // 执行回溯
    backtrack(0, 0.0, 0.0, selected);

    // 恢复原始数组
    weight = orig_weight;
    value = orig_value;

    // 输出
    FILE* fp = fopen("backtrace_result.csv", "w");
    if (fp == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "物品编号,物品重量,物品价值\n");
    for (int i = 0; i < number; i++) {
        if (best_selected_bt[i]) {
            fprintf(fp, "%d,%.2lf,%.2lf\n", i + 1, weight[i], value[i]);
        }
    }
    printf("最大价值为：%.2lf\n", max_value_bt);
    fprintf(fp, "总价值,%.2lf\n", max_value_bt);
    fclose(fp);

    free(best_selected_bt);
    free(selected);
    free(density);
    free(indices);
    free(sorted_weight);
    free(sorted_value);

    end = clock();
    time_use = ((double)(end - start)) * 1000 / CLOCKS_PER_SEC;
    printf("回溯法执行时间: %.2f ms\n", time_use);
}

int main() {
    init();
    //output();
    //BruteForce();
    DPfcn();
    greed();
    backtrace();
    free(weight);
    free(value);
    return 0;
}