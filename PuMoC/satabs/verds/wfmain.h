extern char *PAR_qbfsolver;
extern char *PAR_satsolver;
extern int PAR_m;
extern int PAR_bound;
extern int PAR_tm;
extern time_t t0,t1;
int qbfbmc(workspace *ws2,tree *spec);
int satbmc(workspace *ws2,tree *spec);
void qmdwritecnf(qmdata *qm,qinfo *qi,FILE *g);
model *qmd2model(qmdata *qm,qinfo *qi);
extern dbase *db;
extern workspace *ws1,*ws2;
extern qinfo *qi;
extern qmdata *qm;
extern dbase *db;
extern workspace *ws1,*ws2;
extern dbase *db;
extern workspace *ws1,*ws2;
extern dbase *db;
extern workspace *ws1,*ws2;
extern dbase *db;
extern workspace *ws1,*ws2;
dbase *db;
workspace *ws1,*ws2;
extern dbase *db;
extern workspace *ws1,*ws2;
extern O1npath;
extern char **ORDv1;
extern char **ORDv2;
extern int *ORDv0;
extern int PAR_tm;
extern int PAR_ag;
extern int PAR_verify;
extern int PAR_test;
extern int PAR_m;
extern int PAR_option;
extern int PAR_p;
extern int PAR_ck;
extern char *PAR_ifn;
extern char *PAR_qbfsolver;
extern char *PAR_satsolver;
extern char *PAR_v;
extern time_t t0,t1;
extern int PAR_bmc;
extern int PAR_bound;
void fnesscheck(tree *prog);
tree *fnessget(tree *prog);
tree *fnessaddvlist(tree *list,int n);
tree *fnessaddilist(tree *list,int n);
void fnessaddtlist(int n,tree *dbpr);
void fnessaddt(tree *list,int n);
void *fnessaddprog(tree *prog,tree *processes);
model *createnonrepmm0(workspace *ws2,int k);
model *addnonrep(workspace *ws2,int k,model *mo);
int O1SOLVEapply(char *solver,model *mo,tree *spec,int k,int low,int h,int bv);
int param(char **argv, int i);
void checkfiles();
int param_p(tree *plist);
int message_ck(tree *plist);
void var_order(tree *vlist,tree *vlist_old,int n);
void var_order_vn(char *l1,char *l2,tree *vlist_old);
void getsolvers();
int report(int start,char *fn,tree *p,int r,int k);
int SOLVEapplyq(char *solver,qmdata *qm,qinfo *qi,int bv);
int qm2file(qmdata *qm,qinfo *qi,int bv,int k);
tree *TLFtype1(tree *fml);
tree *TLFtype2(tree *fml);
tree *TLFtype3(tree *fml);
int isTLFtypeAEorE(tree *fml);
extern char *PAR_qbfsolver;
extern char *PAR_satsolver;
extern int PAR_m;
extern int PAR_bound;
extern int PAR_tm;
extern time_t t0,t1;
int qbfbmc(workspace *ws2,tree *spec);
int satbmc(workspace *ws2,tree *spec);
void qmdwritecnf(qmdata *qm,qinfo *qi,FILE *g);
model *qmd2model(qmdata *qm,qinfo *qi);
extern dbase *db;
extern workspace *ws1,*ws2;
extern qinfo *qi;
extern qmdata *qm;
extern dbase *db;
extern workspace *ws1,*ws2;
extern dbase *db;
extern workspace *ws1,*ws2;
extern dbase *db;
extern workspace *ws1,*ws2;
extern dbase *db;
extern workspace *ws1,*ws2;
dbase *db;
workspace *ws1,*ws2;
extern dbase *db;
extern workspace *ws1,*ws2;
extern int PAR_tm;
extern int PAR_m;
extern int PAR_option;
extern char *PAR_qbfsolver;
extern char *PAR_satsolver;
extern time_t t0,t1;
extern int PAR_bmc;
extern int PAR_bound;
void fnesscheck(tree *prog);
tree *fnessget(tree *prog);
tree *fnessaddvlist(tree *list,int n);
tree *fnessaddilist(tree *list,int n);
void fnessaddtlist(int n,tree *dbpr);
void fnessaddt(tree *list,int n);
void *fnessaddprog(tree *prog,tree *processes);
model *createnonrepmm0(workspace *ws2,int k);
model *addnonrep(workspace *ws2,int k,model *mo);
int O1SOLVEapply(char *solver,model *mo,tree *spec,int k,int low,int h,int bv);
int param(char **argv, int i);
void checkfiles();
int param_p(tree *plist);
int message_ck(tree *plist);
void var_order(tree *vlist,tree *vlist_old,int n);
void var_order_vn(char *l1,char *l2,tree *vlist_old);
void getsolvers();
int report(int start,char *fn,tree *p,int r,int k);
int SOLVEapplyq(char *solver,qmdata *qm,qinfo *qi,int bv);
int qm2file(qmdata *qm,qinfo *qi,int bv,int k);
tree *TLFtype1(tree *fml);
tree *TLFtype2(tree *fml);
tree *TLFtype3(tree *fml);
int isTLFtypeAEorE(tree *fml);