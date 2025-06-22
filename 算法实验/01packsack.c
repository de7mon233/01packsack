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

// ������
void BruteForce() {
    clock_t start, end;
    double time_use;
    start = clock();

    double now_weight = 0;
    double now_value = 0;
    double max_value = 0;
    // ��ǰѡ��״̬
    int* state = malloc(sizeof(int) * number);
    int* best = malloc(sizeof(int) * number);

    for (int i = 0; i < (1 << number); i++) {
        now_weight = 0;
        now_value = 0;

        // ���ɵ�ǰѡ��״̬
        for (int j = 0; j < number; j++) {
            state[j] = (i >> j) & 1;
        }

        // ���㵱ǰѡ������������ܼ�ֵ
        for (int j = 0; j < number; j++) {
            if (state[j] == 1) {
                now_weight += weight[j];
                now_value += value[j];
            }
        }

        // �������Ž�
        if (now_weight <= capacity && now_value > max_value) {
            max_value = now_value;
            memcpy(best, state, sizeof(int) * number);  // �������Ž�
        }
    }
    // ���
    FILE* fp = fopen("bruteforce_result.csv", "w");
    if (fp == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "��Ʒ���,��Ʒ����,��Ʒ��ֵ\n");
    for (int i = 0; i < number; i++) {
        if (best[i] == 1) {
            fprintf(fp, "%d,%.2lf,%.2lf\n", i + 1, weight[i], value[i]);
        }
    }
    printf("����ֵΪ��%.2lf\n", max_value);
    fprintf(fp, "�ܼ�ֵ,%.2lf\n", max_value);
    fclose(fp);

    free(state);
    free(best);

    end = clock();
    time_use = ((double)(end - start)) * 1000 / CLOCKS_PER_SEC;
    printf("������ִ��ʱ��: %.2f ms\n", time_use);
}

// ��̬�滮
void DPfcn() {
    clock_t start, end;
    double time_use;
    start = clock();
    // ��̬�滮����dp[i][j]��ʾǰi����Ʒ������Ϊjʱ������ֵ
    double** dp = (double**)malloc((number + 1) * sizeof(double*));
    for (int i = 0; i <= number; i++) {
        dp[i] = (double*)malloc((capacity + 1) * sizeof(double));
        memset(dp[i], 0, (capacity + 1) * sizeof(double));
    }

    // ѡ��·����¼����choice[i][j]��¼����Ϊjʱ�Ƿ�ѡ���˵�i����Ʒ
    int** choice = (int**)malloc((number + 1) * sizeof(int*));
    for (int i = 0; i <= number; i++) {
        choice[i] = (int*)malloc((capacity + 1) * sizeof(int));
        memset(choice[i], 0, (capacity + 1) * sizeof(int));
    }

    // ��̬�滮����
    for (int i = 1; i <= number; i++) {
        for (int j = 0; j <= capacity; j++) {
            dp[i][j] = dp[i - 1][j]; // ��ѡ��i����Ʒ
            if (j >= weight[i - 1]) {
                double select_value = dp[i - 1][j - (int)weight[i - 1]] + value[i - 1];
                if (select_value > dp[i][j]) {
                    dp[i][j] = select_value;
                    choice[i][j] = 1; // ѡ�����Ʒ
                }
            }
        }
    }

    // �����ҳ�ѡ�����Ʒ
    double current_weight = capacity;
    int* selected = (int*)calloc(number, sizeof(int)); // ��¼ÿ����Ʒ�Ƿ�ѡ��

    for (int i = number; i >= 1; i--) {
        if (choice[i][(int)current_weight] == 1) {
            selected[i - 1] = 1; // ��i-1����Ʒ��ѡ��
            current_weight -= weight[i - 1]; // ��ȥ��ǰ��Ʒ������
        }
    }

    // ���
    FILE* fp = fopen("dp_result.csv", "w");
    if (fp == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "��Ʒ���,��Ʒ����,��Ʒ��ֵ\n");
    for (int i = 0; i < number; i++) {
        if (selected[i]) {
            fprintf(fp, "%d,%.2lf,%.2lf\n", i + 1, weight[i], value[i]);
        }
    }
    printf("����ֵΪ��%.2lf\n", dp[number][capacity]);
    fprintf(fp, "�ܼ�ֵ,%.2lf\n", dp[number][capacity]);
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
    printf("��̬�滮ִ��ʱ��: %.2f ms\n", time_use);
}

// ̰��
void greed() {
    clock_t start, end;
    double time_use;
    start = clock();
    // �����ֵ�ܶȲ��洢��Ʒ����
    double* density = malloc(sizeof(double) * number);
    int* indices = malloc(sizeof(int) * number);

    for (int i = 0; i < number; i++) {
        density[i] = value[i] / weight[i];
        indices[i] = i;
    }

    // ����ֵ�ܶȽ�������ͬʱ������Ʒ����
    for (int i = 0; i < number - 1; i++) {
        for (int j = 0; j < number - i - 1; j++) {
            if (density[j] < density[j + 1]) {
                // �����ܶ�
                double tempDensity = density[j];
                density[j] = density[j + 1];
                density[j + 1] = tempDensity;
                // ��������
                int tempIndex = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = tempIndex;
            }
        }
    }

    // ̰��ѡ��
    double current_weight = 0.0;
    double total_value = 0.0;
    int* selected = calloc(number, sizeof(int)); // ��¼ѡ�����Ʒ

    for (int i = 0; i < number; i++) {
        int idx = indices[i];
        if (current_weight + weight[idx] <= capacity) {
            selected[idx] = 1;
            current_weight += weight[idx];
            total_value += value[idx];
        }
    }

    // ���
    FILE* fp = fopen("greed_result.csv", "w");
    if (fp == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "��Ʒ���,��Ʒ����,��Ʒ��ֵ\n");
    for (int i = 0; i < number; i++) {
        if (selected[i]) {
            fprintf(fp, "%d,%.2lf,%.2lf\n", i + 1, weight[i], value[i]);
        }
    }
    printf("����ֵΪ��%.2lf\n", total_value);
    fprintf(fp, "�ܼ�ֵ,%.2lf\n", total_value);
    fclose(fp);

    free(density);
    free(indices);
    free(selected);

    end = clock();
    time_use = ((double)(end - start)) * 1000 / CLOCKS_PER_SEC;
    printf("̰�ķ�ִ��ʱ��: %.2f ms\n", time_use);
}

// ����
double max_value_bt = 0.0;
int* best_selected_bt;
// �����Ͻ�
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
// ���ݵݹ麯��
void backtrack(int i, double cw, double cv, int* selected) {
    // ����Ҷ�ڵ㣬�������Ž�
    if (i == number || cw >= capacity) {
        if (cv > max_value_bt) {
            max_value_bt = cv;
            memcpy(best_selected_bt, selected, sizeof(int) * number);
        }
        return;
    }

    // �����Ͻ�
    double upper_bound = bound(i, cw, cv);

    // ��֦������Ͻ�С�ڵ�ǰ����ֵ���򷵻�
    if (upper_bound <= max_value_bt) {
        return;
    }

    // ��ѡ��ǰ��Ʒ
    backtrack(i + 1, cw, cv, selected);

    // ѡ��ǰ��Ʒ
    if (cw + weight[i] <= capacity) {
        selected[i] = 1;
        backtrack(i + 1, cw + weight[i], cv + value[i], selected);
        selected[i] = 0; // ����
    }
}
// �����㷨������
void backtrace() {
    clock_t start, end;
    double time_use;
    start = clock();

    max_value_bt = 0.0;
    best_selected_bt = malloc(sizeof(int) * number);
    memset(best_selected_bt, 0, sizeof(int) * number);

    // ��ʼ����ǰѡ��״̬����
    int* selected = malloc(sizeof(int) * number);
    memset(selected, 0, sizeof(int) * number);

    // �����ֵ�ܶȲ�����
    double* density = malloc(sizeof(double) * number);
    int* indices = malloc(sizeof(int) * number);

    for (int i = 0; i < number; i++) {
        density[i] = value[i] / weight[i];
        indices[i] = i;
    }

    // ����ֵ�ܶȽ�������
    for (int i = 0; i < number - 1; i++) {
        for (int j = 0; j < number - i - 1; j++) {
            if (density[j] < density[j + 1]) {
                // �����ܶ�
                double tempDensity = density[j];
                density[j] = density[j + 1];
                density[j + 1] = tempDensity;
                // ��������
                int tempIndex = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = tempIndex;
            }
        }
    }

    // ����������Ʒ�����ͼ�ֵ
    double* sorted_weight = malloc(sizeof(double) * number);
    double* sorted_value = malloc(sizeof(double) * number);

    for (int i = 0; i < number; i++) {
        sorted_weight[i] = weight[indices[i]];
        sorted_value[i] = value[indices[i]];
    }

    // ����ԭʼָ�벢�滻Ϊ����������
    double* orig_weight = weight;
    double* orig_value = value;
    weight = sorted_weight;
    value = sorted_value;

    // ִ�л���
    backtrack(0, 0.0, 0.0, selected);

    // �ָ�ԭʼ����
    weight = orig_weight;
    value = orig_value;

    // ���
    FILE* fp = fopen("backtrace_result.csv", "w");
    if (fp == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "��Ʒ���,��Ʒ����,��Ʒ��ֵ\n");
    for (int i = 0; i < number; i++) {
        if (best_selected_bt[i]) {
            fprintf(fp, "%d,%.2lf,%.2lf\n", i + 1, weight[i], value[i]);
        }
    }
    printf("����ֵΪ��%.2lf\n", max_value_bt);
    fprintf(fp, "�ܼ�ֵ,%.2lf\n", max_value_bt);
    fclose(fp);

    free(best_selected_bt);
    free(selected);
    free(density);
    free(indices);
    free(sorted_weight);
    free(sorted_value);

    end = clock();
    time_use = ((double)(end - start)) * 1000 / CLOCKS_PER_SEC;
    printf("���ݷ�ִ��ʱ��: %.2f ms\n", time_use);
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