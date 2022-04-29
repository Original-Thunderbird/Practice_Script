#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//函数结果状态代码
#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR -1

typedef int Status;     //是函数的类型，其值是函数结果状态代码
typedef int QElemType;  //对应顶点结点的下标
typedef struct QNode{
	QElemType data;     //节点信息
	struct QNode *next;
}QNode,*QueuePtr;   //队列节点结构
typedef struct{
	QueuePtr front,rear; //队头、队尾指针
}LinkQueue;     //队列结构

/**
 * 函数名称：InitQueue
 * 函数参数：队列结构指针Q
 * 函数功能：初始化一个空队列
 * 返回值：失败返回ERROR，成功返回OK
 **/
Status InitQueue(LinkQueue *Q)
{
    //构造一个空队列Q
	(*Q).front = (*Q).rear = (QueuePtr)malloc(sizeof(QNode));
	if(!(*Q).front) {
	    printf("分配空间失败");            return ERROR;
	}
	(*Q).front->next = NULL;
	return OK;
}

/**
 * 函数名称：QueueEmpty
 * 函数参数：队列结构Q
 * 函数功能：判断队列是否为空
 * 返回值：是返回TRUE，成功返回FALSE
 **/
Status QueueEmpty(LinkQueue Q)
{
    //若Q为空队列,则返回TRUE,否则返回FALSE
	if(Q.front == Q.rear) return TRUE;
	else return FALSE;
}

/**
 * 函数名称：EnQueue
 * 函数参数：队列结构指针Q，顶点下标e
 * 函数功能：将下标放入队列
 * 返回值：失败返回ERROR，成功返回OK
 **/
Status EnQueue(LinkQueue *Q, QElemType e)
{
    //插入元素e为Q的新的队尾元素
	QueuePtr p = (QueuePtr)malloc(sizeof(QNode));
	if(!p) //存储分配失败
		return ERROR;
	p->data = e;
	p->next = NULL;
	(*Q).rear->next = p;
	(*Q).rear = p;
	return OK;
}

/**
 * 函数名称：DeQueue
 * 函数参数：队列结构指针Q，顶点下标指针e
 * 函数功能：将下标从队列取出
 * 返回值：失败返回ERROR，成功返回OK
 **/
Status DeQueue(LinkQueue *Q, QElemType *e)
{
    //若队列不空,删除Q的队头元素,用e返回其值,并返回OK,否则返回ERROR
	QueuePtr p;
	if((*Q).front == (*Q).rear){
        printf("队列已为空");    return ERROR;
	}
	p = (*Q).front->next;
	*e = p->data;
	(*Q).front->next = p->next;
	if((*Q).rear == p)
		(*Q).rear = (*Q).front;
	free(p);
	return OK;
}

#define KIND 3  //图的默认种类：0-有向图, 1-有向网, 2-无向图, 3-无向网
#define COLUMN 5    //图的个数
#define MAX_VERTEX_NUM 20 //最大顶点数
#define MAX_VERNAME_LEN 10 //顶点名称最大长度
//数据元素类型定义
typedef int Boolean;
typedef int InfoType;   //网中弧长
typedef char VertexType;    //节点名称
typedef struct ArcNode{
	int adjvex;               //该弧所指向的顶点位置
	struct ArcNode * nextarc; //指向下一条弧的指针
	InfoType *info;           //该弧相关信息的指针
}ArcNode;                     //弧结点结构
typedef struct VNode{
	ArcNode * firstarc;             //指向第一条依附该顶点的弧的指针
	char data;                      //顶点信息
}VNode, AdjList[MAX_VERTEX_NUM];    //顶点结点结构
typedef struct{
	AdjList vertices;
	int vexnum, arcnum;       //图的当前顶点数和弧数
	int kind;                 //图的种类标志
}ALGraph;                     //图结构
Boolean judge[COLUMN], flag=FALSE;
Boolean visited[COLUMN][MAX_VERTEX_NUM];  //访问标志数组
int col=1;  //操作的表的序号，初始为1
FILE *fp;
char *filename, *pc, appe[5]=".dat", input[60]; //用于生成文件名
//Status (*VisitFunc)(VertexType a);  //函数变量

Status CreateGraph(ALGraph *G);  //按V和VR的定义构造图G
Status DestroyGraph(ALGraph *G);  //销毁图G
int LocateVex(ALGraph *G, VertexType u);  //若u在图G中存在，返回顶点u的位置序号，否则返回-1
char GetVex(ALGraph *G, int rec);
Status PutVex(ALGraph *G, int rec, VertexType v);
int FirstAdjVex(ALGraph *G, VertexType v);  //返回v的第一个邻接顶点，如果v没有邻接顶点，返回空
int NextAdjVex(ALGraph *G, VertexType v, VertexType w);  //返回v的（相对于w）下一个邻接顶点，如果w是最后一个邻接顶点，返回空。
Status InsertVex(ALGraph *G, VertexType v);  //在图G中增加新顶点v
Status DeleteVex(ALGraph *G, VertexType v);  //在图G中删除顶点v和与v相关的弧
Status InsertArc(ALGraph *G, VertexType v, VertexType w);  //在图G中增加弧<v,w>，如果图G是无向图，还需要增加<w,v>
Status DeleteArc(ALGraph *G, VertexType v, VertexType w);  //在图G中删除弧<v,w>，如果图G是无向图，还需要删除<w,v>
Status DFSTraverse(ALGraph *G);//对图G进行深度优先搜索遍历
Status BFSTraverse(ALGraph *G);  //对图G进行广度优先搜索遍历
int OutDegree(AdjList i);  //计算图G第i个定点的出度
Status SaveGraph(ALGraph List[]);  //保存
Status LoadGraph(ALGraph List[]);  //读取
Status ChooseList(int col, ALGraph **G, ALGraph List[]);

int main(){
    int op, rec;
    VertexType e,v,w;
    ALGraph *G=NULL, List[COLUMN];
    G=List;
    while(op){
        system("cls");       printf("\n\n");
        printf("此系统操作无向网 系统中共%d张图 当前操作第%d张\n", COLUMN, col);
        printf("      Menu for Linear Table On Node Structure \n");
        printf("-------------------------------------------------\n");
        printf("    	  1. CreateGraph      9. DeleteVex\n");
        printf("    	  2. DestroyGraph     10. InsertArc\n");
        printf("    	  3. LocateVex        11. DeleteArc\n");
        printf("    	  4. GetVex           12. DFSTraverse\n");
        printf("    	  5. PutVex           13. BFSTraverse\n");
        printf("    	  6. FirstAdjVex      14. SaveGraph\n");
        printf("    	  7. NextAdjVex       15. LoadGraph\n");
        printf("    	  8. InsertVex        16. ChooseGraph\n");
        printf("    	  0. Exit\n");
        printf("-------------------------------------------------\n");
        printf("    请选择你的操作[0~16]:");
        scanf("%d",&op);
        switch(op){
           	case 1:
             	if(CreateGraph(G)==OK) printf("图构建成功\n");
             	else printf("图构建失败\n");
             	getchar();  getchar();      break;
           	case 2:
             	if(DestroyGraph(G)==OK) printf("图销毁成功\n");
             	else printf("图销毁失败\n");
             	getchar();  getchar();      break;
           	case 3:
           		printf("请输入要查找的顶点e的名称(char型):");
             	fflush(stdin);
             	scanf("%c",&e);  getchar();
             	rec = LocateVex(G,e);
             	if(rec==-1) printf("不存在此顶点\n");
             	else printf("该顶点在图或网中的序号为%d\n",rec);
             	getchar();  getchar();      break;
            case 4:
                printf("请输入要查找的顶点rec的下标(int型):");
             	fflush(stdin);
             	scanf("%d",&rec);  getchar();
             	e = GetVex(G,rec);
             	if(e==ERROR)     printf("查找失败\n");
             	else    printf("%d的名称为%c\n", rec, e);
             	getchar();  getchar();      break;
            case 5:
                printf("请输入要修改的顶点rec的序号(int型):");
             	fflush(stdin);
             	scanf("%d",&rec);  getchar();
             	printf("输入新的名称v(char型):");
             	scanf("%c", &v);  getchar();
             	if(PutVex(G,rec,v)==OK)     printf("修改成功\n");
             	else    printf("修改失败\n");
             	getchar();  getchar();      break;
          	case 6:
          	    flag = FALSE;
             	printf("请输入要查找的顶点e的名称(char型):");
             	fflush(stdin);
             	scanf("%c",&e);  getchar();
             	rec = FirstAdjVex(G,e);
             	if(rec == ERROR){
                    if(flag==TRUE){
                        printf("v没有邻接顶点 ");     flag = FALSE;
                    }
                    printf("返回失败\n");
             	}
             	else
                    printf("v的第一个邻接顶点的序号为%d\n",rec);
             	getchar();  getchar();      break;
           	case 7:
           	    flag = FALSE;
             	printf("请输入要查找的顶点v和其相对的顶点w的名称(char型):");
             	fflush(stdin);
             	scanf("%c", &v);  getchar();
             	scanf("%c", &w);  getchar();
             	rec = NextAdjVex(G,v,w);
             	if(rec==ERROR){
                    if(flag==TRUE){
                        printf("w是v的最后一个邻接顶点 ");    flag = FALSE;
                    }
                    printf("返回失败\n");
             	}
             	else        printf("v相对于w的下一个邻接顶点的序号为%d\n",rec);
             	getchar();  getchar();      break;
           	case 8:
             	printf("请输入要添加的顶点v的名称(char型):");
             	fflush(stdin);
             	scanf("%c", &v);  getchar();
             	if(InsertVex(G,v)==OK) printf("添加成功\n");
             	else printf("添加失败\n");
             	getchar();  getchar();      break;
           	case 9:
            	printf("请输入要删除的顶点v的名称(char型):");
             	fflush(stdin);
             	scanf("%c", &v);  getchar();
             	if(DeleteVex(G,v)==ERROR) printf("不存在此顶点\n");
             	else printf("删除成功\n");
             	getchar();  getchar();      break;
           	case 10:
           		printf("请输入要添加弧的顶点v和顶点w的名称(char型):");
             	fflush(stdin);
             	scanf("%c", &v);  getchar();
             	scanf("%c", &w);  getchar();
             	if(InsertArc(G,v,w)==ERROR) printf("顶点v或w不存在");
             	else printf("添加成功\n");
             	getchar();  getchar();      break;
           	case 11:
           		printf("请输入要删除弧的顶点v和顶点w的名称(char型):");
             	fflush(stdin);
             	scanf("%c", &v);  getchar();
             	scanf("%c", &w);  getchar();
             	if(DeleteArc(G,v,w)==ERROR) printf("顶点v或w不存在");
             	else printf("弧删除成功");
             	getchar();  getchar();      break;
            case 12:
            	DFSTraverse(G);
            	getchar();  getchar();      break;
            case 13:
            	BFSTraverse(G);
            	getchar();  getchar();      break;
            case 14:
                if(SaveGraph(List)==OK)      printf("保存成功\n");
                else    printf("保存失败\n");
                getchar();  getchar();      break;
            case 15:
                if(LoadGraph(List)==OK)   printf("加载成功\n");
                else    printf("加载失败\n");
                getchar();  getchar();      break;
            case 16:
                printf("请输入要操作表的位序vol:(1<=col<=%d)", COLUMN);
                scanf("%d", &col);
                ChooseList(col, &G, List);
                getchar();  getchar();
           	case 0:
             	break;
        }//end of switch
    }//end of while
    return 0;
}

/**
 * 函数名称：CreateGraph
 * 函数参数：图结构指针G
 * 函数功能：构造一个图G
 * 返回值：成功构造返回OK，否则返回ERROR
 **/
Status CreateGraph(ALGraph *G){
	int i,j,k;
	int w; //权值W
	VertexType va, vb;//弧尾Va，弧头Vb
	ArcNode *p;
	/*
    printf("请输入图的3项基本信息\n类型(0-有向图, 1-有向网, 2-无向图, 3-无向网):\n");
	//输图类型
	scanf("%d",&(*G).kind);          getchar();
	while ((*G).kind < 0 || (*G).kind > 3)
	{
		printf("输入值非法 请重输:\n");
		scanf("%d",&(*G).kind);     getchar();
	}
	*/
	if(judge[col-1]==TRUE){
        printf("当前位置已有图 请先销毁再建立新图 ");    return ERROR;
	}
	G->kind= KIND;    //默认为无向网
	//输顶点数
	printf("顶点数（%d以内）:", MAX_VERTEX_NUM);
	scanf("%d", &(G->vexnum));   getchar();
	while (G->vexnum < 0 || G->vexnum > MAX_VERTEX_NUM)
	{
		printf("输入值非法 请重输:");
		scanf("%d",&(G->vexnum));   getchar();
	}
	if(G->vexnum)   judge[col-1] = TRUE;
	//输边数
	printf("弧(边)数:");
	scanf("%d", &(G->arcnum));      getchar();
	printf("请输入%d个顶点的名称或值(1个字符,空格分开):", (*G).vexnum);
	for (i = 0; i < G->vexnum; i++) //输入各顶点信息
	{
		scanf("%c",&(G->vertices[i].data));  getchar();
		G->vertices[i].firstarc = NULL;
	}
	if (G->kind % 2) //1、3为网
        printf("请按序输入弧(边)权值W、弧尾Va和弧头Vb(空格分开):\n");
	else //0、2为图
		printf("请按序输入弧(边)的弧尾Va和弧头Vb(空格分开):\n");
	for (k = 0; k < G->arcnum; k++) //构造邻接表
	{
		printf("%d:", k+1);
		if (G->kind % 2) //网
			scanf("%d %c %c", &w, &va, &vb);
		else //图
			scanf("%c %c", &va, &vb);
		i = LocateVex(G, va); //弧尾
		j = LocateVex(G, vb); //弧头
		if (i == -1 || j == -1)
		{//顶点不存在
			printf("顶点不存在 请重输\n");
			k--;		continue;
		}
		p = (ArcNode *)malloc(sizeof(ArcNode));
		p->adjvex = j;
		if ((*G).kind % 2)//网
		{
			p->info = (int *)malloc(sizeof(int));
			*(p->info) = w;
		}
		else  //图
			p->info = NULL;     //
		p->nextarc = (*G).vertices[i].firstarc; //插在表头，后进先出式
		G->vertices[i].firstarc = p;
		//无向图或无向网还需逆向的
		if(G->kind >= 2)
		{
			p = (ArcNode*)malloc(sizeof(ArcNode));
			p->adjvex = i;
			if ((*G).kind == 3) //无向网
			{
				p->info = (int*)malloc(sizeof(int));
				*(p->info) = w;
			}
			else //无向图
				p->info = NULL;
			p->nextarc = (*G).vertices[j].firstarc; //插在表头
			G->vertices[j].firstarc = p;
		}
	}
	return OK;
}

/**
 * 函数名称：DestroyGraph
 * 函数参数：图结构指针G
 * 函数功能：销毁图G
 * 返回值：成功销毁返回OK，否则返回ERROR
 **/
Status DestroyGraph(ALGraph *G){
    if(judge[col-1]!=TRUE){
        printf("图未初始化 ");   return ERROR;
    }
	int i;
	ArcNode *p, *q;
	for(i = 0; i < G->vexnum; i++)
	{
		p=G->vertices[i].firstarc;
		while(p)
		{
			q = p->nextarc;
			if((*G).kind % 2) //网
				free(p->info);
			free(p);
			p = q;
		}
	}
	G->vexnum = G->arcnum = 0;
	judge[col-1]=FALSE;
	return OK;
}

/**
 * 函数名称：LocateVex
 * 函数参数：图结构指针G，待查顶点名称u
 * 函数功能：查找名为u的顶点
 * 返回值：找到返回结点下标，否则返回ERROR（即-1）
 **/
int LocateVex(ALGraph *G, VertexType u){
    if(judge[col-1]!=TRUE){
        printf("图未初始化 ");   return ERROR;
    }
    if(G->vexnum == 0){
        printf("图为空 ");     return ERROR;
    }
	int i;
	for(i=0; i < G->vexnum; i++)
		if(u == G->vertices[i].data)
			return i;
	return -1;//未找到则返回-1
}

/**
 * 函数名称：GetVex
 * 函数参数：图结构指针G，待查顶点下标rec
 * 函数功能：获得下标为rec的顶点的名称
 * 返回值：找到返回结点名称，否则返回ERROR
 **/
VertexType GetVex(ALGraph *G, int rec){
    if(judge[col-1]!=TRUE){
        printf("图未初始化 ");   return ERROR;
    }
    if(G->vexnum == 0){
        printf("图为空 ");     return ERROR;
    }
    if(rec>= G->vexnum){
        printf("下标越界 ");    return ERROR;
    }
    return G->vertices[rec].data;
}

/**
 * 函数名称：PutVex
 * 函数参数：图结构指针G，要修改的顶点的下标rec，相应结点新名称v
 * 函数功能：给下标为rec的顶点赋名为u
 * 返回值：找到返回OK，否则返回ERROR
 **/
Status PutVex(ALGraph *G, int rec, VertexType v){
    if(judge[col-1]!=TRUE){
        printf("图未初始化 ");   return ERROR;
    }
    if(G->vexnum == 0){
        printf("图为空 ");     return ERROR;
    }
    if(rec>= G->vexnum){
        printf("下标越界 ");    return ERROR;
    }
    G->vertices[rec].data = v;
    return OK;
}

/**
 * 函数名称：FirstAdjVex
 * 函数参数：图结构指针G，待查顶点名称v
 * 函数功能：查找名为v的结点的第一邻接结点下标
 * 返回值：找到返回下标值，否则返回ERROR
 **/
int FirstAdjVex(ALGraph *G, VertexType v){
	ArcNode *p;
	int v1;
	v1 = LocateVex(G,v);//获取v在图G中的序号v1
    if(judge[col-1]!=TRUE){
        printf("图未初始化 ");   return ERROR;
    }
    if(G->vexnum == 0){
        printf("图为空 ");     return ERROR;
    }
	if (v1 == -1){
	    printf("不存在此顶点 ");		return ERROR;
	}
	p = G->vertices[v1].firstarc;
	if (p)		return p->adjvex;
	else
	{	    flag = TRUE;		return ERROR;	}
}

/**
 * 函数名称：NextAdjVex
 * 函数参数：图结构指针G，待查顶点名称v， v的邻接顶点名称w
 * 函数功能：查找名为v的(相对于名为w的顶点的)下一个邻接顶点
 * 返回值：v的(相对于w的)下一个邻接顶点的序号，
 *			若w是v的最后一个邻接点或查找失败,则返回ERROR
 **/
int NextAdjVex(ALGraph *G,VertexType v,VertexType w){
	ArcNode *p;
	int v1, w1;
    if(judge[col-1]!=TRUE){
        printf("图未初始化 ");   return ERROR;
    }
    if(G->vexnum == 0){
        printf("图为空 ");     return ERROR;
    }
	v1 = LocateVex(G,v); //获取v在图G中的序号v1
	w1 = LocateVex(G,w); //获取w在图G中的序号w1
	if (v1 == -1){//顶点不存在
	    printf("顶点v不存在 ");		return ERROR;
	}
	if (w1 == -1){//邻接点不存在
	    printf("顶点w不存在 ");		return ERROR;
	}
	p = G->vertices[v1].firstarc;
	while (p && p->adjvex != w1)//指针p不空且所指表结点不是w
		p = p->nextarc;
	//没找到w或w是最后一个邻接点
	if(!p){
        printf("w不是v的邻接顶点 ");  return ERROR;
	}
	else if(!p->nextarc){
        flag=TRUE;  return ERROR;       //flag用于区分是从主函数调用还是从其它函数调用，以决定是否输出相关提示信息
	}
	else    return p->nextarc->adjvex;
	//返回v的(相对于w的)下一个邻接顶点的序号
}

/**
 * 函数名称：InsertVex
 * 函数参数：图结构指针G，新顶点名称v
 * 函数功能：在图G中增加名为v的新顶点
 * 返回值：成功返回OK，否则返回ERROR
 **/
Status InsertVex(ALGraph *G, VertexType v){
    if(judge[col-1]!=TRUE){
        printf("图未初始化 ");   return ERROR;
    }
    if((*G).vexnum == 0){
        printf("图为空 ");     return ERROR;
    }
	if((*G).vexnum == MAX_VERTEX_NUM){//顶点数达最大值
        printf("顶点数达最大值 ");     return ERROR;
	}
	//构造新顶点
	(*G).vertices[(*G).vexnum].data = v;
	(*G).vertices[(*G).vexnum].firstarc=NULL;
	(*G).vexnum++; //图G的顶点数加1
	return OK;
 }

/**
 * 函数名称：DeleteVex
 * 函数参数：图结构指针G，待删顶点名称v
 * 函数功能：删除G中名为v的顶点及其相关弧
 * 返回值：成功返回OK，否则返回ERROR
 **/
Status DeleteVex(ALGraph *G, VertexType v){
	int i, j;
	ArcNode *p, *q;
    if(judge[col-1]!=TRUE){
        printf("图未初始化 ");   return ERROR;
    }
    if((*G).vexnum == 0){
        printf("图为空 ");     return ERROR;
    }
	j = LocateVex(G, v); //获取顶点v的序号
	if(j == -1)     return ERROR;//v不是图G的顶点{
	//删除以v为出度的弧或边
    p = (*G).vertices[j].firstarc;
	while(p)
	{
		q = p;
		p = p->nextarc;
		if((*G).kind % 2) //网
			free(q->info);
		free(q);
		(*G).arcnum--; //弧或边数减1
	}
	//重新排列顶点
	(*G).vexnum--; //顶点数减1
	for(i = j; i < (*G).vexnum; i++) //顶点v后面的顶点前移
		(*G).vertices[i] = (*G).vertices[i+1];
	//删除以v为入度的弧或边且修改弧所指顶点位置值
	for(i = 0; i < (*G).vexnum; i++)
	{
		p = (*G).vertices[i].firstarc; //指向第1条弧或边
		while(p) //有弧或边
		{
			if(p->adjvex == j)//该弧指向被删结点
			{
				if(p == (*G).vertices[i].firstarc) //待删结点是第1个结点
				{
					(*G).vertices[i].firstarc = p->nextarc;
					if((*G).kind%2) //网
						free(p->info);
					free(p);
					p = (*G).vertices[i].firstarc;
					if((*G).kind < 2)//有向图需再减一次
						(*G).arcnum--; //弧或边数减1
				}
				else
				{
					q->nextarc = p->nextarc;
					if((*G).kind % 2) //网
						free(p->info);
					free(p);
					p = q->nextarc;
					if((*G).kind < 2)//有向需再减一次
						(*G).arcnum--; //弧或边数减1
				}
			}
			else//该弧不指向被删结点
			{
				if(p->adjvex > j)//该弧所指元素位置序号在删除顶点之后
					p->adjvex--;    //更改弧所指顶点位置序号
				q = p;
				p = p->nextarc;//分析下一条弧
			}
		}
	}
	if(G->vexnum==0)        printf("图中已无顶点");
	return OK;
}

/**
 * 函数名称：InsertArc
 * 函数参数：图结构指针G，顶点名称v，w
 * 函数功能：在图G中增加弧<v,w>，如果图G是无向图/网，还需要增加<w,v>
 * 返回值：成功返回OK，否则返回ERROR
 **/
Status InsertArc(ALGraph *G,VertexType v,VertexType w){
	ArcNode *p;
	int w1, i, j;
    if(judge[col-1]!=TRUE){
        printf("图未初始化 ");   return ERROR;
    }
    if((*G).vexnum == 0){
        printf("图为空 ");     return ERROR;
    }
	i = LocateVex(G, v); //获取v位置序号
	j = LocateVex(G, w); //获取w位置序号
	if(i == -1 || j == -1)      return ERROR;//未找到相应顶点
	//建立弧v-w
	(*G).arcnum++;
	if((*G).kind % 2) //网要输权值
	{
		printf("请输入弧(边)%c->%c的权值:\n", v, w);
		scanf("%d", &w1);   getchar();
	}
	p=(ArcNode*)malloc(sizeof(ArcNode));
	p->adjvex = j;
	if((*G).kind % 2) //网
	{
		p->info = (int *)malloc(sizeof(int));
		*(p->info) = w1;
	}
	else		p->info=NULL;
	p->nextarc = (*G).vertices[i].firstarc;
	(*G).vertices[i].firstarc = p;
	//无向图需要再建立弧w->v
	if((*G).kind >= 2)
	{
		p = (ArcNode*)malloc(sizeof(ArcNode));
		p->adjvex = i;
		if((*G).kind == 3) //无向网
		{
			p->info = (int*)malloc(sizeof(int));
			*(p->info) = w1;
		}
		else			p->info = NULL;
		p->nextarc = (*G).vertices[j].firstarc;
		(*G).vertices[j].firstarc = p;
	}
	return OK;
}

/**
 * 函数名称：DeleteArc
 * 函数参数：图结构指针G，待删弧两顶点名称v，w
 * 函数功能：在图G中删除弧<v,w>，如果图G是无向图/网，还需要增加<w,v>
 * 返回值：成功返回OK，否则返回ERROR
 **/
Status DeleteArc(ALGraph *G,VertexType v,VertexType w){
	ArcNode *p, *q;
	int i, j;
	if(judge[col-1]!=TRUE){
        printf("图未初始化 ");   return ERROR;
    }
    if((*G).vexnum == 0){
        printf("图为空 ");     return ERROR;
    }
    if((*G).arcnum == 0){
        printf("图中已无弧 ");     return ERROR;
    }
	i = LocateVex(G, v); //顶点v序号
	j = LocateVex(G, w); //顶点w序号
	if(i < 0 || j < 0)		return ERROR;
	//删除弧<v,w>
	p = (*G).vertices[i].firstarc;
	while(p && p->adjvex != j) //p不空且所指之弧不是待删除弧<v,w>
	{ //p指向下一条弧
		q = p;		p = p->nextarc;
	}
	if(p && p->adjvex == j) //找到弧<v,w>
	{
		if(p == (*G).vertices[i].firstarc) //p所指是第1条弧
			(*G).vertices[i].firstarc = p->nextarc;     //指向下一条弧
		else	q->nextarc = p->nextarc;    //指向下一条弧
		if((*G).kind % 2)   free(p->info);  //网要多释放权值
		free(p); //释放此结点
		(*G).arcnum--; //弧或边数减1
	}
	//无向图要删除对称弧<w,v>
	if((*G).kind >= 2)
	{
		p = (*G).vertices[j].firstarc;
		while(p && p->adjvex != i) //p不空且所指之弧不是待删除弧<w,v>
		{ //p指向下一条弧 */
			q = p;			p = p->nextarc;
		}
		if(p && p->adjvex == i) //找到弧<w,v>
		{
			if(p == (*G).vertices[j].firstarc) //p所指是第1条弧
				(*G).vertices[j].firstarc=p->nextarc;   //指向下一条弧
			else		q->nextarc=p->nextarc;      //指向下一条弧
			if((*G).kind % 2)   free(p->info);      //网要多释放权值
			free(p); //释放此结点
		}
	}
	if((*G).arcnum == 0)     printf("图中已无弧 ");
	return OK;
 }

/**
 * 函数名称：DFS
 * 函数参数：图结构指针G，顶点下标v
 * 函数功能：从第v个顶点出发递归地深度优先遍历图G
 * 返回值：无
 **/
void DFS(ALGraph *G,int v){
	int w;
	VertexType v1, w1;
	v1 = G->vertices[v].data;
	visited[col-1][v] = TRUE; //设置访问标志为TRUE(已访问)
	printf("序号:%d-名称:%c  ", v, G->vertices[v].data); /* 访问第v个顶点 */
	for(w = FirstAdjVex(G,v1); w >= 0; w = NextAdjVex(G, v1, w1=G->vertices[w].data))
		if(!visited[col-1][w])
			DFS(G,w); //对v的尚未访问的邻接点w递归调用DFS
}

/*函数名称： DFSTraverse
*输入参数： 图G, 调用函数visit
*返回值：  Status 成功返回OK；
*函数功能： 对图G进行深度优先搜索遍历，依次对图中的每一个顶点使用函
*				数visit访问一次，且仅访问一次
*/

/**
 * 函数名称：DFSTraverse
 * 函数参数：图结构指针G
 * 函数功能：对图G进行深度优先搜索遍历，依次对图中的每一个顶点使用函
 *			  数visit访问一次，且仅访问一次
 * 返回值：成功返回OK，否则返回ERROR
 **/
Status DFSTraverse(ALGraph *G){
	int v;
	if(judge[col-1]!=TRUE){
        printf("图未初始化 ");   return ERROR;
    }
    if(G->vexnum == 0){
        printf("图为空 ");     return ERROR;
    }
//	VisitFunc = Visit; //使用全局变量VisitFunc,使DFS不必设函数指针参数
	for(v = 0; v < G->vexnum; v++)
		visited[col-1][v]=FALSE; //访问标志数组初始化
	for(v = 0; v < G->vexnum; v++)
		if(!visited[col-1][v])
            DFS(G,v); //对尚未访问的顶点调用DFS
	return OK;
}

/**
 * 函数名称：BFSTraverse
 * 函数参数：图结构指针G
 * 函数功能：按广度优先非递归遍历图G
 * 返回值：成功返回OK，否则返回ERROR
 **/
Status BFSTraverse(ALGraph *G){
	int v, u, w;
	VertexType u1, w1;
	LinkQueue Q;
	if(judge[col-1]!=TRUE){
        printf("图未初始化 ");   return ERROR;
    }
    if(G->vexnum == 0){
        printf("图为空 ");     return ERROR;
    }
	for(v = 0; v < G->vexnum; ++v)   visited[col-1][v]=FALSE;
    InitQueue(&Q); //置空的辅助队列Q
    for(v = 0; v < G->vexnum; ++v)
		if(!visited[col-1][v]) //v尚未访问
		{
			visited[col-1][v] = TRUE;
			printf("序号:%d-名称:%c  ", v, G->vertices[v].data);
			EnQueue(&Q, v); //v入队列
			while(!QueueEmpty(Q))
			{
				DeQueue(&Q, &u); //队头元素出队并置为u
				u1 = G->vertices[u].data;
				for(w = FirstAdjVex(G,u1); w >= 0; w = NextAdjVex(G,u1,w1=G->vertices[w].data))
					if(!visited[col-1][w]) //w为u的尚未访问的邻接顶点
					{
						visited[col-1][w] = TRUE;
						printf("序号:%d-名称:%c  ", w, G->vertices[w].data);
						EnQueue(&Q,w); //w入队
					}
			}
		}
	return OK;
}

/**
 * 函数名称：OutDegree
 * 函数参数：图G的第i个顶点地址
 * 函数功能：计算第i个顶点出度
 * 返回值：第i个顶点出度
 **/
int OutDegree(AdjList i){
	int num = 0;
	ArcNode * p;
	p = i->firstarc;
	while (p != NULL)
	{
		num++;
		p = p->nextarc;
	}
	return num;
}

/**
 * 函数名称：LoadGraph
 * 函数参数：图结构数组List
 * 函数功能：从文件中读取多个图的信息
 * 返回值：成功返回OK，否则返回ERROR
 **/
Status LoadGraph(ALGraph List[COLUMN]){
    /*
    if(judge!=TRUE){
        printf("图未初始化 ");   return ERROR;
    }
    */
	int i, j, k, num, yl;      //k为顶点下标, num为每个顶点的出度
	FILE *fp;
	ArcNode *p;
    printf("请输入您要恢复的数据所在的文件名:");
    scanf("%s",input);
    filename=input;
	if ((fp = fopen(filename,"r")) == NULL){
		printf("文件打开失败 ");		return ERROR;
	}
	fread(&yl,sizeof(int),1,fp);      //读取有效表个数
	//读取图基本信息
    for(i=0;i<yl;i++){
        fread(&(List[i].kind), sizeof(int), 1, fp);    //类别
        fread(&(List[i].vexnum), sizeof(int), 1, fp);  //点数
        fread(&(List[i].arcnum), sizeof(int), 1, fp);  //边数
        //读取邻接表
        for(j=0; j<List[i].vexnum; j++){  //有剩余顶点
            fread(&(List[i].vertices[j].data), sizeof(char), 1, fp);     //名称
            printf("图-%d,序号-%d,名称-%c",i,j,List[i].vertices[j].data);
            fread(&num, sizeof(int), 1, fp);                        //当前顶点出度
            if (num == 0)			List[i].vertices[j].firstarc = NULL;     //出度为0
            else
            {
                printf("邻接点序号:");
                List[i].vertices[j].firstarc = NULL;
                for (k = 0; k < num; k++)
                {
                    p = (ArcNode *)malloc(sizeof(ArcNode));
                    fread(&(p->adjvex), sizeof(int), 1, fp);    //邻接点编号
                    printf("%d ",p->adjvex);
                    if (List[i].kind % 2)//网
                    {
                        p->info = (int *)malloc(sizeof(int));
                        fread(p->info, sizeof(InfoType), 1, fp);    //
                    }
                    else    p->info = NULL; //图
                    p->nextarc = List[i].vertices[j].firstarc; //插在表头，后进先出型
                    List[i].vertices[j].firstarc = p;
                }
            }
            printf("\n\n");
        }
        judge[i] = TRUE;
    }
	fclose(fp);
	return OK;
}

/**
 * 函数名称：SaveGraph
 * 函数参数：图结构数组List
 * 函数功能：将多个有效图的信息存入文件
 * 返回值：成功返回OK，否则返回ERROR
 **/
Status SaveGraph(ALGraph List[COLUMN]){
	int i, h, num, yl=0;
	FILE  *fp;
	ArcNode *p;
    for(i=0;i<COLUMN;i++){      //统计有元素的表的个数
        if(judge[i]==TRUE && List[i].vexnum!=0)
            yl++;
    }
    if(yl ==0){       //无有效数据时报错返回
         printf("没有待存数据");         return ERROR;
    }
    /*
	printf("请输入保存文件名:");
    scanf("%s",filename);   getchar();
    */
    time_t lt;
    lt=time(NULL);
    filename=ctime(&lt);
    filename[strlen(filename)-1]='\0';
    strcat(filename,appe);      //以上生成时间命名的文件名字符串
    pc=&filename[0];
    while(*pc){       //文件名字符串规范化，将‘:’与' ' 用‘_’代替
        if((*pc==':' ) ||   (*pc==' ')){   *pc='_';    }
        pc++;
    }
    //写文件的方法
	if ((fp = fopen(filename, "w")) == NULL)
{
		printf("文件打开失败 ");		return ERROR;
	}
	fwrite(&yl,sizeof(int),1,fp);   //写入有效表的个数
	//保存图基本信息
	for(h=0; h<COLUMN; h++){
        if(judge[h]==TRUE && List[h].vexnum!=0){
            fwrite(&(List[h].kind), sizeof(int), 1, fp);    //类别
            fwrite(&(List[h].vexnum), sizeof(int), 1, fp);  //点数
            fwrite(&(List[h].arcnum), sizeof(int), 1, fp);  //边数
            //保存邻接表
            for (i = 0; i < List[h].vexnum; i++)
            {
                fwrite(&(List[h].vertices[i].data), sizeof(char), 1, fp);   //名称
                printf("表-%d,序号-%d,名称-%c ",h,i,List[h].vertices[i].data);
                if(List[h].vertices[i].firstarc==NULL)    printf("\n\n");
                num = OutDegree(&(List[h].vertices[i]));        //当前顶点出度
                fwrite(&num, sizeof(int), 1, fp);
                if ((p = List[h].vertices[i].firstarc) != NULL)
                {
                    fwrite(&(p->adjvex), sizeof(int), 1, fp);     //邻接顶点编号
                    printf("邻接点序号:%d ",p->adjvex);
                    if (List[h].kind % 2)//网
                        fwrite(p->info, sizeof(int), 1, fp);    //弧长
                    while ((p = p->nextarc) != NULL)
                    {
                        fwrite(&(p->adjvex), sizeof(int), 1, fp); //邻接顶点编号
                        printf("%d ",p->adjvex);
                        if (List[h].kind % 2)//网
                            fwrite(p->info, sizeof(int), 1, fp);//弧长
                    }
                    printf("\n\n");
                }
            }
        }
	}
	fclose(fp);
	return OK;
}

/**
 * 函数名称：ChooseList
 * 函数参数：要替换的表位置col，图结构指针G，图结构数组List
 * 函数功能：将G指向要操作的图下标col-1
 * 返回值：失败返回ERROR，成功返回OK
 **/
Status ChooseList(int col, ALGraph **G, ALGraph List[COLUMN]){
    if(col<1 || col>COLUMN){
        printf("下标越界 操作失败 ");
        return ERROR;
    }
    *G=&(List[col-1]);
    if(judge[col-1]!=TRUE)
        printf("此表尚未初始化 ");    //防止误操作
    return OK;
}
