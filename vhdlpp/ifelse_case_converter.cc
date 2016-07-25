#include "ifelse_case_converter.h"
#include "mach7_includes.h"
#include "sequential.h"
#include "architec.h"
#include "expression.h"

using namespace std;
using namespace mch;

bool BranchesToCases::containsIfSequential(const list<SequentialStmt*> &slist){
    for (auto &i : slist){
        Match(i){
            Case(C<IfSequential>()){
                return true;
            }
        } EndMatch;
    }

    return false;
}

// performes complete deep clone of if/else statement lists
CaseSeqStmt *BranchesToCases::makeCaseSeq(const Expression *cond,
                                          const list<SequentialStmt *> &if_,
                                          const list<SequentialStmt *> &else_){
    ExpName *tr = new ExpName(perm_string::literal("TRUE"));
    ExpName *fa = new ExpName(perm_string::literal("FALSE"));

    list<Expression *> *tmpTrue, *tmpFalse;
    tmpTrue = new list<Expression*>{tr};
    tmpFalse = new list<Expression*>{fa};

    list<SequentialStmt *> tmpIf, tmpElse;
    for (auto &i : if_)
        tmpIf.push_back(i->clone());
    for (auto &i : else_)
        tmpElse.push_back(i->clone());

    CaseSeqStmt::CaseStmtAlternative *altTrue, *altFalse;
    altTrue = new CaseSeqStmt::CaseStmtAlternative(tmpTrue, &tmpIf);
    altFalse = new CaseSeqStmt::CaseStmtAlternative(tmpFalse, &tmpElse);
    list<CaseSeqStmt::CaseStmtAlternative *> alternatives = {altTrue, altFalse};

    return new CaseSeqStmt(cond->clone(), &alternatives);
}

CaseSeqStmt *BranchesToCases::transformIfElse(const IfSequential *ifs){
    if (!containsIfSequential(ifs->if_) &&
        !containsIfSequential(ifs->else_)) {

        return makeCaseSeq(ifs->cond_, ifs->if_, ifs->else_);

    } else if (!containsIfSequential(ifs->if_) &&
               containsIfSequential(ifs->else_)) {
        list<SequentialStmt *> tmp;
        // transform all ifs to cases in else_
        for (auto &i : ifs->else_){
            Match(i){
                Case(C<IfSequential>()){
                    tmp.push_back(
                        transformIfElse(
                            dynamic_cast<IfSequential *>(i)));

                    break;
                }
                Otherwise(){
                    tmp.push_back(i);
                    break;
                }
            } EndMatch;
        }

        return makeCaseSeq(ifs->cond_, ifs->if_, tmp);

    } else if (containsIfSequential(ifs->if_) &&
               !containsIfSequential(ifs->else_)) {
        list<SequentialStmt *> tmp;
        // transform all ifs to cases in else_
        for (auto &i : ifs->if_){
            Match(i){
                Case(C<IfSequential>()){
                    tmp.push_back(
                        transformIfElse(
                            dynamic_cast<IfSequential *>(i)));

                    break;
                }
                Otherwise(){
                    tmp.push_back(i);
                    break;
                }
            } EndMatch;
        }

        return makeCaseSeq(ifs->cond_, tmp, ifs->else_);

    } else {
        list<SequentialStmt *> tmpIf, tmpElse;
        // transform all ifs to cases in else_
        for (auto &i : ifs->if_){
            Match(i){
                Case(C<IfSequential>()){
                    tmpIf.push_back(
                        transformIfElse(
                            dynamic_cast<IfSequential *>(i)));

                    break;
                }
                Otherwise(){
                    tmpIf.push_back(i);
                    break;
                }
            } EndMatch;
        }
        for (auto &i : ifs->else_){
            Match(i){
                Case(C<IfSequential>()){
                    tmpElse.push_back(
                        transformIfElse(
                            dynamic_cast<IfSequential *>(i)));

                    break;
                }
                Otherwise(){
                    tmpElse.push_back(i);
                    break;
                }
            } EndMatch;
        }

        return makeCaseSeq(ifs->cond_, tmpIf, tmpElse);
    }

    return 0;
}

int BranchesToCases::operator()(AstNode *n){
    ProcessStatement *proc = dynamic_cast<ProcessStatement*>(n);
    list<SequentialStmt *> newStmtList;

    for (auto &i : proc->statements_){
        Match(i){
            Case(C<IfSequential>()){
                // check that there are no elsif clauses
                if (dynamic_cast<IfSequential*>(i)->elsif_.size() != 0){
                    cout << "error in BranchesToCases Functor!" << endl;
                    return 1;
                }

                newStmtList.push_back(transformIfElse(
                        dynamic_cast<IfSequential*>(i)));

                delete i;
            }
            Otherwise(){
                newStmtList.push_back(i);
            }
        } EndMatch;
    }

    proc->statements_ = newStmtList;

    return 0;
}
