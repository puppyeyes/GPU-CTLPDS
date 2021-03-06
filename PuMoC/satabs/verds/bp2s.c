#include "bp2s.h"
#include "bp2s.h"

/*******************************************************************/
#define bpi 	bp->right->left
#define bpt 	bp->right->right->left
/*******************************************************************/
/*******************************************************************/
#define spectrue()      treecompose("|",neg(treenum(1)),treenum(1))
#define specfalse()     treecompose("&",neg(treenum(1)),treenum(1))
/*******************************************************************/
model *bp2s(tree *bp,tree *vconstr,tree *bs,int booln,int k,int bv,int opt)
{
	model *mo,*mo2;

        mo=bp2s0(bp,bs,booln,k,bv,opt); 
	if (vconstr!=NULL) mo=bp2sADDconstr(mo,bp,vconstr,booln,k);
	return mo;
}
/*******************************************************************/
model *bp2s0(tree *bp,tree *bs,int booln,int k,int bv,int opt)
{
        int tp=TLFnnftype(bs);

        if (tp&2) { error("bp2s specificaiton problem"); }
        if ((tp&1)==0) return bp2sLTL(bp,bs,booln,k,bv,opt); 
	return bp2sCTL(bp,bs,booln,k,bv,opt); 
}
/*****************************************************************/
model *bp2sLTL(tree *bp,tree *bs,int booln,int k,int bv,int opt)
{
	model *mo,*mo2;
	tree *t0=TLFnegate(treecopy(bs));
	tree *t1;
	int npath=1;

	if (bv) t1=SPECltl2f1(t0,k,0,booln); 
		else t1=SPECltl2f0(t0,k,-1,booln,bpt);
        t1=treedopost(t1,TLFnnfsimp);
        if (t1->node[0]==TRUE) treereplace(t1,spectrue());
        if (t1->node[0]==FALSE) treereplace(t1,specfalse());

        mo=CNFsys2model(bpi,bpt,booln,k,npath);
        mo2=CNFspec2model(t1,mo->n); 
        mo=CNFconcatmodel(mo,mo2);

	treefree(t0); treefree(t1); return mo;
}
model *bp2sCTL(tree *bp,tree *bs,int booln,int k,int bv,int opt)
{
	model *mo,*mo2;
	tree *t0=treecopy(bs);
	tree *t1;
	int npath=SPECnpath(bs,bv,k,opt);


	switch (bv) {
	    case 0: t0=TLFnegate(t0);
		    if (opt&32) t1=SPECactl2f0CTLR(t0,k,booln);
       		    else t1=SPECactl2f0(t0,k,booln);
		    eqstateconstruct(t1,booln); treedopost(t1,SPECreplcolon);
		    break;
	    case 1: if (opt&16) t1=SPECactl2f1CTLR(t0,k,booln);
                    else t1=SPECactl2f1(t0,k,booln);
		    eqstateconstruct(t1,booln); treedopost(t1,SPECreplcolonneg);
		    t1=TLFnegate(t1);
		    break;
	}

        t1=treedopost(t1,TLFnnfsimp);
        if (t1->node[0]==TRUE) treereplace(t1,spectrue());
        if (t1->node[0]==FALSE) treereplace(t1,specfalse());

        mo=CNFsys2model(bpi,bpt,booln,k,npath);
        mo2=CNFspec2model(t1,mo->n); 
        mo=CNFconcatmodel(mo,mo2);

	treefree(t0); treefree(t1); return mo;
}

/*****************************************************************/
model *bp2sADDconstr(model *mo,tree *bp,tree *varconstr,int booln,int k)
{
	model *mo2;
	tree *ts1,*ts2;

 return mo; 
/* no add for testing zwh*/

        ts1=treecompose("F",NULL,TLFnegate(treecopy(varconstr)));
        ts2=bpLTLspec2s(bp,ts1,booln,k,1,0);

        mo2=CNFspec2model(ts2,mo->n);
        mo=CNFconcatmodel(mo,mo2);

        treefree(ts1); treefree(ts2); return mo;
}
/*****************************************************************/
tree *bpLTLspec2s(tree *bp,tree *bs,int booln,int k,int bv,int opt)
{
	tree *t0=TLFnegate(treecopy(bs));
	tree *t1;

	if (bv) t1=SPECltl2f1(t0,k,0,booln); 
		else t1=SPECltl2f0(t0,k,-1,booln,bpt);
        t1=treedopost(t1,TLFnnfsimp);
        if (t1->node[0]==TRUE) treereplace(t1,spectrue());
        if (t1->node[0]==FALSE) treereplace(t1,specfalse());

	treefree(t0); return t1;
}

/*******************************************************************/
model *CNFnonrepetition(tree *init,tree *trans,int bn,int k,int auxp)
{
	int *tmstring,*tm2string,*mm,*m1;
	int i,j,n,a,s,q,np;
	int aux1,aux0;
	model *mo;
	tree *tr;

	mo=(model *)malloc(sizeof(model)); 
	mo->mstring=(int *)malloc(sizeof(int)*MAXSTRING); 
	mo->n=mo->m=mo->l=0; 

	tmstring=(int *)malloc(sizeof(int)*MAXSTRING); *tmstring=0;
	tm2string=(int *)malloc(sizeof(int)*MAXSTRING); 

	tr=TLFnegate(treecopy(init)); tr=treedopost(tr,TLFnnf);
	aux1=auxp; auxp=CNFnnf2cnf(tr,0,0,auxp,tmstring); treefree(tr);
	aux0=auxp-aux1;
	for (n=0,i=2;i<=k+1;i++,n++) {
		CNFrotate(tmstring+1,tm2string+*tmstring*n,
			*tmstring,1,1,i,i,bn,n*aux0);
	}
	auxp=aux1+n*aux0;
	CNFaddstring(tm2string,*tmstring*n,mo);

	*tmstring=0; 
	tr=TLFnegate(treecopy(trans)); tr=treedopost(tr,TLFnnf); 
	treedoprevoid(tr,TLFlcnf);
	aux1=auxp; auxp=CNFnnf2cnf(tr,0,0,auxp,tmstring); treefree(tr);
	aux0=auxp-aux1;

	for (n=0,i=1;i<=k+1;i++) {
		for (j=i+2;j<=k+1;j++,n++) {
			CNFrotate(tmstring+1,tm2string+*tmstring*n,
				*tmstring,1,2, i,j,bn,n*aux0);
		}
	}
	auxp=aux1+n*aux0;
	CNFaddstring(tm2string,*tmstring*n,mo);

	free(tmstring); free(tm2string);
	mo->n=auxp; mo->mstring=(int *)realloc(mo->mstring,sizeof(int)*(mo->l));
	return mo;
}

/*******************************************************************/
model *CNFsys2model(tree *init,tree *trans,int bn,int k,int npath)
{
	int *tmstring,*mm,*m1;
	int i,j,n,a,s,q,np;
	int auxp;
	model *mo;
	tree *tr;

	mo=(model *)malloc(sizeof(model)); 
	mo->mstring=(int *)malloc(sizeof(int)*MAXSTRING); 
	mo->n=mo->m=mo->l=0; 

	tmstring=(int *)malloc(sizeof(int)*MAXSTRING); 
	auxp=bn*(k+1)*npath;

	tr=treecopy(init);
	*tmstring=0; auxp=CNFnnf2cnf(tr,0,0,auxp,tmstring); treefree(tr);
	CNFaddstring(tmstring+1,*tmstring,mo);

	if (k==0) {
		free(tmstring);
		mo->n=auxp;
		mo->mstring=(int *)realloc(mo->mstring,sizeof(int)*(mo->l));
		return mo;
	}

	tr=treecopy(trans);
	*tmstring=0; auxp=CNFnnf2cnf(tr,0,0,auxp,tmstring); treefree(tr);
	/* cnfwrite(tmstring+1,*tmstring,stdout); nl; */

	n=*tmstring*k*npath; /*tmstring=realloc(tmstring,sizeof(int)*n+1);*/
	mm=tmstring+1+*tmstring;

	q=auxp-bn*(k+1)*npath; a=2*bn; 
	for (np=0;np<npath;np++)
	for (i=0;i<k;i++) {
		if (np==0&&i==0) continue;
		for (j=1;j<=*tmstring;j++) 
		if (tmstring[j]==0) *mm++=0; else {
		    if (tmstring[j]>0) s=1; else s=-1; 
		    if (s*tmstring[j]<=a) *mm++=tmstring[j]+(i+(np)*(k+1))*bn*s;
		    else *mm++=tmstring[j]+(i+(np)*(k))*q*s; 
		}
	}
	auxp=auxp+q*(k-1)+q*(npath-1)*k;
	CNFaddstring(tmstring+1,n,mo);
/*
	tr=treecopy(spec);
	*tmstring=0; auxp=CNFnnf2cnf(tr,0,0,auxp,tmstring); treefree(tr);
	CNFaddstring(tmstring+1,*tmstring,mo);
*/
	free(tmstring);
	mo->n=auxp;
	mo->mstring=(int *)realloc(mo->mstring,sizeof(int)*(mo->l));

	return mo;
}
model *CNFspec2model(tree *spec,int auxp)
{
	int *tmstring;
	model *mo;
	tree *tr;

	mo=(model *)malloc(sizeof(model)); 
	mo->mstring=(int *)malloc(sizeof(int)*MAXSTRING); 
	mo->n=mo->m=mo->l=0; 

	tmstring=(int *)malloc(sizeof(int)*MAXSTRING); *tmstring=0; 
	tr=treecopy(spec); auxp=CNFnnf2cnf(tr,0,0,auxp,tmstring); 
	treefree(tr);
	CNFaddstring(tmstring+1,*tmstring,mo); free(tmstring);

	mo->n=auxp;
	mo->mstring=(int *)realloc(mo->mstring,sizeof(int)*(mo->l));
	return mo;
}
model *CNFconcatmodel(model *m1,model *m2)
{
	int i;
	int *p1,*p2;
	int *mstring;

	m1->mstring=(int *)realloc(m1->mstring,(m1->l+m2->l)*sizeof(int)); 
	m1->n=(m2->n>m1->n?m2->n:m1->n); m1->m+=m2->m; 

	p1=m1->mstring+m1->l; 
	for (p2=m2->mstring,i=0;i<m2->l;i++) { *p1++=*p2++; } m1->l+=m2->l; 
	free(m2->mstring); free(m2);

	return m1;
}
/*******************************************************************/
void CNFaddstring(int *mstring,int mstringc,model *mo)
{
        int i;
        int *mm=mo->mstring+mo->l;

        if ((mo->l+=mstringc)>MAXSTRING) error("MAXSTRING");
        for (i=0;i<mstringc;i++) { *mm++=mstring[i]; if (!mstring[i]) mo->m++; }
}
/*******************************************************************/

/*******************************************************************/
#define ptrans  ws2->prog->right->right->left
/*******************************************************************/
tree *crspec(workspace *ws2,int pp)
{
	tree *spec=NULL;

	spec=listget1n(ws2->prog->right->right->right,pp);
	spec=treecopy(spec);
        if (pp==-1) spec=treeconst(1);
        if (pp==-2) spec=agTRUE();
        if (pp==-3) spec=agagTRUE();
        if (pp==-4) spec=afagTRUE();
        if (pp==-5) spec=afFALSE();
	if (spec==NULL) spec=agCompose(treecopy(ws2->deadlockfreecond));

	return spec;
}
/*******************************************************************/
tree *agTRUE()
{
        tree *t=treeconst(1); /*treecompose("|",neg(treenum(1)),treenum(1));*/
        return treecompose("A",NULL,treecompose("G",NULL,t));
}
tree *agagTRUE()
{
        tree *t=agTRUE();
        return treecompose("A",NULL,treecompose("G",NULL,t));
}
tree *afagTRUE()
{
        tree *t=agTRUE();
        return treecompose("A",NULL,treecompose("F",NULL,t));
}
tree *afFALSE()
{
        return treecompose("A",NULL,treecompose("F",NULL,treeconst(0)));
}
tree *agCompose(tree *prop)
{
        tree *t=treecompose("G",NULL,prop);
        return treecompose("A",NULL,t);
}

/*******************************************************************/
tree *SPECactl2f0(tree *t,int k,int pn)
{
	tree *t0;

	t0=actl2f0a(t,k,pn,0,0,0); 
/*
	t0=actlreplaceeq(t0,vars,pn);
        t0=treetraverse(t0,NULL,NULL,cleanbool);
*/
	return t0;
}
tree *SPECactl2f1(tree *t,int k,int pn)
{
	tree *t0;

	t0=actl2f1a(t,k,pn,0,0,0); 
/*
	t0=actlreplaceeq(t0,vars,pn);
	return negate(t0);
*/
	return t0;
}
/*******************************************************************/
tree *eqpath(int pa,int k)
{
        int i,j;
        tree *t0;

        if (k==0) return treeconst(0);
        t0=eqstate(pa,0,pa,1,k);

        for (i=0;i<k;i++)
        for (j=i+1;j<=k;j++) {
                if (j==1) continue;
                t0=treecompose("|",t0,eqstate(pa,i,pa,j,k));
        }
        return t0;
}
/*******************************************************************/
tree *makeform_ctl(tree *f(),tree *t,int k,int pn,int pa,int x,int y,int j)
{
	int i;
	tree *t0;

	if (k<j) return treeconst(0);
	t0=f(t,k,pn,pa,x,y);
	for (i=1;i<=j;i++) t0=treecompose("&",f(t,k,pn,pa,x,y+i),t0);
	return t0;
}
/*******************************************************************/
tree *actl2f1a(tree *t,int k,int pn,int pa,int x,int y)
{
	tree *t0,*t1,*t2; 
	switch (t->node[0]) {
		case 'A': t1=actl2f1a(t->right,k,pn,pa+1,pa+1,0);
			  if (x==0) return t1;
			  t0=eqstate(x,y,pa+1,0,k);
			  return treecompose(":",t0,t1);
		case 'X': if (y==k) return treeconst(0);
			  return t1=actl2f1a(t->right,k,pn,pa,x,y+1);
		case 'F': t0=actl2f1a(t->right,k,pn,pa,x,y); 
			  if (y==k) return t0;
			  pa+=fkx(t->right,k);
			  t2=actl2f1a(t,k,pn,pa,x,y+1); 
			  return treecompose("|",t0,t2);
		case 'G': if (t->left!=NULL) 
			  return makeform_ctl(actl2f1a,
				t->right,k,pn,pa,x,y,atoi(t->left->node));
			  t0=actl2f1a(t->right,k,pn,pa,x,y); 
			  if (y==k) return treecompose("&",t0,eqpath(x,k));
			  t2=actl2f1a(t,k,pn,pa,x,y+1); 
			  return treecompose("&",t0,t2);
		case 'U': t0=actl2f1a(t->right,k,pn,pa,x,y); 
			  if (y==k) return t0;
			  pa+=fkx(t->right,k);
			  t1=actl2f1a(t->left,k,pn,pa,x,y); 
			  t2=actl2f1a(t,k,pn,pa,x,y+1); 
			  return treecompose("|",t0,treecompose("&",t1,t2));
		case 'R': t0=actl2f1a(t->right,k,pn,pa,x,y); 
			  t1=actl2f1a(t->left,k,pn,pa,x,y); 
			  if (y==k) return treecompose("&",t0, 
				treecompose("|",t1,eqpath(x,k)));
			  pa+=fkx(t->left,k);
			  t2=actl2f1a(t,k,pn,pa,x,y+1); 
			  return treecompose("&",t0,treecompose("|",t1,t2));
		case '&': t0=actl2f1a(t->left,k,pn,pa,x,y); 
			  t1=actl2f1a(t->right,k,pn,pa,x,y); 
			  return treecompose("&",t0,t1);
		case '|': t0=actl2f1a(t->left,k,pn,pa,x,y); 
			  t1=actl2f1a(t->right,k,pn,pa,x,y); 
			  return treecompose("|",t0,t1);
		case '!': 
		default:  
			if (k==0||x==0) x=1;
			return treeaddnumber(treecopy(t),(x-1)*pn*(k+1)+y*pn);
	}
}
/*******************************************************************/
tree *actl2f0a(tree *t,int k,int pn,int pa,int x,int y)
{
	tree *t0,*t1,*t2; 

	switch (t->node[0]) {
		case 'E': t1=actl2f0a(t->right,k,pn,pa+1,pa+1,0);
			  if (x==0) return t1;
			  t0=eqstate(x,y,pa+1,0,k);
			  return treecompose(":",t0,t1);
		case 'X': if (y==k) return treeconst(0);
			  return t1=actl2f0a(t->right,k,pn,pa,x,y+1);
		case 'F': t0=actl2f0a(t->right,k,pn,pa,x,y); 
			  if (y==k) return t0;
			  t2=actl2f0a(t,k,pn,pa,x,y+1); 
			  return treecompose("|",t0,t2);
		case 'G': if (t->left!=NULL) 
			  return makeform_ctl(actl2f0a,
				t->right,k,pn,pa,x,y,atoi(t->left->node));
			  t0=actl2f0a(t->right,k,pn,pa,x,y); 
			  pa+=gkx(t->right,k);
			  if (y==k) return treecompose("&",t0,eqpath(x,k));
			  t2=actl2f0a(t,k,pn,pa,x,y+1); 
			  return treecompose("&",t0,t2);
		case 'U': t0=actl2f0a(t->right,k,pn,pa,x,y); 
			  if (y==k) return t0;
			  t1=actl2f0a(t->left,k,pn,pa,x,y); 
			  pa+=gkx(t->left,k);
			  t2=actl2f0a(t,k,pn,pa,x,y+1); 
			  return treecompose("|",t0,treecompose("&",t1,t2));
		case 'R': t0=actl2f0a(t->right,k,pn,pa,x,y); 
			  pa+=gkx(t->right,k);
			  t1=actl2f0a(t->left,k,pn,pa,x,y); 
			  if (y==k) return treecompose("&",t0,
				treecompose("|",t1,eqpath(x,k)));
			  t2=actl2f0a(t,k,pn,pa,x,y+1); 
			  return treecompose("&",t0,treecompose("|",t1,t2));
		case '&': t0=actl2f0a(t->left,k,pn,pa,x,y); 
			  t1=actl2f0a(t->right,k,pn,pa,x,y); 
			  return treecompose("&",t0,t1);
		case '|': t0=actl2f0a(t->left,k,pn,pa,x,y); 
			  t1=actl2f0a(t->right,k,pn,pa,x,y); 
			  return treecompose("|",t0,t1);
		case '!': 
		default:  
			if (k==0||x==0) x=1;
			return treeaddnumber(treecopy(t),(x-1)*pn*(k+1)+y*pn);
	}
}
/*******************************************************************/
int fkx(tree *t,int k)
{
        int x,y,z;
        if (t==NULL|k==0) return 0;
        switch (t->node[0]) {
                case '&': x=fkx(t->left,k); y=fkx(t->right,k); return x>y?x:y;
                case '|': x=fkx(t->left,k); y=fkx(t->right,k); return x+y;
                case 'A': return fkx(t->right,k)+1;
                case 'X': return fkx(t->right,k);
                case 'F': return (k+1)*fkx(t->right,k);
                case 'G': return fkx(t->right,k);
                case 'U': x=fkx(t->left,k); y=fkx(t->right,k);
                          z=(x>y?x:y); return k*y+z;
                case 'R': x=fkx(t->left,k); y=fkx(t->right,k);
                          z=(x>y?x:y); return k*x+z;
		default: return 0;
        }
}
/*******************************************************************/
int gkx(tree *t,int k)
{       
        int x,y,z;
        if (t==NULL|k==0) return 0;
        switch (t->node[0]) {
                case '&': x=gkx(t->left,k); y=gkx(t->right,k); return x+y;
                case '|': x=gkx(t->left,k); y=gkx(t->right,k); return x>y?x:y;
                case 'E': return gkx(t->right,k)+1;
                case 'X': return gkx(t->right,k);
                case 'F': return gkx(t->right,k);
                case 'G': return (k+1)*gkx(t->right,k);
                case 'U': x=gkx(t->left,k); y=gkx(t->right,k); return k*x+y;
                case 'R': x=gkx(t->left,k); y=gkx(t->right,k); return (k+1)*y+x;
		default: return 0;
        }
}
/*******************************************************************/
/*
tree *actl2f1aOO(tree *t,int k,int pn,int pa,int x,int y)
{
	tree *t0,*t1,*t2; 

	switch (t->node[0]) {
		case 'A': t1=actl2f1a(t->right,k,pn,pa+1,pa+1,0);
			  if (x==0) return t1;
			  t0=eqstate(x,y,pa+1,0,k);
			  return treecompose(":",t0,t1);
		case 'X': if (y==k) return treeconst(0);
			  return t1=actl2f1a(t->right,k,pn,pa,x,y+1);
		case 'F': t0=actl2f1a(t->right,k,pn,pa,x,y); 
			  if (y==k) return t0;
			  pa+=fkx(t->right,k);
			  t2=actl2f1a(t,k,pn,pa,x,y+1); 
			  return treecompose("|",t0,t2);
		case 'G': t0=actl2f1a(t->right,k,pn,pa,x,y); 
			  if (y==k) return treecompose("&",t0,eqpath(x,k));
			  t2=actl2f1a(t,k,pn,pa,x,y+1); 
			  return treecompose("&",t0,t2);
		case 'U': t0=actl2f1a(t->right,k,pn,pa,x,y); 
			  if (y==k) return t0;
			  pa+=fkx(t->right,k);
			  t1=actl2f1a(t->left,k,pn,pa,x,y); 
			  t2=actl2f1a(t,k,pn,pa,x,y+1); 
			  return treecompose("|",t0,treecompose("&",t1,t2));
		case 'R': t0=actl2f1a(t->right,k,pn,pa,x,y); 
			  t1=actl2f1a(t->left,k,pn,pa,x,y); 
			  if (y==k) return treecompose("&",t0, 
				treecompose("|",t1,eqpath(x,k)));
			  pa+=fkx(t->left,k);
			  t2=actl2f1a(t,k,pn,pa,x,y+1); 
			  return treecompose("&",t0,treecompose("|",t1,t2));
		case '&': t0=actl2f1a(t->left,k,pn,pa,x,y); 
			  t1=actl2f1a(t->right,k,pn,pa,x,y); 
			  return treecompose("&",t0,t1);
		case '|': t0=actl2f1a(t->left,k,pn,pa,x,y); 
			  t1=actl2f1a(t->right,k,pn,pa,x,y); 
			  return treecompose("|",t0,t1);
		case '!': 
		default:  return treeaddnumber(treecopy(t),(x-1)*pn*(k+1)+y*pn);
	}
}
/*******************************************************************/

/*******************************************************************/
tree *SPECactl2f0CTLR(tree *t,int k,int pn)
{
	return CTLRactl2f0a(t,k,pn,0,0,0); 
}
tree *SPECactl2f1CTLR(tree *t,int k,int pn)
{
	return CTLRactl2f1a(t,k,pn,0,0,0); 
}
/*******************************************************************/
tree *eqpathj(int pa,int k,int j)
{
        int i;
        tree *t0;

        if (j==0) return treeconst(0);

        t0=eqstate(pa,0,pa,j,k);
        for (i=1;i<j;i++) t0=treecompose("|",t0,eqstate(pa,i,pa,j,k));
        return t0;
}
/*******************************************************************/
tree *CTLRactl2f1a(tree *t,int k,int pn,int pa,int x,int y)
{
	tree *t0,*t1,*t2; 
	switch (t->node[0]) {
		case 'A': t1=CTLRactl2f1a(t->right,k,pn,pa+1,pa+1,0);
			  if (x==0) return t1;
			  t0=eqstate(x,y,pa+1,0,k);
			  return treecompose(":",t0,t1);
		case 'X': if (y==k) return treeconst(0);
			  return t1=CTLRactl2f1a(t->right,k,pn,pa,x,y+1);
		case 'F': t0=CTLRactl2f1a(t->right,k,pn,pa,x,y); 
			  if (y==k) return t0;
			  pa+=CTLRfkx(t->right,k);
			  t2=CTLRactl2f1a(t,k,pn,pa,x,y+1); 
			  return treecompose("|",t0,t2);
		case 'G': if (t->left!=NULL) 
			  return makeform_ctl(CTLRactl2f1a,
				t->right,k,pn,pa,x,y,atoi(t->left->node));
			  if (k==0) return treeconst(0);
			  t0=CTLRactl2f1a(t->right,k,pn,pa,x,y); 
			  if (y==k-1) 
			  return treecompose("&",t0,eqpathj(x,k,y+1));
			  t2=CTLRactl2f1a(t,k,pn,pa,x,y+1); 
			  if (y<k-1) t2=treecompose("|",eqpathj(x,k,y+1),t2);
			  return treecompose("&",t0,t2);
		case 'U': t0=CTLRactl2f1a(t->right,k,pn,pa,x,y); 
			  if (y==k) return t0;
			  pa+=CTLRfkx(t->right,k);
			  t1=CTLRactl2f1a(t->left,k,pn,pa,x,y); 
			  t2=CTLRactl2f1a(t,k,pn,pa,x,y+1); 
			  return treecompose("|",t0,treecompose("&",t1,t2));
		case 'R': t0=CTLRactl2f1a(t->right,k,pn,pa,x,y); 
			  t1=CTLRactl2f1a(t->left,k,pn,pa,x,y); 
			  if (y==k) return treecompose("&",t0,t1); 
			  pa+=CTLRfkx(t->left,k);
			  t2=CTLRactl2f1a(t,k,pn,pa,x,y+1); 
			  if (y<=k-1) t2=treecompose("|",eqpathj(x,k,y+1),t2);
			  return treecompose("&",t0,treecompose("|",t1,t2));
		case '&': t0=CTLRactl2f1a(t->left,k,pn,pa,x,y); 
			  t1=CTLRactl2f1a(t->right,k,pn,pa,x,y); 
			  return treecompose("&",t0,t1);
		case '|': t0=CTLRactl2f1a(t->left,k,pn,pa,x,y); 
			  t1=CTLRactl2f1a(t->right,k,pn,pa,x,y); 
			  return treecompose("|",t0,t1);
		case '!': 
		default:  
			if (k==0||x==0) x=1;
			return treeaddnumber(treecopy(t),(x-1)*pn*(k+1)+y*pn);
	}
}
/*******************************************************************/
tree *CTLRactl2f0a(tree *t,int k,int pn,int pa,int x,int y)
{
	tree *t0,*t1,*t2; 
	switch (t->node[0]) {
		case 'E': t1=CTLRactl2f0a(t->right,k,pn,pa+1,pa+1,0);
			  if (x==0) return t1;
			  t0=eqstate(x,y,pa+1,0,k);
			  return treecompose(":",t0,t1);
		case 'X': if (y==k) return treeconst(0);
			  return t1=CTLRactl2f0a(t->right,k,pn,pa,x,y+1);
		case 'F': t0=CTLRactl2f0a(t->right,k,pn,pa,x,y); 
			  if (y==k) return t0;
			  t2=CTLRactl2f0a(t,k,pn,pa,x,y+1); 
			  return treecompose("|",t0,t2);
		case 'G': if (t->left!=NULL) 
			  return makeform_ctl(CTLRactl2f0a,
				t->right,k,pn,pa,x,y,atoi(t->left->node));
			  if (k==0) return treeconst(0);
			  t0=CTLRactl2f0a(t->right,k,pn,pa,x,y); 
/*
			  if (y==k-1) return treecompose("&",t0,eqpath(x,k));
*/
			  if (y==k-1) 
			  return treecompose("&",t0,eqpathj(x,k,y+1));
			  pa+=CTLRgkx(t->right,k);
			  t2=CTLRactl2f0a(t,k,pn,pa,x,y+1); 
/*
			  if (y<k-1) t2=treecompose("|",eqpathj(x,k,y+1),t2);
*/
			  return treecompose("&",t0,t2);
		case 'U': t0=CTLRactl2f0a(t->right,k,pn,pa,x,y); 
			  if (y==k) return t0;
			  t1=CTLRactl2f0a(t->left,k,pn,pa,x,y); 
			  pa+=CTLRgkx(t->left,k);
			  t2=CTLRactl2f0a(t,k,pn,pa,x,y+1); 
			  return treecompose("|",t0,treecompose("&",t1,t2));
		case 'R': t0=CTLRactl2f0a(t->right,k,pn,pa,x,y); 
			  pa+=CTLRgkx(t->right,k);
			  t1=CTLRactl2f0a(t->left,k,pn,pa,x,y); 
			  if (y==k) return treecompose("&",t0,t1);
			  t2=CTLRactl2f0a(t,k,pn,pa,x,y+1); 
			  if (y==k-1) t2=treecompose("|",eqpathj(x,k,y+1),t2);
			  return treecompose("&",t0,treecompose("|",t1,t2));
		case '&': t0=CTLRactl2f0a(t->left,k,pn,pa,x,y); 
			  t1=CTLRactl2f0a(t->right,k,pn,pa,x,y); 
			  return treecompose("&",t0,t1);
		case '|': t0=CTLRactl2f0a(t->left,k,pn,pa,x,y); 
			  t1=CTLRactl2f0a(t->right,k,pn,pa,x,y); 
			  return treecompose("|",t0,t1);
		case '!': 
		default:  
			if (k==0||x==0) x=1;
			return treeaddnumber(treecopy(t),(x-1)*pn*(k+1)+y*pn);
	}
}
/*******************************************************************/
int CTLRfkx(tree *t,int k)
{
        int x,y,z;
        if (t==NULL|k==0) return 0;
        switch (t->node[0]) {
                case '&': x=CTLRfkx(t->left,k); y=CTLRfkx(t->right,k); return x>y?x:y;
                case '|': x=CTLRfkx(t->left,k); y=CTLRfkx(t->right,k); return x+y;
                case 'A': return CTLRfkx(t->right,k)+1;
                case 'X': return CTLRfkx(t->right,k);
                case 'F': return (k+1)*CTLRfkx(t->right,k);
                case 'G': return CTLRfkx(t->right,k);
                case 'U': x=CTLRfkx(t->left,k); y=CTLRfkx(t->right,k);
                          z=(x>y?x:y); return k*y+z;
                case 'R': x=CTLRfkx(t->left,k); y=CTLRfkx(t->right,k);
                          z=(x>y?x:y); return k*x+z;
		default: return 0;
        }
}
/*******************************************************************/
int CTLRgkx(tree *t,int k)
{       
        int x,y,z;
        if (t==NULL) return 0;
        switch (t->node[0]) {
                case '&': x=CTLRgkx(t->left,k); y=CTLRgkx(t->right,k); return x+y;
                case '|': x=CTLRgkx(t->left,k); y=CTLRgkx(t->right,k); return x>y?x:y;
                case 'E': return CTLRgkx(t->right,k)+1;
                case 'X': return CTLRgkx(t->right,k);
                case 'F': return CTLRgkx(t->right,k);
/*
                case 'G': return (k+1)*CTLRgkx(t->right,k);
*/
                case 'G': return (k)*CTLRgkx(t->right,k);
                case 'U': x=CTLRgkx(t->left,k); y=CTLRgkx(t->right,k); return k*x+y;
                case 'R': x=CTLRgkx(t->left,k); y=CTLRgkx(t->right,k); return (k+1)*y+x;
		default: return 0;
        }
}
/*******************************************************************/

/*******************************************************************/
#define myprints(f,s,m)   	{ m[++*m]=s; }
#define myprints0(f,s0,m) 	{ if (s0) m[++*m]=-s0; m[++*m]=0; }
/*******************************************************************/
int CNFnnf2cnf_1(tree *t,char c,int s0,int s,int *mstring)
{
	int i;
	if (t==NULL) return s;
	if (t->node[0]==c) { 
		i=CNFnnf2cnf_1(t->left,c,s0,s,mstring); 
		i=CNFnnf2cnf_1(t->right,c,s0,i,mstring); return i;
	} 
        if (t->node[0]=='!') {
                myprints(stdout,-atoi(t->right->node),mstring);
                if (c=='&') myprints0(stdout,s0,mstring);
        } else if (isdigit(t->node[0])) {
                myprints(stdout,atoi(t->node),mstring);
                if (c=='&') myprints0(stdout,s0,mstring);
        } else if (c=='&') {
                sprintf(t->node+1,"%i",s0);
        } else /*if (c=='|')*/ {
                myprints(stdout,++s,mstring); sprintf(t->node+1,"%i",s);
                if (c=='&') myprints0(stdout,s0,mstring);
        }
        return s;
}
int CNFnnf2cnf_2(tree *t,char c,int s,int *mstring)
{
	int i;
	if (t==NULL) return s;
	if (t->node[0]==c) { 
		i=CNFnnf2cnf_2(t->left,c,s,mstring); 
		i=CNFnnf2cnf_2(t->right,c,i,mstring); return i;
	} 
	if (t->node[0]=='!') return s; 
	else if (isdigit(t->node[0])) return s;
	else return CNFnnf2cnf(t,t->node[0],atoi(t->node+1),s,mstring);
}
int CNFnnf2cnf(tree *t,char c,int s0,int auxp,int *mstring)
{
	int i;

	if (t==NULL) return auxp;

	if (c==0) {
		if (t->node[0]=='!'||isdigit(t->node[0]))
			treereplace(t,treecompose("&",treecopy(t),treecopy(t)));
		c=t->node[0];
	}
	i=CNFnnf2cnf_1(t,c,s0,auxp,mstring); 
	if (c=='|') myprints0(stdout,s0,mstring); 
	i=CNFnnf2cnf_2(t,c,i,mstring); 
	return i;
}
/*******************************************************************/
int *CNFrotate(int *s1,int *s2,int n,int a1,int a2,int b1,int b2,int bn,int au)
{
        int j,s;
        int a1a,a1b,a2a,a2b,b1a,b2a;

        a1a=(a1-1)*bn+1; a1b=a1*bn;
        a2a=(a2-1)*bn+1; a2b=a2*bn;
        b1a=(b1-1)*bn+1-a1a; b2a=(b2-1)*bn+1-a2a;

        for (j=0;j<n;j++)
        if (s1[j]==0) s2[j]=0; else {
            if (s1[j]>0) s=1; else s=-1;
            if (a1a<=s*s1[j]&&s*s1[j]<=a1b) s2[j]=s1[j]+b1a*s;
            else if (a2a<=s*s1[j]&&s*s1[j]<=a2b) s2[j]=s1[j]+b2a*s;
            else s2[j]=s1[j]+au*s;
        }
	return s2;
}
/*******************************************************************/

/*******************************************************************/
tree *SPECltl2f0(tree *t,int k,int lstart,int pn,tree *tr)
{
	tree *t0,*t1; 

	if (lstart==-1) {
		t0=(tree *)malloc(sizeof(tree));	
		t0->node[0]='|'; t0->node[1]=0; 
		t0->left=ltl2f0b(t,k,0,pn,lstart,0); 
		t0->right=SPECltl2f0(t,k,lstart+1,pn,tr); 
		return t0;
	} else { 
		t0=(tree *)malloc(sizeof(tree));
		t0->node[0]='&'; t0->node[1]=0; 
		t0->left=treetr(tr,k,lstart,pn);
		t0->right=ltl2f0b(t,k,0,pn,lstart,0); if (lstart==k) return t0;
		t1=(tree *)malloc(sizeof(tree));
		t1->node[0]='|'; t1->node[1]=0; 
		t1->left=t0; 
		t1->right=SPECltl2f0(t,k,lstart+1,pn,tr); return t1;
	}
}
/*******************************************************************/
tree *SPECltl2f1(tree *t,int k,int i,int pn)
{
	tree *t0,*t1,*t2; 

	switch (t->node[0]) {
		case 'X': if (k==i) return treeconst(1);
			  else return SPECltl2f1(t->right,k,i+1,pn); 
		case 'G': return makeform_ll(SPECltl2f1,t->right,k,i,k,'&',pn); 
		case 'F': return treeconst(1);
		case 'U': t0=SPECltl2f1(t->right,k,i,pn); 
			  t1=SPECltl2f1(t->left,k,i,pn);
			  if (i==k) return treecompose("|",t0,t1);
			  t2=SPECltl2f1(t,k,i+1,pn); 
			  return treecompose("|",t0,treecompose("&",t1,t2));
		case 'R': t0=SPECltl2f1(t->right,k,i,pn); 
			  if (i==k) return t0;
			  t1=SPECltl2f1(t->left,k,i,pn);
			  t2=SPECltl2f1(t,k,i+1,pn); 
			  return treecompose("&",t0,treecompose("|",t1,t2));
		case '&': t0=SPECltl2f1(t->left,k,i,pn); 
			  t1=SPECltl2f1(t->right,k,i,pn); 
			  return treecompose("&",t0,t1);
		case '|': t0=SPECltl2f1(t->left,k,i,pn); 
			  t1=SPECltl2f1(t->right,k,i,pn); 
			  return treecompose("|",t0,t1);
		case '!': 
		default:  return treeaddnumber(treecopy(t),i*pn);
	}
}
/*******************************************************************/
tree *makeform_ll(tree *f(),tree *t,int k,int i,int j,char c,int pn)
{
	tree *t0,*t1;;

	if (i>j) return NULL;
	if (i==j) return f(t,k,i,pn); 
	t0=(tree *)malloc(sizeof(tree));	
	t0->node[0]=c; t0->node[1]=0; 
	t0->left=f(t,k,i,pn); t0->right=makeform_ll(f,t,k,i+1,j,c,pn); 
	return t0;
}
/*******************************************************************/
tree *makeform_l0(tree *f(),tree *t,int k,int i,int j,char c,int pn,int ll)
{
	tree *t0,*t1;;

	if (i>j) return NULL;
	if (i==j) return f(t,k,i,pn,ll); 
	t0=(tree *)malloc(sizeof(tree));	
	t0->node[0]=c; t0->node[1]=0; 
	t0->left=f(t,k,i,pn,ll,0); t0->right=makeform_l0(f,t,k,i+1,j,c,pn,ll); 
	return t0;
}
/*******************************************************************/
tree *ltl2f0b(tree *t,int k,int i,int pn,int ll,int j)
{
	tree *t0,*t1,*t2,*t3; 

	switch (t->node[0]) {
		case 'X': if (k==i&&ll>=0) i=ll; else i=i+1;
			  if (i>k) return treeconst(0); 
			  return ltl2f0b(t->right,k,i+1,pn,ll,j); 
		case 'G': if (ll<0) return treeconst(0); 
			  if (ll<i) i=ll;
			  return makeform_l0(ltl2f0b,t->right,k,i,k,'&',pn,ll);
		case 'F': if (i>ll&&ll>=0) i=ll;
			  return makeform_l0(ltl2f0b,t->right,k,i,k,'|',pn,ll); 
		case 'U': t->node[0]++; t0=ltl2f0b(t,k,i,pn,ll,k); t->node[0]--;
			  if (!(i>ll&&ll>=0)) return t0;
			  t1=makeform_l0(ltl2f0b,t->left,k,i,k,'&',pn,ll); 
			  t->node[0]++; t2=ltl2f0b(t,k,ll,pn,ll,i-1); 
			  t->node[0]--;
			  return treecompose("|",t0,treecompose("&",t1,t2));
		case 'R': t->node[0]++; t0=ltl2f0b(t,k,i,pn,ll,k); t->node[0]--;
			  if (!(ll>=0)) return t0;
			  t1=makeform_l0(ltl2f0b,t->left,k,i,k,'&',pn,ll); 
		          if (i<=ll) return treecompose("|",t0,t1);
			  t->node[0]++; t2=ltl2f0b(t,k,ll,pn,ll,i-1); 
			  t->node[0]--;
			  t3=makeform_l0(ltl2f0b,t->right,k,ll,i-1,'&',pn,ll); 
			  t2=treecompose("|",t2,t3);
			  return treecompose("|",t0,treecompose("&",t1,t2));
		case 'V': t0=ltl2f0b(t->right,k,i,pn,ll,j); 
			  if (i==j) return t0;
			  t1=ltl2f0b(t->left,k,i,pn,ll,j); 
			  t2=ltl2f0b(t,k,i+1,pn,ll,j); 
			  return treecompose("|",t0,treecompose("&",t1,t2));
		case 'S': t0=ltl2f0b(t->right,k,i,pn,ll,j); 
			  t1=ltl2f0b(t->left,k,i,pn,ll,j); 
			  if (i==j) return treecompose("&",t0,t1);
			  t2=ltl2f0b(t,k,i+1,pn,ll,j); 
			  return treecompose("&",t0,treecompose("|",t1,t2));
		case '&': t0=ltl2f0b(t->left,k,i,pn,ll,j); 
			  t1=ltl2f0b(t->right,k,i,pn,ll,j); 
			  return treecompose("&",t0,t1);
		case '|': t0=ltl2f0b(t->left,k,i,pn,ll,j); 
			  t1=ltl2f0b(t->right,k,i,pn,ll,j); 
			  return treecompose("|",t0,t1);
		case '!': 
		default:  return treeaddnumber(treecopy(t),i*pn);
	}
}
/*******************************************************************/

/*******************************************************************/
tree *SPECreplcolon(tree *t)
{
	if (t->node[0]==':') { t->node[0]='&'; } 
	return t;
}
tree *SPECreplcolonneg(tree *t)
{
	if (t->node[0]==':') { t->node[0]='|'; t->left=TLFnegate(t->left); } 
	return t;
}
/*******************************************************************/
int SPECnpath(tree *spec,int bv,int k,int ctl_opt)
{
        int npath;
        tree *ts;

        if (bv) { npath=fkx(spec,k); }
        else {
                ts=TLFnegate(treecopy(spec));
                if (ctl_opt&32) npath=CTLRgkx(ts,k); else npath=gkx(ts,k);
                treefree(ts);
        }

        if (npath==0) npath=1;
        return npath;
}
/*******************************************************************/
