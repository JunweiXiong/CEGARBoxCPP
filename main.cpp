#include "Bitset/Bitset.h"
#include "Clausifier/Trieform/Trieform.h"
#include "Clausifier/TrieformFactory/TrieformFactory.h"
#include "Formula/And/And.h"
#include "Formula/Atom/Atom.h"
#include "Formula/Box/Box.h"
#include "Formula/Diamond/Diamond.h"
#include "Formula/FEnum/FEnum.h"
#include "Formula/False/False.h"
#include "Formula/Formula/Formula.h"
#include "Formula/Not/Not.h"
#include "Formula/Or/Or.h"
#include "Formula/True/True.h"
#include "ParseFormula/ParseFormula.h"
#include "ParseFormulaNew/ParseFormulaNew.h"
#include <argp.h>
#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <minisat/core/Solver.h>
#include <string>
#include <unordered_set>

using namespace std;

const char *argp_program_version = "CEGARBox 0.1.1";
const char *argp_program_bug_address = "u6757090@anu.edu.au";
static char doc[] = "An efficient theorem prover for modal logic.";
static char args_doc[] = "";
static struct argp_option options[] = {
    {"file", 'f', "FILE", 0, "File containing input formula."},
    {"reflexive", 't', 0, 0, "Enables reflexivity."},
    {"symmetric", 'b', 0, 0, "Enables symmetry."},
    {"transitive", '4', 0, 0, "Enables transitivity."},
    {"serial", 'd', 0, 0, "Enables seriality."},
    {"euclidean", 'e', 0, 0, "Enables transitivity."},
    {"tense", 'n', 0, 0, "Enables Tense Logic."},
    {"valid", 'a', 0, 0, "Prove validity."},
    {"verbose", 'v', 0, 0, "Verbosity."},
    {0, 0, 0, 0, 0, 0}};

struct arguments_struct {
  string filename = "file.p";
  SolverConstraints settings;
  bool tense = false;
  bool valid = false;
  bool verbose = false;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  arguments_struct *arguments = static_cast<arguments_struct *>(state->input);
  switch (key) {
  case 'f': {
    arguments->filename = arg;
  } break;
  case 't':
    arguments->settings.reflexive = true;
    break;
  case 'b':
    arguments->settings.symmetric = true;
    break;
  case '4':
    arguments->settings.transitive = true;
    break;
  case 'd':
    arguments->settings.serial = true;
    break;
  case 'e':
    arguments->settings.euclidean = true;
    break;
  case 'n':
    arguments->tense = true;
    break;
  case 'a':
    arguments->valid = true;
    break;
  case 'v':
    arguments->verbose = true;
    break;
  case ARGP_KEY_ARG:
    return 0;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

void solve(arguments_struct &args);

int main(int argc, char *argv[]) {
  arguments_struct arguments;

  argp_parse(&argp, argc, argv, 0, 0, &arguments);
  solve(arguments);
}

void solve(arguments_struct &args) {
#if DEBUG_TIME
  auto start = chrono::steady_clock::now();
#endif
  auto start = chrono::steady_clock::now();
  if (args.verbose) {
    cout << "Begin" << endl;
  }
  auto read = chrono::steady_clock::now();
#if DEBUG_PROGRESS
  cout << "Begin" << endl;
#endif

#if DEBUG_TIME
  auto read = chrono::steady_clock::now();
#endif

  // cout << "start parsing" << endl;
  shared_ptr<Formula> formula = ParseFormula(&args.filename).parseFormula();
  // string other = "a.p";
  // shared_ptr<Formula> correct = ParseFormula(&other).parseFormula();
  // cout << "Wrong" << formula->toString() << endl;
  // cout << "Right" << correct->toString() << endl;
  // cout << (*formula == *correct) << endl;

  
  if (args.valid) {
    // cout << "start not formula" << endl;
    formula = Not::create(formula);
  }

#if DEBUG_TIME
  auto parse = chrono::steady_clock::now();
#endif
#if DEBUG_PROGRESS
  cout << "Parsed: " << formula->toString() << endl;
#endif
  auto parse = chrono::steady_clock::now();
  if (args.verbose) {
    cout << "Parsed: " << formula->toString() << endl;
  }

  if (args.settings.reflexive && args.settings.transitive){
    formula = formula->s4reduction();
    if (args.verbose) {
    cout << "S4 reduction: " << formula->toString() << endl;
    }
    formula = formula->negatedNormalForm();
    formula = formula->simplify();
    formula = formula->modalFlatten();
    shared_ptr<Trieform> trie = TrieformFactory::makeTrie(formula, args.settings);
    trie->reduceClauses();
    trie->preprocess();
    trie->removeTrueAndFalse();
    trie -> prepareSAT();
    bool satisfiable = trie->isSatisfiable();
  if (args.valid) {
    cout << (satisfiable ? "Invalid" : "Valid") << endl;
  } else {
    cout << (satisfiable ? "Satisfiable" : "Unsatisfiable") << endl;
  }
    return;
  }



  // cout << "start nnf" << endl;
  formula = formula->negatedNormalForm();
  // correct = correct->negatedNormalForm();

  // cout << (*formula == *correct) << endl;

#if DEBUG_TIME
  auto nnf = chrono::steady_clock::now();
#endif
#if DEBUG_PROGRESS
  cout << "Negated normal form: " << formula->toString() << endl;
#endif

  auto nnf = chrono::steady_clock::now();
  if (args.verbose) {
    cout << "Negated normal form: " << formula->toString() << endl;
  }
  // cout << "start simplify" << endl;
  formula = formula->simplify();
  // correct = correct->simplify();

  // cout << (*formula == *correct) << endl;

#if DEBUG_TIME
  auto simplify = chrono::steady_clock::now();
#endif
#if DEBUG_PROGRESS
  cout << "Simplified: " << formula->toString() << endl;
#endif

  auto simplify = chrono::steady_clock::now();
  if (args.verbose) {
    cout << "Simplified: " << formula->toString() << endl;
  }
  // cout << "start modalflatten" << endl;
  formula = formula->modalFlatten();
  // correct = correct->modalFlatten();

  // cout << (*formula == *correct) << endl;

#if DEBUG_TIME
  auto flatten = chrono::steady_clock::now();
#endif
#if DEBUG_PROGRESS
  cout << "Flattenned: " << formula->toString() << endl;
#endif

  auto flatten = chrono::steady_clock::now();
  if (args.verbose) {
    cout << "Flattenned: " << formula->toString() << endl;
  }
  // cout << "start maketrie" << endl;
  shared_ptr<Trieform> trie = TrieformFactory::makeTrie(formula, args.settings);
  // shared_ptr<Trieform> otherTrie =
  //     TrieformFactory::makeTrie(correct, args.settings);
  if (args.verbose) {
    cout << "Constructed trie" << endl;
    cout << "Initial trie:" << endl << trie->toString() << endl;
    cout << "Normal cache:" << endl << trie->getCache().toString() << endl;
  }
  auto construct = chrono::steady_clock::now();
#if DEBUG_PROGRESS
  cout << "Constructed trie" << endl;
#endif
#if DEBUG_INITIAL_TRIE
  cout << "Initial trie:" << endl << trie->toString() << endl;
#endif
#if DEBUG_NORMAL_CACHE
  cout << "Normal cache:" << endl << trie->getCache().toString() << endl;
#endif
#if DEBUG_TIME
  auto construct = chrono::steady_clock::now();
#endif
  // cout << "Initial trie:" << endl << trie->toString() << endl;
  // cout << "Correct trie:" << endl << otherTrie->toString() << endl;
  // cout << "start reduceclauses" << endl;
  trie->reduceClauses();
  // otherTrie->reduceClauses();

#if DEBUG_TIME
  auto reduce = chrono::steady_clock::now();
#endif
#if DEBUG_PROGRESS
  cout << "Reduced trie" << endl;
#endif
#if DEBUG_REDUCED_TRIE
  cout << "Reduced trie:" << endl << trie->toString() << endl;
#endif
#if DEBUG_REDUCED_CACHE
  cout << "Reduced cache:" << endl << trie->getCache().toString() << endl;
#endif

  auto reduce = chrono::steady_clock::now();
  if (args.verbose) {
    cout << "Reduced trie" << endl;
    cout << "Reduced trie:" << endl << trie->toString() << endl;
    cout << "Reduced cache:" << endl << trie->getCache().toString() << endl;
  }
  
  if (args.tense) {
    trie->preprocessTense();
  }
  // cout << "start preprocess" << endl;
  trie->preprocess();
  // cout << "Processed trie:" << endl << trie->toString() << endl;
  // otherTrie->preprocess();

#if DEBUG_PROGRESS
  cout << "Preprocessed trie" << endl;
#endif
#if DEBUG_PROCESSED_TRIE
  cout << "Processed trie:" << endl << trie->toString() << endl;
#endif

  if (args.verbose) {
    cout << "Preprocessed trie" << endl;
    cout << "Processed trie:" << endl << trie->toString() << endl;
  }
  // cout << "start removeTrueFalse" << endl;
  trie->removeTrueAndFalse();
  // otherTrie->removeTrueAndFalse();
  // cout << "start preparesat" << endl;
  trie->prepareSAT();
  // otherTrie->prepareSAT();

#if DEBUG_TIME
  auto prepare = chrono::steady_clock::now();
#endif
#if DEBUG_PROGRESS
  cout << "Prepared SAT" << endl;
#endif

  auto prepare = chrono::steady_clock::now();
  if (args.verbose) {
    cout << "Prepared SAT" << endl;
  }


  // cout << "start issatisfiable" << endl;
  bool satisfiable = trie->isSatisfiable();
  if (args.valid) {
    cout << (satisfiable ? "Invalid" : "Valid") << endl;
  } else {
    cout << (satisfiable ? "Satisfiable" : "Unsatisfiable") << endl;
  }
  // satisfiable = otherTrie->isSatisfiable();
  // if (args.valid) {
  //   cout << (satisfiable ? "Invalid" : "Valid") << endl;
  // } else {
  //   cout << (satisfiable ? "Satisfiable" : "Unsatisfiable") << endl;
  // }

#if DEBUG_TIME
  auto solve = chrono::steady_clock::now();
#endif
#if DEBUG_PROGESS
  cout << "Solved" << endl;
#endif

#if DEBUG_TIME
  auto readTime = read - start;
  auto parseTime = parse - start;
  auto nnfTime = nnf - start;
  auto simplifyTime = simplify - start;
  auto flattenTime = flatten - start;
  auto constructTime = construct - start;
  auto reduceTime = reduce - start;
  auto prepareTime = prepare - start;
  auto solveTime = solve - start;
  cout << "READ TIME: " << chrono::duration<double, milli>(readTime).count()
       << " ms" << endl;
  cout << "PARSE TIME: " << chrono::duration<double, milli>(parseTime).count()
       << " ms" << endl;
  cout << "NNF TIME: " << chrono::duration<double, milli>(nnfTime).count()
       << " ms" << endl;
  cout << "SIMPLIFY TIME: "
       << chrono::duration<double, milli>(simplifyTime).count() << " ms"
       << endl;
  cout << "FLATTEN TIME: "
       << chrono::duration<double, milli>(flattenTime).count() << " ms" << endl;
  cout << "CONSTRUCT TIME: "
       << chrono::duration<double, milli>(constructTime).count() << " ms"
       << endl;
  cout << "REDUCE TIME: " << chrono::duration<double, milli>(reduceTime).count()
       << " ms" << endl;
  cout << "PREPARE TIME: "
       << chrono::duration<double, milli>(prepareTime).count() << " ms" << endl;
  cout << "SOLVE TIME: " << chrono::duration<double, milli>(solveTime).count()
       << " ms" << endl;
#endif

  if (args.verbose) {
    auto solve = chrono::steady_clock::now();
    cout << "Solved" << endl;

    auto readTime = read - start;
    auto parseTime = parse - start;
    auto nnfTime = nnf - start;
    auto simplifyTime = simplify - start;
    auto flattenTime = flatten - start;
    auto constructTime = construct - start;
    auto reduceTime = reduce - start;
    auto prepareTime = prepare - start;
    auto solveTime = solve - start;
    cout << "READ TIME: " << chrono::duration<double, milli>(readTime).count()
         << " ms" << endl;
    cout << "PARSE TIME: " << chrono::duration<double, milli>(parseTime).count()
         << " ms" << endl;
    cout << "NNF TIME: " << chrono::duration<double, milli>(nnfTime).count()
         << " ms" << endl;
    cout << "SIMPLIFY TIME: "
         << chrono::duration<double, milli>(simplifyTime).count() << " ms"
         << endl;
    cout << "FLATTEN TIME: "
         << chrono::duration<double, milli>(flattenTime).count() << " ms"
         << endl;
    cout << "CONSTRUCT TIME: "
         << chrono::duration<double, milli>(constructTime).count() << " ms"
         << endl;
    cout << "REDUCE TIME: "
         << chrono::duration<double, milli>(reduceTime).count() << " ms"
         << endl;
    cout << "PREPARE TIME: "
         << chrono::duration<double, milli>(prepareTime).count() << " ms"
         << endl;
    cout << "SOLVE TIME: " << chrono::duration<double, milli>(solveTime).count()
         << " ms" << endl;
  }
}