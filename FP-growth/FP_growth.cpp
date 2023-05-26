#pragma GCC optimize(2)

#include <iostream>
#include <vector>
#include <map>
#include <cmath>
using namespace std;

class node
{
public:
    int num;  // 这个点有多少
    int fat;  // 这个点的父亲节点
    int data; // 这个点的数据
    map<int, int> edge;

    node() : num(0), fat(-1), data(-1) {}
    node(int data1, int data_num1, int p1) : data(data1), num(data_num1), fat(p1) {}
    void print(int null_num)
    {
        for (int i = 0; i < null_num; i++)
            cout << "     ";
        cout << data << "/" << num << "/" << fat;
        cout << endl;
    }
};

class Database
{
public:
    vector<pair<vector<int>, int>> data;

    multimap<int, int, greater<int>> &get_min_support_sort(int Support_Min)
    {
        map<int, int> min_tool;
        for (auto i = 0; i < data.size(); i++)
        {
            vector<int> &data_item = data[i].first;
            int date_num = data[i].second;
            for (int j = 0; j < data_item.size(); j++)
            {
                min_tool[data_item[j]] += date_num;
            }
        }

        multimap<int, int, greater<int>> *key_num = new multimap<int, int, greater<int>>;

        for (auto it = min_tool.begin(); it != min_tool.end(); it++)
        {
            if (it->second < Support_Min)
                continue;
            key_num->insert({it->second, it->first});
        }

        return *key_num;
    }

    void print()
    {
        cout << "总数量：" << data.size() << "\n";
        for (int i = 0; i < data.size(); i++)
        {
            vector<int> &a = data[i].first;
            for (int j = 0; j < a.size(); j++)
            {
                cout << a[j] << " ";
            }
            cout << "/" << data[i].second << "\n";
        }
    }
};

class Tree
{
public:
    map<int, vector<int>> data_index; // 索引
    vector<node> data;
    bool flag;
    Tree() // 建树
    {
        node *head = new node();
        data.push_back(*head);
        flag = true;
    }
    void insert(vector<int> &p, int p_num) // p经过离散化 P_num p的数量
    {
        // 遍历
        int index = 0;
        for (int i = 0; i < p.size(); i++)
        { // 如果有这个点
            auto it = data[index].edge.find(p[i]);
            if (it != data[index].edge.end())
            {
                // 变成这个点
                index = it->second;
                data[index].num += p_num;
            } // 没有这个点
            else
            {
                node *p1 = new node(p[i], p_num, index);
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
            }
        }
    }

    void insert(Database &all_Data) // 装入一个数据库
    {
        for (int i = 0; i < all_Data.data.size(); i++)
        {
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

    vector<vector<int>> line_out()
    {
        vector<vector<int>> tool;
        for (int i = 1; i < data.size(); i++)
        {
            for (int j = 0; j < tool.size(); j++)
            {
                vector<int> a = tool[j];
                a.push_back(data[i].data);
                tool.push_back(a);
            }
            tool.push_back({data[i].data});
        }
        return tool;
    }
    vector<int> line_out_num()
    {
        vector<int> tool(6);
        for (int i = 1; i < data.size(); i++)
        {

            for (int j = tool.size() - 1; j > 0; j--)
            {
                tool[j] += tool[j - 1];
            }
            tool[1]++;
        }
        return tool;
    }

    void print()
    {
        dfs(0, 0);
    }
    void dfs(int index, int num)
    {
        data[index].print(num);
        for (auto it = data[index].edge.begin(); it != data[index].edge.end(); it++)
        {
            dfs(it->second, num + 1);
        }
    }
};

Tree tree_L1;
Database database_all;     // 数据库
vector<vector<int>> L[10]; // 装所有的频繁项集
int Support_Min;           //>= 要满足的最小数量

void read() // 读入
{
    // 打开wad
    freopen("retail.dat", "r", stdin);
    vector<int> date_tool;
    while (1)
    {
        int x = 0;
        char ch = getchar();
        while (ch != EOF && ch < '0' || ch > '9')
        {
            if (ch == '\n')
            {
                database_all.data.push_back({date_tool, 1});
                date_tool.clear();
            }
            ch = getchar();
        }
        if (ch == EOF)
            break;
        while (ch >= '0' && ch <= '9')
        {
            x = (x << 3) + (x << 1) + ch - '0', ch = getchar();
        }
        date_tool.push_back(x);

        if (ch == '\n')
        {
            database_all.data.push_back({date_tool, 1});
            date_tool.clear();
        }
        else if (ch == EOF)
            break;
    }
    database_all.data.push_back({date_tool, 1});

    // 输出数量
    printf("\n数据数量:%d\n", database_all.data.size());
}

Database *data_base_build(Database &database, multimap<int, int, greater<int>> key_num)
{
    Database *database_new = new Database();

    for (int i = 0; i < database.data.size(); i++)
    {
        vector<int> &data_item = database.data[i].first;
        vector<int> tool;
        for (auto it = key_num.begin(); it != key_num.end(); it++)
        {
            for (int j = 0; j < data_item.size(); j++)
            {
                if (data_item[j] == it->second)
                {
                    tool.push_back(it->second);
                }
            }
        }
        if (tool.size() > 0)
            database_new->data.push_back({tool, database.data[i].second});
    }
    return database_new;
}

vector<int> tree_solve(Tree &tree)
{
    vector<int> freq_num(6);

    if (tree.flag)
    {
        return tree.line_out_num();
    }

    for (auto it = tree.data_index.begin(); it != tree.data_index.end(); it++)
    {
        vector<int> &data_item = it->second;
        Database *tool = new Database;
        for (int i = 0; i < data_item.size(); i++)
        {
            int a = data_item[i];
            vector<int> op = tree.get_line(a);
            if (op.size() > 0)
                tool->data.push_back({op, tree.data[a].num});
        }
        // cout << "数字:" << it->first << endl;
        // tool->print();

        multimap<int, int, greater<int>> &key_num = tool->get_min_support_sort(Support_Min);
        Database *database_new = data_base_build(*tool, key_num);
        Tree *tree_small = new Tree();
        tree_small->insert(*database_new);

        // cout << "清除:";
        // database_new->print();
        // cout << "树:";
        //  tree_small->print();

        vector<int> num = tree_solve(*tree_small);
        for (int i = 0; i < num.size() - 1; i++)
        {
            freq_num[i + 1] += num[i];
        }
        freq_num[1]++;
    }

    return freq_num;
}

void solve()
{

    multimap<int, int, greater<int>> &key_num = database_all.get_min_support_sort(Support_Min);
    auto it = data_base_build(database_all, key_num);
    // it->print();

    tree_L1.insert(*it);
    // tree_L1.print();
    vector<int> a = tree_solve(tree_L1);

    int sum = 0;
    for (int i = 1; i < a.size(); i++)
    {
        cout << i << "L:" << a[i] << "\n";
        sum += a[i];
    }
    cout << "sum:" << sum << endl;
}

int main()
{
    clock_t start, end; // 定义clock_t变量
    start = clock();    // 开始时间

    double percent = 1.5;
    cout << "输入百分比:";
    read();
    Support_Min = ceil(percent * database_all.data.size() / 100);
    cout << "最小数量" << Support_Min << endl;
    solve();

    end = clock(); // 结束时间
    cout << "running time = " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;
}
