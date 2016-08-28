#include "LKH.h"


/*************************************************************************
*  函数名称：read_file
*  功能说明：解析demand每一行的数据
*  参数说明：读取文件并按行输出到buff,buff为一个指针数组,每一个元素是一个字符指针，
   对应文件中一行的内容,spec为允许解析的最大行数
*  函数返回：0：打不开文件，1：读到的行数
*  修改时间：2016-4-14
*************************************************************************/
int read_file(char ** const buff, const unsigned int spec, const char * const filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Fail to open file %s, %s.\n", filename, strerror(errno));
        return 0;
    }

    char line[IN_MAX_LINE_LEN + 2];
    unsigned int cnt = 0;
    while ((cnt < spec) && !feof(fp))
    {
        line[0] = 0;
        fgets(line, IN_MAX_LINE_LEN + 2, fp);
        if (line[0] == 0)   continue;
        buff[cnt] = (char *)malloc(IN_MAX_LINE_LEN + 2);
        strncpy(buff[cnt], line, IN_MAX_LINE_LEN + 2 - 1);
        buff[cnt][IN_MAX_LINE_LEN + 2 - 1] = 0;
        cnt++;
    }
    fclose(fp);
    return cnt;
}


/*************************************************************************
*  函数名称：V_init
*  功能说明：解析topo每一行的数据
*  参数说明：filename为输入文件名字
*  函数返回：无
*  修改时间：2016-4-14
*************************************************************************/
void V_init(const char * const filename)
{
	int i,j;
	FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Fail to open file %s, %s.\n", filename, strerror(errno));
        return ;
    }
	int ch,id,s,t,c;//char,linkid,srcID,dstID,linkcost
    while (!feof(fp))
    {
        //if(4 != fscanf(fp,"%d,%d,%d,%d",&id,&s,&t,&c))
		//	continue;
		id = s = t = c = 0;
		if (!isdigit(ch = getc(fp)))	continue;
		id = ch - '0';
		while (',' != (ch = getc(fp)))	id = 10 * id + (ch - '0');
		while (',' != (ch = getc(fp)))	s = 10 * s + (ch - '0');
		while (',' != (ch = getc(fp)))	t = 10 * t + (ch - '0');
		while (isdigit(ch = getc(fp)))	c = 10 * c + (ch - '0');//isdigit is in ctype.h
	
        V.num = GetMax(V.num , GetMax(s,t));
		MaxReachECost = GetMax(MaxReachECost,c);
        if(V.cost[s][t] == 0)
		{
            V.cost[s][t] = c;
            V.id[s][t] = id;
			NODE[t].preNd[NODE[t].preCn++] = s;
        }
		else if(c < V.cost[s][t])
		{
			V.subCost[s][t] = V.cost[s][t];
			V.subId[s][t] = V.id[s][t];
			V.cost[s][t] = c;
			V.id[s][t] = id;
		}
		else if(V.subCost[s][t] == 0 || c < V.subCost[s][t])
		{
			V.subCost[s][t] = c;
			V.subId[s][t] = id;
		}
    }
	fclose(fp);

	//set V.num,SubMaxECost,MaxECost
    V.num++;
	#ifdef PRELIMINARY
		MaxECost = SubMaxECost = MaxReachECost*V.num + 1;
	#else
		SubMaxECost = MaxReachECost*V.num + 1;
		MaxECost 	= SubMaxECost*(V.num + 1);
	#endif	
	
	//initial V.cost
    for(i = 0; i < V.num; i++)
        for(j = 0; j < V.num; j++)
		{
			if(V.cost[i][j] == 0)
				V.cost[i][j] = V.subCost[i][j] = MaxECost;
			else if(V.subCost[i][j] == 0)
				V.subCost[i][j] = MaxECost;
		}
}


/*************************************************************************
*  函数名称：Road_init
*  功能说明：解析输入文件的两行数据，demand为要求通过V'的二维维数组
*  参数说明：filename是路径信息文件名
*  函数返回：无
*  修改时间：2016-4-14
*************************************************************************/
void Road_init(const char * const filename)
{	
	char *demand[RW_LINE_NUM];
    int demandNum,i,j,str_len,tmp;
    demandNum = read_file(demand, RW_LINE_NUM, filename);
    if (demandNum != RW_LINE_NUM)
    {
        printf("Please input valid demand file.\n");
        return ;
    }
	for(j = 0;j < RW_LINE_NUM;j++)
	{
		Rp = &Rd[j];
		int *const list = Rp->VVlist;
		//initial V.s,V.t,Road.VVlist
#ifdef PRELIMINARY
		if(RW_LINE_NUM+1 == sscanf(demand[j],"%d,%d,%d",&V.s,&V.t,list + (++Rp->VVnumInit)))
#else
		if(RW_LINE_NUM+1 == sscanf(demand[j],"%d,%d,%d,%d",&tmp,&V.s,&V.t,list + (++Rp->VVnumInit)))
#endif
		{	//V' only contains s&t two nodes
			list[0] = V.s;
			list[1] = V.t;
		}
		else
		{
			list[0] = V.s;
			Rp->VVnumInit++;
			//set Road.VVlist={s,V'}
			for(i = 0,str_len = strlen(demand[j]); i < str_len; i++)
				if(demand[j][i] == '|')
					if(1 != sscanf(demand[j]+i+1,"%d",list+(Rp->VVnumInit++)))
						Rp->VVnumInit--;
			list[Rp->VVnumInit] = V.t;
		}
		//initial Road.isVVinit
		//memset(Rp->isVVinit,False,sizeof(unsigned char)*(V.num));
		for(i = 0; i < Rp->VVnumInit; i++)
			Rp->isVVinit[list[i]] = True;
		Rp->isVVinit[V.t] = True;
	}
}


/*************************************************************************
*  函数名称：exam_result
*  功能说明：解析result.csv数据是否合理
*  参数说明：mode:"Simple","Complex"
*  函数返回：无
*  修改时间：2016-3-27
*************************************************************************/
#ifdef EXAM_RESULT
void exam_result(const char * const topofile,const char * const resultfile,int mode)
{
	char *result;
    int i,str_len;
    if (1 != read_file(&result, 1, resultfile))
    {
        printf("Please input valid result file.\n");
        return ;
    }
	//NA
	if(!strcmp(result,"NA"))
	{
		printf("NA\n");
		return;
	}
	//read links,linkt
	int links[MaxE]={0},linkt[MaxE]={0},linkc[MaxE]={0},linkID,srcID,dstID,linkCost,v1,v2;
	FILE *fp = fopen(topofile, "r");
    while (!feof(fp)) 
    {
        if(fscanf(fp,"%d,%d,%d,%d",&linkID,&srcID,&dstID,&linkCost) != 4)
			continue;
		links[linkID] = srcID;
		linkt[linkID] = dstID;
		linkc[linkID] = linkCost;
    }
	fclose(fp);
    //exam the start node
    int link,isUse[MaxV]={0},lastt,sumCost = 0,non_must[MaxV],nonNum=0;
	sscanf(result,"%d",&linkID);
	if(links[linkID]==0 && linkt[linkID]==0)//exam if link exist
	{
		printf("linkID %d does not exist\n",linkID);
        return ;
	}
	if(links[linkID] != V.s)//exam the start node
	{
		printf("Start node error : now is %d,should be %d\n", links[linkID], V.s);
        return ;
	}
	//exam the other node
	isUse[links[linkID]] = True;
	lastt = linkt[linkID];
	sumCost += linkc[linkID];
	Rp0->Vtour[Rp0->VtourCn] = links[linkID];
	Rp0->VtourCost[Rp0->VtourCn] = linkc[linkID];
	Rp0->VtourId[Rp0->VtourCn++] = linkID;
    for(i = 0,str_len = strlen(result); i < str_len; i++)
        if(result[i] == '|')
		{
            sscanf(result+i+1,"%d",&linkID);
			v1 = links[linkID];
			v2 = linkt[linkID];
			if(v1==0 && v2==0)//exam if link exist
			{
				printf("linkID %d does not exist\n",linkID);
				return ;
			}
			if(v1 != lastt)//exam if this start node is last end node
			{
				printf("Rode break : locate at link %d,last node is %d,should be %d\n",linkID,lastt,v1);
				return ;
			}
			if(isUse[v1] == True)//exam if this start node is used >=2 times
			{
				printf("Node %d is used >=2 times : locate at link %d,last node is %d\n",v1,linkID,lastt);
				return ;
			}
			//isUse[v1] = True;
			lastt = v2;
			sumCost += linkc[linkID];
			if(Rp0->isVVinit[v1] == False)
				non_must[nonNum++] = v1;
			isUse[v1] = True;
			Rp0->Vtour[Rp0->VtourCn] = v1;
			Rp0->VtourCost[Rp0->VtourCn] = linkc[linkID];
			Rp0->VtourId[Rp0->VtourCn++] = linkID;
		}
	//exam the last node
	if(lastt != V.t)
	{
		printf("End node error : now is %d,should be %d\n", lastt, V.t);
        return ;
	}
	isUse[lastt] = True;
	Rp0->Vtour[Rp0->VtourCn++] = lastt;
	
	//exam if all must-nodes is visited
	for(i = 0;i < Rp0->VVnumInit + 1;i++)
		if(isUse[Rp0->VVlist[i]] == False)
		{
			printf("Must-node %d is not visited\n",Rp0->VVlist[i]);
			return ;
		}
	//ouput cost(simple mode)
	if(mode == 0)
	{
		printf("%d\n",sumCost);
		return;
	}
	//ouput cost and non-must-nodes、other infomation(complex mode)
	printf("cost is %d\n",sumCost);
	if(nonNum > 0)
	{
		printf("non-must : ");
		for(i = 0;i < nonNum;i++)
			printf("%d,",non_must[i]);
		printf("\n");
	}
    printf("Nodes:(in each line,first and last is must,other is not.LinkId is in[],length is in())");
    for(i = 0; i < Rp0->VtourCn - 1; i++)
    {
		int v1 = Rp0->Vtour[i],v2 = Rp0->Vtour[i+1];
		if(Rp0->isVVinit[v1] == True)
			printf("\n%3d", v1);
		else
			printf("-->[%4d(%2d)]-->%3d",Rp0->VtourId[i-1],Rp0->VtourCost[i-1],v1);
		if(Rp0->isVVinit[v2] == True)
			printf("-->[%4d(%2d)]-->%3d",Rp0->VtourId[i],Rp0->VtourCost[i],v2);
    }
}
#endif
