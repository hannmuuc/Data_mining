#pragma GCC optimize(2)

#include<bits/stdc++.h>
using namespace std;
const int N=1e5+10;

int data_num; //>= 数量要大于这个1L
vector<int> data_tool[N];//数据
int C_tool123[200000];
int n;   //数据的数量

class node{
    public:
    vector<int> a; //存储每个元素
    int num;     //在原文件有几个
    void print(){
        for(int i=0;i<a.size();i++)cout<<a[i]<<" "; cout<<":"<<num;
        cout<<endl;
    }
};
vector<node> C[N];  // 未有num
vector<node> L[N];  // 处理num

int create_C(int k){
    for(int i=0;i<L[k].size();i++){
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

int check_data(vector<int> &p){

    int num=0;
    for(int i=0;i<n;i++){
        int k=0,flag=1;
      for(int j=0;j<p.size();){
        if(k==data_tool[i].size()){
            flag=0;
            break;
        }else if(p[j]==data_tool[i][k]){
            k++;j++;
        }else if(p[j]>data_tool[i][k]){
            k++;
        }else {
            flag=0;
            break;
        }
      }
      if(flag==1)num++;
    }
    return num;
}

int create_L(int k){
    int num=0;
    for(int i=0;i<C[k].size();i++){
        int a=check_data(C[k][i].a);
        C[k][i].num=a;
        if(a>=data_num){
            L[k].push_back(C[k][i]);
            num++;
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

int main(){
   clock_t start,end; //定义clock_t变量
    start = clock();   //开始时间

    cout<<"输入百分比\n";
    double gb=1.5;
    //cin>>gb;
    data_num=gb*88162/100;
    read();
   for(int i=0;i<N;i++){   //生成C1
        if(C_tool123[i]<data_num)continue;
        node a;
        a.a.push_back(i);
        a.num=C_tool123[i];
        L[1].push_back(a);
    }

    cout<<"1L:"<<L[1].size()<<endl;;

   // for(int i=0;i<L[1].size();i++)L[1][i].print();
    for(int k=2;;k++){
        create_C(k-1);
        //cout<<k<<"C:"<<;
       // for(int i=0;i<C[k].size();i++)C[k][i].print();
        int a=create_L(k);
        cout<<k<<"L:"<<L[k].size()<<endl;;
        //for(int i=0;i<L[k].size();i++)L[k][i].print();
        if(a==0)break;
    }
    end = clock();   //结束时间
    cout<<"running time = "<<double(end-start)/CLOCKS_PER_SEC<<"s"<<endl;
    return 0;
}
/*
4 1  //数据数量 最小的出现次数
3 1 3 4 //数量 商品序号
3 2 3 5
4 1 2 3 5
2 2 5
*/