int remove_red(int *p0,int k);
int remove_red2(int *p0);
int intin(int a,int *b,int x,int y);
void freeqmdata(qmdata *md);
int qmdwrite(qmdata *md,qinfo *qi,FILE *g);
int qmdwrite1(qmdata *md,FILE *g,int mode);
void pnintwrite(int *ss,FILE *g);
void qmdsimp(qmdata *md,qinfo *qi);
void mdmoveup1(qmdata *md0,qmdata *md,qmdata *mdq);
int *mdintegrate(int *a1,int *a2);
int qmdlen(qmdata *md);
int qmdm(qmdata *md);
float utiltime(int finish);
int fileok(char *s);
int intlen(int *ss);
void quicksort_ab(int *a,int *b,int l,int r ) /* decreasing */;
void quicksort_abf(float *a,int *b,int l,int r) /* decreasing */;
char *file2string(char *fn);
void intwrite(int *ss,int a,int b,FILE *g);
void charwrite(char *ss,int a,int b,FILE *g);
void cnfstringwrite(int *mstring,int mstringc,FILE *g);
void prettyintwrite(int *ss,int a,int b,FILE *g);
