extern int m2aux;;
extern TBD **m2cache;;
extern TBD **m2fness;;
TBD *f2eg(TBD* r0);
TBD *f2eu(TBD *r0,TBD *r0a);
TBD *f2ef(TBD *r0);
TBD *f2er(TBD *r0,TBD *r0a);
TBD *f2xx(TBD *r0);
TBD *f2ega(TBD* r0);
TBD *f2eua(TBD *r0,TBD *r0a);
TBD *f2efa(TBD *r0);
TBD *f2era(TBD *r0,TBD *r0a);
TBD *f2xxa(TBD *r0);
extern int m2rcount;;
extern int m2rcounta;;
extern int *m2order;;
extern int *m2oi;;
extern int *m2varcount;;
extern char **m2vlist;;
extern char **m2rlist;;
int m2start(tree *init,tree *trs,tree *fness,tree *prop,int n,int opt,char **v1,char **v2,int *v0);
TBD *m2mc(tree *prop);
TBD *m2prop(tree *prop);
TBD *m2prope(tree *prop);
TBD *m2tbd(tree *prop);
int m2ordersequential(int n,int reverse);
int m2orderinterleave(int n,int reverse);
int m2trel(tree *prop,int c);
int m2ptrel(tree *prop,int c);
int m2orderinit(char **v1,char **v2,int *v0,int n);
TBD *m2fg2(TBD* r0,TBD **r1,int n,TBD *fg(),TBD *f());
TBD *m2ur2(TBD* r0,TBD *r0a,TBD **r1,int n,TBD *f(),TBD *g(),TBD *exf());
TBD *m2exone2(TBD* r0,TBD *r1,int bn,int len,TBD *f());
TBD *m2ex2(TBD* r0,TBD **r1,int n,TBD *f());
TBD *m2fg(TBD* r0,TBD **r1,int n,TBD *fg(),TBD *f());
TBD *m2ur(TBD* r0,TBD *r0a,TBD **r1,int n,TBD *f(),TBD *g(),TBD *exf());
TBD *m2exone(TBD* r0,TBD *r1,int bn,int len,TBD *f());
TBD *m2ex(TBD* r0,TBD **r1,int n,TBD *f());
void m2var(tree *prop,int n);
void m2initorder(tree *trs,int n);
void m2neworder(tree *trs,int i,int n);
int m2var1(tree *prop,int n,int v);
void m2randorder(int *clause,int a,int b);
int m2TLFsub(tree *tf);
int m2TLFisalla(tree *tf);
int m2TLFtype(tree *tf);
tree *m2TLFgetag(tree *t);
tree *m2TLFgetaf(tree *t);
int m2TLFdualopr(char a);
tree *m2TLFnegate(tree *t);
TBD *m2bdd(TBD* w);
int main(int argc, char *argv[]);
