int isprop(tree *t);
int opnumber(char a) ;
tree *negate(tree *t);
void nnftemp(tree *t);
tree *nnf(tree *t);
tree *cnf(tree *t);
tree *forsimplify_basics(tree *t);
tree *forsimplify(tree *t);
tree *doall(tree *t,tree *f());
tree *dnf(tree *t);
