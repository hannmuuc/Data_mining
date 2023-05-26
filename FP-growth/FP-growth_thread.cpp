#pragma GCC optimize(2)
//#pragma comment(lib, "pthreadVC2.lib")

#include <iostream>
#include <bits/stdc++.h>
#include <pthread.h>
#include <semaphore.h>
using namespace std;
typedef pair<int, int> pii;
typedef pair<int, pii> piii;
const int NUM = 1e5;
class node
{
public:
    int num;                  // 这个点有多少
    int fat;                  // 这个点的父亲节点
    int data;                 // 这个点的数据
    int deep;                 // 节点的深度
    int choose_num[12] = {0}; // 选择的时候的数量
    map<int, int> edge;       // 下一个节点的边

    node() : num(0), fat(-1), data(-1), deep(-1) {}
    node(int data1, int data_num1, int p1) : data(data1), num(data_num1), fat(p1), deep(-1) {}
    node(int data1, int data_num1, int p1, int deep1, int choose_num1, int thread_num) : data(data1), num(data_num1), fat(p1), deep(deep1) { choose_num[thread_num] = choose_num1; }
};

class Database
{
public:
    vector<pair<vector<int>, int>> data;
    Database() {}
    Database(int size)
    {
        vector<pair<vector<int>, int>> *da = new vector<pair<vector<int>, int>>(size);
        data = *da;
    }
};

vector<vector<long long>> frequent_jisuan(70);

class Tree
{
public:
    map<int, vector<int>> data_index; // 索引
    vector<int> data_index_tool;
    vector<node> data;
    bool flag;
    Tree() // 建树
    {
        node *head = new node();
        data.push_back(*head);
        flag = true;
        // data_index = *(new vector<vector<int>>(17000));
    }

    void check_data_index()
    {
        data_index_tool.clear();
        for (auto it = data_index.begin(); it != data_index.end(); it++)
        {
            data_index_tool.push_back(it->first);
        }
    }

    void insert(vector<int> &p, int p_num) // p经过离散化 P_num p的数量
    {
        // 遍历
        int index = 0;
        for (int i = 0; i < p.size(); i++)
        { // 如果有这个点
            auto it = data[index].edge.find(p[i]);
            if (it == data[index].edge.end())
            {
                node *p1 = new node(p[i], p_num, index, data[index].deep + 1, 0, 0);
                data[index].edge.insert({p[i], data.size()});
                if (data[index].edge.size() > 1)
                    flag = false;
                auto it = data_index.find(p[i]);
                if (it != data_index.end())
                    it->second.push_back(data.size());
                else
                {
                    vector<int> a;
                    a.push_back(data.size());
                    data_index.insert({p[i], a});
                }
                index = data.size();
                data.push_back(*p1);

            } // 没有这个点
            else
            {
                // 变成这个点
                index = it->second;
                data[index].num += p_num;
            }
        }
    }

    void insert(Database &all_Data) // 装入一个数据库
    {
        for (int i = 0; i < all_Data.data.size(); i++)
        {
            if (all_Data.data[i].first.size() > 0)
                insert(all_Data.data[i].first, all_Data.data[i].second);
        }
    }

    vector<int> get_line(int num)
    {
        vector<int> a;
        num = data[num].fat;
        while (num != 0)
        {
            a.push_back(data[num].data);
            num = data[num].fat;
        }

        return a;
    }

    multimap<int, int, greater<int>> &create_map(int Support_Min, int num, int thread_num)
    {
        multimap<int, int, greater<int>> *key = new multimap<int, int, greater<int>>;
        map<int, int> data_num;
        // vector<int> data_num(100);
        vector<int> &data_item_tool = data_index[num];
        if (data_item_tool.size() == 0)
            return *key;

        multimap<int, int, greater<int>> data_item;
        for (int i = 0; i < data_item_tool.size(); i++)
        {
            int son = data_item_tool[i];
            int fat = data[son].fat;
            if (fat == -1)
                continue;
            if (data[fat].choose_num[thread_num] == 0)
                data_item.insert({data[fat].deep, fat});
            data[fat].choose_num[thread_num] += data[son].num;
        }

        queue<int> q;
        int q_deep;
        auto it = data_item.begin();
        int son = it->second;

        q.push(son);
        q_deep = data[son].deep;

        for (it++; it != data_item.end(); it++)
        {
            son = it->second;

            while (q_deep > data[son].deep)
            {
                int q_size = q.size();
                for (int i = 0; i < q_size; i++)
                {
                    int son_tool = q.front();
                    int fat_tool = data[son_tool].fat;
                    q.pop();
                    if (son_tool == 0)
                        break;
                    data_num[data[son_tool].data] += data[son_tool].choose_num[thread_num];

                    if (data[fat_tool].choose_num[thread_num] == 0)
                        q.push(fat_tool);
                    data[fat_tool].choose_num[thread_num] += data[son_tool].choose_num[thread_num];
                }
                q_deep--;
            }

            q.push(son);
        }

        while (q_deep > -1)
        {
            int q_size = q.size();
            for (int i = 0; i < q_size; i++)
            {
                int son_tool = q.front();
                int fat_tool = data[son_tool].fat;
                q.pop();
                if (son_tool == 0)
                    break;
                data_num[data[son_tool].data] += data[son_tool].choose_num[thread_num];

                if (data[fat_tool].choose_num[thread_num] == 0)
                    q.push(fat_tool);
                data[fat_tool].choose_num[thread_num] += data[son_tool].choose_num[thread_num];
            }
            q_deep--;
        }
        // solve();

        for (auto it = data_num.begin(); it != data_num.end(); it++)
        {
            if (it->second < Support_Min)
                continue;
            key->insert({it->second, it->first});
        }

        return *key;
    }

    Tree &cut_node(multimap<int, int, greater<int>> *key, int thread_num)
    {
        // 建立树
        Tree *r = new Tree;
        map<int, int> key_num;
        for (auto it = key->begin(); it != key->end(); it++)
        {
            key_num.insert({it->second, it->first});
        }

        cut_node(*r, 0, {0}, key_num, thread_num);
        return *r;
    }
    void cut_node(Tree &r, int r_now, vector<int> prior, map<int, int> &key_num, int thread_num)
    {
        // 所有的边
        vector<pair<int, int>> insert_node;
        for (int i = 0; i < prior.size(); i++)
            dfs(prior[i], insert_node, 0, key_num, thread_num);

        // 判断这个点有没有插入过
        map<int, pair<int, vector<int>>> next_node;

        // 将所有节点放入next_node中
        for (int i = 0; i < insert_node.size(); i++)
        {
            int son = insert_node[i].first;
            int num = insert_node[i].second;
            auto it = next_node.find(data[son].data);

            if (it != next_node.end())
            { // 如果之前插入过
                r.data[it->second.first].num += num;
                it->second.second.push_back(son);
            }
            else
            { // 之前未插入过
                // 插入到Tree r中
                node *p = new node(data[son].data, num, r_now, r.data[r_now].deep + 1, 0, thread_num);
                int index = r.data.size();
                r.data[r_now].edge.insert({data[son].data, index});
                if (r.data[r_now].edge.size() > 1)
                    r.flag = false;

                auto it = r.data_index.find(data[son].data);
                if (it != r.data_index.end())
                    it->second.push_back(r.data.size());
                else
                {
                    vector<int> a;
                    a.push_back(r.data.size());
                    r.data_index.insert({data[son].data, a});
                }
                r.data.push_back(*p);
                // 插入已经插入的节点
                next_node.insert({data[son].data, {index, {son}}});
            }
        }

        // 递归
        for (auto it = next_node.begin(); it != next_node.end(); it++)
        {
            cut_node(r, it->second.first, it->second.second, key_num, thread_num);
        }
    }

    void dfs(int r, vector<pair<int, int>> &node, int deep, map<int, int> &key_num, int thread_num)
    {
        if (deep != 0)
        {
            auto is = key_num.find(data[r].data);
            if (is != key_num.end())
            {
                node.push_back({r, data[r].choose_num[thread_num]});
                data[r].choose_num[thread_num] = 0;
                return;
            }
        }
        data[r].choose_num[thread_num] = 0;
        for (auto it = data[r].edge.begin(); it != data[r].edge.end(); it++)
        {
            if (data[it->second].choose_num[thread_num] != 0)
                dfs(it->second, node, deep + 1, key_num, thread_num);
        }
    }

    vector<long long> &line_out_num()
    {
        return frequent_jisuan[data.size() - 1];
    }
};

Tree tree_L1;
Database database_all(NUM);             // 数据库
int Support_Min;                        //>= 要满足的最小数量
int Data_Max = 0;                       // 数据中的最大值
vector<int> Data[NUM];                  // 数据
int Data_Num;                           // 数据的数量
int data_index[NUM];                    // 是不是L1
int data_count[NUM];                    // 每个数量
int L1_NUM;                             // L1的数量
pii thread_create_L1[20];               // 多线程参数 生成database
pair<Tree *, pii> thread_tree_tool[20]; // 参数
vector<long long> thread_data[30];      // 多线程返回的数据

bool compare(pair<int, int> s1, pair<int, int> s2)
{
    return s1.second > s2.second;
}

int read() // 读入
{
    // 打开
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

    // 输出数量
    printf("\n数据数量:%d  数据最大值:%d\n", Data_Num, Data_Max);

    frequent_jisuan[0].push_back(0);

    for (int i = 1; i < 70; i++)
    {
        frequent_jisuan[i] = frequent_jisuan[i - 1];
        frequent_jisuan[i].push_back(0);
        for (int j = frequent_jisuan[i].size() - 1; j > 0; j--)
        {
            frequent_jisuan[i][j] += frequent_jisuan[i][j - 1];
        }
        frequent_jisuan[i][1]++;
    }

    return 0;
}

vector<long long> tree_solve(Tree &tree)
{
    vector<long long> freq_num(70);

    if (tree.flag)
    {
        return tree.line_out_num();
    }

    for (auto it = tree.data_index.begin(); it != tree.data_index.end(); it++)
    {

        if (it->second.size() == 0)
            continue;
        multimap<int, int, greater<int>> &map1 = tree.create_map(Support_Min, it->first, 4);
        Tree &tree_small = tree.cut_node(&map1, 4);

        vector<long long> num = tree_solve(tree_small);
        for (int i = 1; i < freq_num.size(); i++)
        {
            if (i > num.size())
                break;
            freq_num[i] += num[i - 1];
        }
        freq_num[1]++;
    }

    return freq_num;
}

void *thread_tree_solve_item(void *rank)
{
    vector<long long> freq_num(70);
    pii kl = *(pii *)rank;
    int k = kl.first, thread_num = kl.second;
    Tree &tree = tree_L1;
    if (k >= tree.data_index_tool.size())
    {
        thread_data[kl.first] = freq_num;
        return NULL;
    }

    while (1)
    {
        int num1 = tree.data_index_tool[k];
        multimap<int, int, greater<int>> &map1 = tree.create_map(Support_Min, num1, kl.first);
        Tree &tree_small = tree.cut_node(&map1, kl.first);

        vector<long long> num = tree_solve(tree_small);
        for (int i = 1; i < freq_num.size(); i++)
        {
            if (i > num.size())
                break;
            freq_num[i] += num[i - 1];
        }
        freq_num[1]++;

        k += thread_num;
        if (k >= tree.data_index_tool.size())
            break;
    }
    thread_data[kl.first] = freq_num;

    return NULL;
}

vector<long long> thread_tree_solve(Tree &tree)
{
    vector<long long> freq_num(70);
    tree.check_data_index();
    pthread_t thread[30];
    int thread_num = 12;
    for (int i = 0; i < thread_num; i++)
    {
        thread_create_L1[i] = {i, thread_num};
        pthread_create(&thread[i], NULL, thread_tree_solve_item, (void *)&thread_create_L1[i]);
    }
    for (int i = 0; i < thread_num; i++)
    {
        pthread_join(thread[i], NULL);
        for (int j = 0; j < thread_data[i].size(); j++)
        {
            freq_num[j] += thread_data[i][j];
        }
    }

    return freq_num;
}

void *thread_production_database(void *rank)
{
    pii kl = *(pii *)rank;
    int k = kl.first, thread_num = kl.second;
    map<int, int> opi;
    while (1)
    {
        opi.clear();
        vector<int> tool;
        for (int j = 0; j < Data[k].size(); j++)
        {
            if (data_index[Data[k][j]] != -1)
            {
                opi.insert({data_index[Data[k][j]], 0});
            }
        }
        for (auto it = opi.begin(); it != opi.end(); it++)
        {
            tool.push_back(it->first);
        }
        if (tool.size() != 0)
            database_all.data[k] = {tool, 1};

        k += thread_num;
        if (k >= Data_Num)
            break;
    }
    return NULL;
}

void solve()
{
    // clock_t start, end; // 定义clock_t变量
    // start = clock();    // 开始时间
    // 处理L1
    vector<pair<int, int>> L1_sort;
    for (int i = 0; i <= Data_Max; i++)
    {
        if (data_count[i] < Support_Min)
            continue;
        L1_sort.push_back({i, data_count[i]});
    }
    sort(L1_sort.begin(), L1_sort.end(), compare);

    for (int i = 0; i < L1_sort.size(); i++)
        data_index[L1_sort[i].first] = i + 1;
    L1_NUM = L1_sort.size();

    // 建立数据库

    pthread_t thread[30];
    int thread_num = 8;
    for (int i = 0; i < thread_num; i++)
    {
        thread_create_L1[i] = {i, thread_num};
        pthread_create(&thread[i], NULL, thread_production_database, (void *)&thread_create_L1[i]);
    }
    for (int i = 0; i < thread_num; i++)
        pthread_join(thread[i], NULL);

    // end = clock(); // 结束时间
    //   cout << "database time = " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;

    // 建树

    tree_L1.insert(database_all);

    // end = clock(); // 结束时间
    //  cout << "tree time = " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;

    vector<long long> opo = thread_tree_solve(tree_L1);

    int sum = 0;
    for (int i = 1; i < opo.size(); i++)
    {
        if (opo[i] == 0)
            break;
        // cout << i << "L:" << opo[i] << endl;
        sum += opo[i];
    }
    cout << "sum:" << sum << endl;

    // end = clock(); // 结束时间
    //  cout << "solve time = " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;
}

int main()
{
    clock_t start, end; // 定义clock_t变量
    start = clock();    // 开始时间
    double percent = 0.05;
    cout << "输入百分比:";
    read();
    Support_Min = ceil(percent * Data_Num / 100);
    cout << "最小数量" << Support_Min << endl;

    solve();

    end = clock(); // 结束时间
    cout << "running time = " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;
}
