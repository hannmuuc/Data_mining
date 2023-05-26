#pragma GCC optimize(2)
//#pragma comment(lib, "pthreadVC2.lib")

#include <iostream>
#include <bits/stdc++.h>
#include <pthread.h>
#include <semaphore.h>
using namespace std;
typedef pair<int, int> pii;
typedef pair<int, pii> piii;
const int NUM = 88162 + 1024;

int Data_Max = 0;      // �����е����ֵ
vector<int> Data[NUM]; // ����
int Data_Num;          // ���ݵ�����
bitset<NUM> data_bitset[10000];

int data_index[NUM];      // �ǲ���L1
vector<int> Data_L1[NUM]; // ����
int data_index_L2[NUM];   // L2���ݼ�
int data_count[NUM];      // ÿ������
int Support_Min;          //>= Ҫ�������С����

class node
{
public:
    // ����
    vector<int> itemsets;
    int support_num;
    const long long base = 1e9 + 7;
    // ��ӡ
    void print()
    {
        for (int i = 0; i < itemsets.size(); i++)
            cout << itemsets[i] << " ";
        cout << ":" << support_num;
        cout << endl;
    }
    // ��ϣ
    vector<unsigned long long> ba;
    vector<unsigned long long> inc;
    int ba_flag = 0;
    void prework()
    {
        if (ba_flag == 1)
            return;
        ba.push_back(1);
        inc.push_back(0);
        for (int i = 1; i <= itemsets.size(); i++)
        {
            ba.push_back(ba[ba.size() - 1] * base);
            inc.push_back(inc[inc.size() - 1] * base + itemsets[i - 1]);
        }
        ba_flag = 1;
    }
    inline unsigned long long get(long long l, long long r)
    {
        return inc[r] - inc[l - 1] * ba[r - l + 1];
    }
    inline unsigned long long remove(long long p)
    {
        return inc[p - 1] * ba[itemsets.size() - p] + get(p + 1, itemsets.size());
    }
};

vector<node> C[20];
vector<node> L[20];
bitset<NUM> L_bitset[2][10000];
unordered_map<unsigned long long, vector<int>> L_create_C_map;
unordered_map<unsigned long long, int> L_hash_map;

// �߳�
sem_t L_available;           // ���ڻ���L
pii thread_create_L1[20];    // ����1l
piii thread_L_available[20]; // creat_L

int create_C(int k)
{ // ��L[k] ���� C[k+1]  ���Ӷ�O(KM)  k��Ƶ�������� m������ƽ������
    // ��ʼ��
    L_create_C_map.clear();
    L_hash_map.clear();
    // ���� L[k]���� ����hash
    for (int i = 0; i < L[k].size(); i++)
    {
        L[k][i].prework();
        unsigned long long a = L[k][i].get(1, L[k][i].itemsets.size() - 1);
        unsigned long long L_hash = L[k][i].get(1, L[k][i].itemsets.size());
        auto it = L_create_C_map.find(a);
        L_hash_map.insert({L_hash, i});
        if (it != L_create_C_map.end())
            it->second.push_back(i);
        else
        {
            // ����ȥ�����һ���ַ���ȵ�hash
            vector<int> b = {i};
            L_create_C_map.insert({a, b});
        }
    }
    // ��L����C
    for (auto it = L_create_C_map.begin(); it != L_create_C_map.end(); it++)
    {
        // ÿ����ȵ�hash
        vector<int> &L_same_hash = it->second;
        vector<int> itemset_prior = L[k][L_same_hash[0]].itemsets;
        itemset_prior.pop_back();
        // ÿ����ȵ�hash �������
        for (int i = 0; i < L_same_hash.size(); i++)
        { // ����C
            for (int j = i + 1; j < L_same_hash.size(); j++)
            {
                // �Ⱥ�����
                vector<int> &first = L[k][L_same_hash[i]].itemsets;
                vector<int> &second = L[k][L_same_hash[j]].itemsets;
                // ����C�ڵ�
                node C_next;
                C_next.itemsets = itemset_prior;
                C_next.support_num = 0;
                C_next.itemsets.push_back(min(first[first.size() - 1], second[second.size() - 1]));
                C_next.itemsets.push_back(max(first[first.size() - 1], second[second.size() - 1]));
                C[k + 1].push_back(C_next);
                // ���C�ڵ��Ƿ����
                auto &it = C[k + 1][C[k + 1].size() - 1];
                it.prework();
                // ����ÿ�ֿ���
                for (int i = 0; i < it.itemsets.size() - 1; i++)
                {
                    auto a = it.remove(i + 1);
                    if (L_hash_map.find(a) == L_hash_map.end())
                    {
                        // ȡ��
                        C[k + 1].pop_back();
                        break;
                    }
                }
            }
        }
    }
    return 0;
}

int check_data(node &p, int k, int num)
{ // check_data->thread_work->create_L
    // �ҵ�size-1 �� item[size]��ֵ
    int itemset_size = p.itemsets.size() - 1;
    p.prework();

    auto a = p.get(1, itemset_size);
    auto b = p.itemsets[itemset_size];
    auto it = L_hash_map[a];
    auto is = data_index[b];

    // ��bitset������
    bitset<NUM> L_bitset_tool;
    if (k == 2)
        L_bitset_tool = data_bitset[it] & data_bitset[is];
    else
        L_bitset_tool = L_bitset[(k + 1) % 2][it] & data_bitset[is];
    // ����
    int to1 = L_bitset_tool.count();

    C[k][num].support_num = to1;
    // ������С֧�ֶ�
    if (to1 >= Support_Min)
    {
        // �������ȷ������
        sem_wait(&L_available);
        int p_num = L[k].size();
        L[k].push_back(C[k][num]);
        sem_post(&L_available);
        // �л�
        L_bitset[k % 2][p_num] = L_bitset_tool;
    }
    return to1;
}

void *thread_work(void *rank)
{ // check_data->thread_work->create_L
    // ����
    piii kl = *(piii *)rank;
    int k = kl.first, i = kl.second.first, thread_num = kl.second.second;
    while (1)
    {
        // ��������
        if (i >= C[k].size())
            return NULL;
        check_data(C[k][i], k, i);
        i += thread_num;
    }

    return NULL;
}

int create_L(int k)
{
    // ��C[k] ����L[k]  ���Ӷ�O(KN/32) k��Ƶ�������� N�����ݵ�����
    if (C[k].size() == 0)
        return 0;
    // �߳�����
    pthread_t thread[20];
    int thread_num = 8;
    if (C[k].size() < 100)
        thread_num = 1;

    // �߳�ִ��
    for (int i = 0; i < thread_num; i++)
    {
        thread_L_available[i] = {k, {i, thread_num}};
        pthread_create(&thread[i], NULL, thread_work, (void *)&thread_L_available[i]);
    }
    for (int i = 0; i < thread_num; i++)
        pthread_join(thread[i], NULL);

    return 0;
}

void *thread_production_C1(void *rank)
{ // C[1]�ĺ���
    // ����
    pii kl = *(pii *)rank;
    int bit_num = 64; // ÿ�δ���64λ
    int k = kl.first * bit_num, thread_num = kl.second * bit_num;
    while (1)
    {
        for (int i = 0; i < bit_num; i++)
        {
            int index = k + i;
            if (index >= Data_Num)
                break;
            // �ж�
            for (int j = 0; j < Data[index].size(); j++)
            {
                int it = data_index[Data[index][j]];
                if (it >= 0)
                {
                    Data_L1[index].push_back(Data[index][j]);
                    data_bitset[it][index] = 1;
                }
            }
        }
        k += thread_num;
        if (k >= Data_Num)
            break;
    }
    return NULL;
}

void *thread_production_L2(void *rank)
{
    pii kl = *(pii *)rank;
    int k = kl.first, thread_num = kl.second;

    while (1)
    {
        int p1 = L[2][k].itemsets[0];
        int p2 = L[2][k].itemsets[1];
        L_bitset[2 % 2][k] = data_bitset[data_index[p1]] & data_bitset[data_index[p2]];
        k += thread_num;
        if (k >= L[2].size())
            break;
    }

    return NULL;
}

int read() // ����
{
    // ��ʼ��
    sem_init(&L_available, 0, 1);
    // ��
    freopen("retail.dat", "r", stdin);
    Data_Num = 0;
    while (1)
    {
        int x = 0;
        char ch = getchar();

        while (ch != EOF && ch < '0' || ch > '9')
        {
            if (ch == '\n')
                Data_Num++;
            ch = getchar();
        }
        if (ch == EOF)
            break;
        while (ch >= '0' && ch <= '9')
        {
            x = (x << 3) + (x << 1) + ch - '0', ch = getchar();
        }
        Data[Data_Num].push_back(x);
        Data_Max = max(x, Data_Max);
        data_count[x]++;
        data_index[x] = -1;

        if (ch == '\n')
            Data_Num++;
        else if (ch == EOF)
            break;
    }
    Data_Num++;

    // �������
    printf("\n��������:%d  �������ֵ:%d\n", Data_Num, Data_Max);
    return 0;
}

void solve()
{ // ������
    // ����L1
    for (int i = 0; i <= Data_Max; i++)
    {
        if (data_count[i] < Support_Min)
            continue;
        node a;
        a.itemsets.push_back(i);
        a.support_num = data_count[i];
        data_index[i] = L[1].size();
        L[1].push_back(a);
    }

    // ����bitset
    pthread_t thread[10];
    int thread_num = 8;
    for (int i = 0; i < thread_num; i++)
    {
        thread_create_L1[i] = {i, thread_num};
        pthread_create(&thread[i], NULL, thread_production_C1, (void *)&thread_create_L1[i]);
    }
    for (int i = 0; i < thread_num; i++)
        pthread_join(thread[i], NULL);

    map<long long, long long> tool;
    for (int i = 0; i < Data_Num; i++)
    {
        for (int j = 0; j < Data_L1[i].size(); j++)
        {
            for (int k = j + 1; k < Data_L1[i].size(); k++)
            {
                long long a = Data_L1[i][j] * NUM + Data_L1[i][k];
                tool[a]++;
            }
        }
    }
    for (auto it = tool.begin(); it != tool.end(); it++)
    {
        if (it->second >= Support_Min)
        {
            vector<int> a;
            int p1 = it->first / NUM, p2 = it->first % NUM;
            a.push_back(it->first / NUM);
            a.push_back(it->first % NUM);
            node b;
            b.itemsets = a;
            b.support_num = it->second;
            L[2].push_back(b);
        }
    }

    thread_num = 2;
    for (int i = 0; i < thread_num; i++)
    {
        thread_create_L1[i] = {i, thread_num};
        pthread_create(&thread[i], NULL, thread_production_L2, (void *)&thread_create_L1[i]);
    }
    for (int i = 0; i < thread_num; i++)
        pthread_join(thread[i], NULL);

    // ����
    for (int k = 3;; k++)
    {
        create_C(k - 1);
        create_L(k);
        if (L[k].size() == 0)
            break;
    }

    // ���
    int date_sum_num = 0;
    date_sum_num += L[1].size();

    printf("1L:%d\n", L[1].size());
    printf("2L:%d\n", L[2].size());
    for (int k = 2;; k++)
    {
        if (L[k].size() == 0)
            break;
        printf("KL:%d\n", L[k].size());
        date_sum_num += L[k].size();
    }
    printf("������:%d\n", date_sum_num);
}

int main()
{
    double percent = 0.1;
    cout << "����ٷֱ�:";
    // cout<<percent<<"%\n";
    // cin>>percent;

    auto begin = std::chrono::high_resolution_clock::now();

    // ����
    read();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

    printf("Input Time measured: %.3f seconds.\n", elapsed.count() * 1e-9);

    Support_Min = ceil(percent * Data_Num / 100);
    cout << "��С����" << Support_Min << endl;

    // ����
    solve();

    auto end_solve = std::chrono::high_resolution_clock::now();
    auto elapsed_solve = std::chrono::duration_cast<std::chrono::nanoseconds>(end_solve - end);

    printf("Solve Time measured: %.3f seconds.\n", elapsed_solve.count() * 1e-9);
    printf("Process Time measured: %.3f seconds.\n", (elapsed_solve.count() + elapsed.count()) * 1e-9);

    return 0;
}
