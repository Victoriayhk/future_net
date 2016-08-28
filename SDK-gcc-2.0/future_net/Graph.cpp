#include "future_net.h"


/*************************************************************************
*  函数名称：Graph::initial
*  功能说明：解析topo每一行的数据
*  参数说明：topo为输入字符指针数组，edge_num为边的条数
*  函数返回：无
*  修改时间：2016-4-26
*************************************************************************/
void Graph::initial(char *topo[MAX_EDGE_NUM], int edge_num)
{
	char *line;
	int ch,d,s,t,c,len;	//char,linkid,srcID,dstID,linkcost,length
    for(int i = 0,j;i < edge_num;i++)
    {
        //if(4 != sscanf(topo[i],"%d,%d,%d,%d",&d,&s,&t,&c))
		//	continue;
		//faster than sscanf
		line = topo[i];
		len = strlen(line);
		j = d = s = t = c = 0;
		//isdigit is in ctype.h
		while(!isdigit(ch = line[j++]) && j<len);
		if(isdigit(ch))	d = ch - '0';
		else	continue;
		while (isdigit(ch = line[j++]))	 d = 10 * d + (ch - '0');
		while(!isdigit(ch = line[j++])); s = ch - '0';
		while (isdigit(ch = line[j++]))	 s = 10 *  s + (ch - '0');
		while(!isdigit(ch = line[j++])); t = ch - '0';
		while (isdigit(ch = line[j++]))	 t = 10 *  t + (ch - '0');
		while(!isdigit(ch = line[j++])); c = ch - '0';
		while (isdigit(ch = line[j++]))	 c = 10 *  c + (ch - '0');

        //update cost、subCost、id、subId、Node、
		if(cost[s][t] == 0)
		{
            cost[s][t] = c;
            id[s][t] = d;
			Node[t].addPred(s);
        }
		else if(c < cost[s][t])
		{
			subCost[s][t] = cost[s][t];
			subId[s][t] = id[s][t];
			cost[s][t] = c;
			id[s][t] = d;
		}
		else if(subCost[s][t] == 0 || c < subCost[s][t])
		{
			subCost[s][t] = c;
			subId[s][t] = d;
		}
		//update num、MaxReachCost
		num = GetMax(num , GetMax(s,t));
    }
    num++;
	setCost();
}


/*************************************************************************
*  函数名称：Graph::setCost
*  功能说明：设置cost，subCost,如果没有值(系统初始化为0)就设置MaxReachCost
*  参数说明：无
*  函数返回：无
*  修改时间：2016-4-26
*************************************************************************/
void Graph::setCost()
{
    for(int i = 0; i < num; i++)
        for(int j = 0; j < num; j++)
		{
			if(cost[i][j] == 0)
			{
				cost[i][j] = subCost[i][j] = UnReachCost;
				id[i][j] = subId[i][j] = -1;
			}
			else if(subCost[i][j] == 0)
			{
				subCost[i][j] = UnReachCost;
				subId[i][j] = -1;
			}
		}
}
