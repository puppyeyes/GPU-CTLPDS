int inclausepn(int c0,int *clause);
int intclausewritelen(int level,qinfo *qi,FILE *g);
int intclausewriteall(int c0,qinfo *qi,FILE *g);
int simp(qinfo *qi,int rt);
int simpprepare(qinfo *qi,char *gim,int *clen,int *cure);
int assignable(qinfo *qi,int c0,int val,qcdata *cd,int *clen);
int assignablee(int c0,int val,qcdata *cd,int *clen);
int assignablea(int c0,int val,qcdata *cd,int *clen);
int assignableec(int c0,int val,qcdata *cd,int *clen);
int assignableed(int c0,int val,qcdata *cd,int *clen);
int assignablead(int c0,int val,qcdata *cd,int *clen);
int assignableac(int c0,int val,qcdata *cd,int *clen);
int qunitp(int rt,qinfo *qi,int *clen,int *cur);
int qunitprop1(qinfo *qi,int rt,int neg,int c,sdata *sd,qcdata *cd,int *clen,int *cure,char *gim);
int qunitprop0(qinfo *qi,int rt,int c0,int val,qcdata *cd,int *clen,int *cure,char *gim);
int qunitgetliteral(qinfo *qi,int c0,qcdata *cd,int *clen,char *gim);
