//#pragma GCC optimize(2)

#include<bits/stdc++.h>
using namespace std;
const int N=1e5+10;
unsigned long long base=1e9+7;

int data_num; //>= 数量要大于这个1L
vector<int> data_tool[N];//数据
bitset<N> L_data[500];
unordered_map<int,int> L_data_tool;
int C_tool123[200000];
int n;   //数据的数量

class node{
    public:
    vector<int> a; //存储每个元素
    int num;     //在原文件有几个
    unsigned long long hx;
    
    void print(){
        for(int i=0;i<a.size();i++)cout<<a[i]<<" "; cout<<":"<<num;
        cout<<endl;
    }
    void prework(){
        hx=0;
        for(int i=0;i<a.size()-1;i++){
            hx=hx*base+a[i];
        }
    }
    unsigned long long get(){
        return hx*base+a[a.size()-1];
    }

};
vector<node> C[N];  // 未有num
vector<node> L[N];  // 处理num
bitset<N> L_index[2][500];
unordered_map<unsigned long long,int> L_all_map;

int create_C(int k){
    for(int i=0;i<L[k].size();i++){
        L[k][i].prework();
        L_all_map.insert({L[k][i].get(),i});
        for(int j=i+1;j<L[k].size();j++){
            int flag=1;
            for(int z=0;z<k-1;z++){
                if(L[k][i].a[z]!=L[k][j].a[z]){
                    flag=0;break;
                }
            }
            if(flag==1){
                vector<int> ab;
                for(int z=0;z<k-1;z++)ab.push_back(L[k][i].a[z]);
                ab.push_back(min(L[k][i].a[k-1],L[k][j].a[k-1]));
                ab.push_back(max(L[k][i].a[k-1],L[k][j].a[k-1]));
                C[k+1].push_back({ab,0});
            }
        }
    }

    return 0;
}

int check_data(node &p,int p_num,int k){
    
    int z=p.a.size()-1;
    p.prework();
    
    auto a=p.hx;
    
    auto b=p.a[z];
    auto it=L_all_map[a];
    auto is=L_data_tool[b];
    if(k==2)L_index[k%2][p_num]=L_data[it]&L_data[is];
    else L_index[k%2][p_num]=L_index[(k+1)%2][it]&L_data[is];
    return L_index[k%2][p_num].count();
}

int create_L(int k){ //由C[k] 生成L[k+1]  复杂度O(KN/32) k：频繁集数量 N：数据的数量
    int num=0;
    if(k==1){
        for(int i=0;i<C[k].size();i++){
            L_index[k%2][i]=L_data[C[k][i].a[0]];
            int a=L_index[k%2][i].count();
            C[k][i].num=a;
            if(a>=data_num){
                L[k].push_back(C[k][i]);
                num++;
            }
        }
    }else{
        for(int i=0;i<C[k].size();i++){
            int a=check_data(C[k][i],L[k].size(),k);
            C[k][i].num=a;
            if(a>=data_num){
                L[k].push_back(C[k][i]);
                num++;
            }
        }
    }
    return num;
}



inline int read()
{
    freopen("retail.dat","r",stdin);
    n=0;
    int num=0;
    while(1){
        int x=0;
        char ch=getchar();

        while(ch!=EOF&&ch<'0'||ch>'9')
        {
            if(ch=='\n')n++;
            ch=getchar();
        }
        if(ch==EOF)break;
        while(ch>='0' && ch<='9'){
             x=x*10+ch-'0',ch=getchar();
        }
        data_tool[n].push_back(x);
        C_tool123[x]++;
        num++;
        if(ch=='\n')n++;
        if(ch==EOF)break;
    }
    n++;
   cout<<n<<" "<<num<<endl;
   return 0;
} 

void solve(){

    //生成C1
   for(int i=0;i<N;i++){   
        if(C_tool123[i]<data_num)continue;
        node a;
        a.a.push_back(i);
        a.num=C_tool123[i];
         L_data_tool.insert({i,L[1].size()});
        L[1].push_back(a);
    }
    for(int i=0;i<n;i++){
        for(int j=0;j<data_tool[i].size();j++){
            auto it=L_data_tool.find(data_tool[i][j]);
            if(it!=L_data_tool.end()){
                L_data[it->second][i]=1;
            }            

        }
    }
    
   //输出
    int date_sum_num=0;
     cout<<"1L:"<<L[1].size()<<endl;
     date_sum_num+=L[1].size();
    for(int k=2;;k++){
        create_C(k-1);
        create_L(k);
        cout<<k<<"L:"<<L[k].size()<<endl;
         date_sum_num+=L[k].size();
        if(L[k].size()==0)break;
    }
    cout<<"总数量为："<< date_sum_num<<endl;


}

int main(){
    cout<<"输入百分比\n";
    double gb=1;
    //cin>>gb;
    
    clock_t start,end1,end2; //定义clock_t变量
    start = clock();   //开始时间
    read();
    data_num=ceil(gb*n/100);
    cout<<"最小数量"<<data_num<<endl;
    end1 = clock();   //结束时间
    cout<<"input time = "<<double(end1-start)/CLOCKS_PER_SEC<<"s"<<endl;


    solve();

    end2 = clock();   //结束时间
    cout<<"processing time = "<<double(end2-end1)/CLOCKS_PER_SEC<<"s"<<endl;
   
    cout<<"running time = "<<double(end2-start)/CLOCKS_PER_SEC<<"s"<<endl;
    return 0;
}
/*
4 1  //数据数量 最小的出现次数
3 1 3 4 //数量 商品序号
3 2 3 5
4 1 2 3 5
2 2 5
*/