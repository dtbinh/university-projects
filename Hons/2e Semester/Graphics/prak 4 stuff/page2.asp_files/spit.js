

spitHere();
gITXTNIdx=0;
for (i=0;i<gK.length;i++)
gKL[i]=cpl(gK[i]);
function rKWL2(m) {
if (gITXTNIdx >= gITXTN.length) {
hIT(aAD,aKW,gSI);
var ndbET=new Date().getTime();
dbM('Hook Elapsed time: '+(ndbET-ndbST)+' ms.',1);
} else {
var tnow = (new Date()).getTime();
while((gITXTNIdx < gITXTN.length) && ((new Date()).getTime() - tnow < m)) {
aKW=fH(aKW,gITXTNIdx,gK,gITXTN[gITXTNIdx],mk);
gITXTNIdx++;
}
window.setTimeout('rKWL2(50);', 50);
}
}
rKWL2(250);

