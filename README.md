
![image](https://github.com/hannmuuc/Data_mining/blob/master/image/image1.png)

### Apriori算法
本算法实现了Apriori关联规则挖掘算法，并通过多线程来加速运行速度。<br>


### 运行过程
输入<br>
`freopen("retail.dat","r",stdin);//read()函数读入数据`<br>
处理<br>
 `thread_production_C1`<br>
....<br>
`   for(int k=2;;k++){                     `<br>
`       create_C(k-1);//创建C[K]           `<br>
`        create_L(k); //创建L[K]           `<br>
`       if(L[k].size()==0)break;           `<br>
 `   }                                      `<br>
输出<br>
 `int date_sum_num=0;//总数量                `<br>
 `   date_sum_num+=L[1].size();             `<br>
 `    printf("1L:%d\n",L[1].size());        `<br>
 `   for(int k=2;;k++){                     `<br>
 `       if(L[k].size()==0)break;           `<br>
 `       printf("%dL:%d\n",k,L[k].size());  `<br>
 `       date_sum_num+=L[k].size();         `<br>
 `   }                                      `<br>
 `   printf("总数量:%d\n",date_sum_num);     `<br>


### 注意事项
 代码使用了pthread实现多线程 需要支持pthread的环境下<br>
 测试数据集文件在retail.txt下<br>
 支持度阈值为向上取整<br>
