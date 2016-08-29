#include "LKH.h"

static int len = 0;

//#define OUT_DEBUG_INFO
#ifdef OUT_DEBUG_INFO
	static char strResult[RW_LINE_NUM*30*OUT_MAX_LINE_LEN];
#else
	static char strResult[RW_LINE_NUM*OUT_MAX_LINE_LEN];
#endif

/*************************************************************************
*  函数名称：exam_repeated_node
*  功能说明：将信息写到result
*  参数说明：IsExistRoad：1，存在路；0，不存在路
*  函数返回：无
*  修改时间：2016-3-17
*************************************************************************/
void record_result(unsigned char IsExistRoad)
{
	if(!IsExistRoad)
	{
		len += sprintf(strResult, "NA");
		return ;
	}

	int i,j,v0,v1,v2;
	for(j = 0;j < RW_LINE_NUM;j++)
	{
		Rp = &Rd[j];
		for(i = 0; i < Rp->VtourCn - 1; i++)
			len += sprintf(strResult + len, "%d|", Rp->VtourId[i]);
		strResult[len - 1] = '\n';
#ifdef OUT_DEBUG_INFO
		len += sprintf(strResult + len, "Nodes:in each line,first and last is must,other is not.LinkId is in[],length is in()");
		for(i = 0; i < Rp->VtourCn - 1; i++)
		{
			v1 = Rp->Vtour[i], v2 = Rp->Vtour[i+1];
			if(Rp->isVV[v1] == True)
				len += sprintf(strResult + len,"\n%3d", v1);
			else
				len += sprintf(strResult + len,"-->[%5d(%2d)]-->%3d",Rp->VtourId[i-1],Rp->VtourCost[i-1],v1);
			if(Rp->isVV[v2] == True)
				len += sprintf(strResult + len,"-->[%5d(%2d)]-->%3d",Rp->VtourId[i],Rp->VtourCost[i],v2);
		}
		len += sprintf(strResult + len, "\nRoad%d : VVnumInit=%d,	VVnum=%d\n",j,Rp->VVnumInit,Rp->VVnum);
		len += sprintf(strResult + len, "Road%d : BestCost = %lld\n\n\n\n\n\n",j,Rp->cost);
		if(j==1)
		{
			len += sprintf(strResult + len, "repeated edge count = %5d\n", V.reIdCn);
			len += sprintf(strResult + len, "Two roads sum Cost  = %5lld\n", V.RCostSum);
			len += sprintf(strResult + len, "Used time			= %5ldms\n\n\n\n", clock()/1000);
		}
#endif
	}
	strResult[len - 1] = '\0';
}



/*************************************************************************
*  函数名称：write_result2file
*  功能说明：将result写到文件
*  参数说明：filename
*  函数返回：无
*  修改时间：2016-3-17
*************************************************************************/
void write_result2file(const char * const filename)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("Fail to open file %s, %s.\n", filename, strerror(errno));
        return;
    }
    fputs(strResult, fp);
	fclose(fp);
}


/*************************************************************************
*  函数名称：write_CostVV2file
*  功能说明：把VVcost write_to file
*  参数说明：filename
*  函数返回：无
*  修改时间：2016-3-10
*  待优化 ：不通过文件，直接使用CostVV
*************************************************************************/
void write_VVcost2file(const char * const filename)
{
    int i,j;
    FILE *fp = fopen(filename, "a+");
    if (fp == NULL)
    {
        printf("Fail to open file %s, %s.\n", filename, strerror(errno));
        return;
    }
    /*fprintf(fp,"NAME:  %s\n",filename);
    fprintf(fp,"TYPE: ATSP\n");
    fprintf(fp,"COMMENT: %d city problem\n",Rp0->VVnum);
    fprintf(fp,"DIMENSION:  %d\n",Rp0->VVnum);
    fprintf(fp,"EDGE_WEIGHT_TYPE: EXPLICIT\n");
    fprintf(fp,"EDGE_WEIGHT_FORMAT: FULL_MATRIX\n");
    fprintf(fp,"EDGE_WEIGHT_SECTION\n");*/
    for(i = 0; i < Rp0->VVnum; i++)
    {
		fprintf(fp,"%9d  ",Rp0->VVlist[i]);
        for(j = 0; j < Rp0->VVnum; j++)
            fprintf(fp,"%9d  ",Rp0->VVcost[i][j]);
        fprintf(fp,"\n");
    }
	//fprintf(fp,"%9d  ",Rp0->VVlist[Rp0->VVnum]);
	fprintf(fp,"Rp0->VVnum = %d\n",Rp0->VVnum);
	//fprintf(fp,"V.num = %d\n",V.num);
	//fprintf(fp,"Rp0->VVs=%d,Rp0->VVt=%d\n",Rp0->VVs,Rp0->VVt);
	//fprintf(fp,"V.costInit : \n");
	//for(i = 0; i < V.num; i++)
	//{
	//	for(j = 0; j < V.num; j++)
    //        fprintf(fp,"%9d  ",V.costInit[i][j]);
    //    fprintf(fp,"\n");
	//}
	//fprintf(fp,"V.isVV : \n",V.num);
	//for(i = 0; i < V.num; i++)
	//{
    //    fprintf(fp,"%9d  ",V.isVV[i]);
	//}
	fprintf(fp,"\n");
    //fprintf(fp,"EOF");
    fclose(fp);
}


/*************************************************************************
*  函数名称：write_Vpath2file
*  功能说明：write V.path to file
*  参数说明：filename
*  函数返回：无
*  修改时间：2016-3-10
*  待优化 ：不通过文件，直接使用CostVV
*************************************************************************/
//void write_Vpath2file(const char * const filename)
//{
//    int i,j;
//    FILE *fp = fopen(filename, "w+");
//    if (fp == NULL)
//    {
//        printf("Fail to open file %s, %s.\n", filename, strerror(errno));
//        return;
//    }
//    for(i = 0; i < V.num; i++)
//    {
//        for(j = 0; j < V.num; j++)
//            fprintf(fp,"%d		",V.path[i][j]);
//        fprintf(fp,"\n");
//    }
//    fclose(fp);
//}



/*************************************************************************
*  函数名称：write_result2file
*  功能说明：释放指针指向的缓冲区
*  参数说明：buff，二维指针；valid_item_num，行数
*  函数返回：无
*  修改时间：2016-3-17
*************************************************************************/
/*void release_buff(char ** const buff, const int valid_item_num)
{
 int i;
    for (i = 0; i < valid_item_num; i++)
        free(buff[i]);
}*/
