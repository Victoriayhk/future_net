%read topo
dirstr = 'case1/';
topo = load([dirstr 'topo.csv']);
[Linknum,N] = size(topo);
Vnum = 1 + max(max(topo(:,2)),max(topo(:,2)));
%read LinkID and Cost
LinkID = -1 * ones(Vnum);
VCost = zeros(Vnum);
for i = 1:Linknum
    srcID = 1 + topo(i,2);
    dstID = 1 + topo(i,3);
    LinkID(srcID,dstID) = topo(i,1);
    VCost(srcID,dstID) = topo(i,4);
end
%read demand,s,t,VV
fid = fopen([dirstr 'demand.csv'],'r');
str = fscanf(fid,'%s');
str = strrep(str,',','|');
demand = sscanf(str,'%d|',[1 inf]);
[M,N] = size(demand);
VVnum = N - 1;
s = demand(1);
t = demand(2);
VV = [s,demand(1,3:VVnum+1)];
%set IsVV
IsVV = zeros(1,Vnum);
for i = 1:VVnum + 1
    IsVV(demand(i)+1) = 1;
end
%set VVCost
VVCost = zeros(VVnum);
for i=1:VVnum
    for j=2:VVnum
        VVCost(i,j) = VCost(VV(i)+1,VV(j)+1);
    end
end
for i=2:VVnum
    VVCost(i,1) = VCost(VV(i)+1,t+1);
end

%set biograph
IDS = {'0','1'};
% for i=0:Vnum-1
%     IDS = {IDS,i+'0'};
% end
bg = biograph(VCost,IDS);
set(bg.nodes,'shape','circle','color',[1,1,1]);
set(bg.nodes,'lineColor',[0,0,0],'lineWidth',2);
set(bg.nodes,'textColor',[0,0,0],'fontsize',9);
set(bg,'showWeights','on','arrowSize',8,'edgeFontSize',20);
set(bg,'layoutType','radial');
bg.EdgeType = 'curved';

dolayout(bg);
bg.nodes(1).shape = 'rectangle';
dolayout(bg,'pathsonly',true);
view(bg);