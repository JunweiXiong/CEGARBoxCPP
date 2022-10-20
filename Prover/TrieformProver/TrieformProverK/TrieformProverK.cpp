#include "TrieformProverK.h"

shared_ptr<Trieform>
TrieformFactory::makeTrieK(const shared_ptr<Formula> &formula,
                           shared_ptr<Trieform> trieParent) {
  shared_ptr<Trieform> trie = shared_ptr<Trieform>(new TrieformProverK());
  trie->initialise(formula, trieParent);
  return trie;
}
shared_ptr<Trieform>
TrieformFactory::makeTrieK(const shared_ptr<Formula> &formula,
                           const vector<int> &newModality,
                           shared_ptr<Trieform> trieParent) {
  shared_ptr<Trieform> trie = shared_ptr<Trieform>(new TrieformProverK());
  trie->initialise(formula, newModality, trieParent);
  return trie;
}
shared_ptr<Trieform>
TrieformFactory::makeTrieK(const vector<int> &newModality,
                           shared_ptr<Trieform> trieParent) {
  shared_ptr<Trieform> trie = shared_ptr<Trieform>(new TrieformProverK());
  trie->initialise(newModality, trieParent);
  return trie;
}

TrieformProverK::TrieformProverK() {}
TrieformProverK::~TrieformProverK() {}

shared_ptr<Trieform>
TrieformProverK::create(const shared_ptr<Formula> &formula) {
  return TrieformFactory::makeTrieK(formula, shared_from_this());
}
shared_ptr<Trieform> TrieformProverK::create(const shared_ptr<Formula> &formula,
                                             const vector<int> &newModality) {
  return TrieformFactory::makeTrieK(formula, newModality, shared_from_this());
}
shared_ptr<Trieform> TrieformProverK::create(const vector<int> &newModality) {
  shared_ptr<Trieform> test = shared_from_this();
  return TrieformFactory::makeTrieK(newModality, shared_from_this());
}

shared_ptr<Bitset>
TrieformProverK::convertAssumptionsToBitset(literal_set literals) {

  shared_ptr<Bitset> bitset =
      shared_ptr<Bitset>(new Bitset(2 * assumptionsSize));
  for (Literal literal : literals) {
    bitset->set(2 * idMap[literal.getName()] + literal.getPolarity());
  }
  return bitset;
}

void TrieformProverK::updateSolutionMemo(const shared_ptr<Bitset> &assumptions,
                                         Solution solution) {
  if (solution.satisfiable) {
    localMemo.insertSat(assumptions);
  } else {
    localMemo.insertUnsat(assumptions, solution.conflict);
  }
}

void TrieformProverK::preprocess() {}

void TrieformProverK::prepareSAT(name_set extra) {
  for (string name : extra) {
    idMap[name] = assumptionsSize++;
  }
  modal_names_map modalExtras = prover->prepareSAT(clauses, extra);
  for (auto modalSubtrie : subtrieMap) {
    modalSubtrie.second->prepareSAT(modalExtras[modalSubtrie.first]);
  }
}





Solution TrieformProverK::prove(literal_set assumptions) {
  stack<shared_ptr<TrieformProverK>> trie_stack;
  stack<literal_set> assumption_stack;
  stack<Solution> solution_stack;
  stack<modal_literal_map> triggeredDiamonds_stack;
  stack<modal_literal_map> triggeredBoxes_stack;
  stack<int> parent_index_stack;
  stack<Literal> diamond_type_stack;
  stack<int> modality_stack;

 
  modal_literal_map fake_map;
  Literal not_subset_diamond("not_subset_diamond",true);
  Solution fake_solution;

  shared_ptr<TrieformProverK> itself = dynamic_pointer_cast<TrieformProverK>(shared_from_this());
  trie_stack.push((itself));
  assumption_stack.push(assumptions);
  solution_stack.push(fake_solution);
  triggeredDiamonds_stack.push(fake_map);
  triggeredBoxes_stack.push(fake_map);
  parent_index_stack.push(0);
  diamond_type_stack.push(not_subset_diamond);
  modality_stack.push(1);


  // test mode
  bool log = false;

  while (!trie_stack.empty()){
    // if (log) {cout << trie_stack.size();};

    // extract the node from frontier
    shared_ptr<TrieformProverK> current_trie = trie_stack.top();
    literal_set current_assumption = assumption_stack.top();
    int parent_index = parent_index_stack.top();
    Literal diamond_type = diamond_type_stack.top();
    int current_modality = modality_stack.top();
    int current_index = static_cast<int>(trie_stack.size());


    // local rule
    shared_ptr<Bitset> assumptionsBitset =
      current_trie->convertAssumptionsToBitset(current_assumption);

    // print out assumption   
    cout << "----------------------------" << endl;

    cout << "total assumption start" << endl;
    for (auto x : trie_stack.top()->idMap){
      cout << x.first << x.second << endl;
    }
    cout << "total assumption end" << endl;
    
    cout << "assumption size: " << trie_stack.top()->assumptionsSize << endl;
    cout << assumption_stack.size() << " assumption start" << endl;
    for (auto i : current_assumption){
      cout << i.getName() << i.getPolarity() << " ";
    }
    cout << endl;
    cout << assumption_stack.size() << " assumption end" << endl;

    
    // p1 & p2 & []p3 & <>p4 & <>p5 & <>~p6
    // if (trie_stack.size() >1 ){
    //   cout << "memory test p3" << endl;
    //   literal_set test_assumption;
    //   test_assumption.insert(Literal("p3",1));
    //   shared_ptr<Bitset> test_assumptionsBitset =
    //     current_trie->convertAssumptionsToBitset(test_assumption);
    //   LocalSolutionMemoResult test_memoResult = current_trie->localMemo.getFromMemo(test_assumptionsBitset);
    //   if (test_memoResult.inSatMemo){
    //     cout << "in memory" << endl;
    //   }else{
    //     cout << "not in memory" << endl;
    //   }
    //   cout << "memory test p4" << endl;
    //   test_assumption.clear();
    //   test_assumption.insert(Literal("p4",1));
    //   test_assumptionsBitset =
    //     current_trie->convertAssumptionsToBitset(test_assumption);
    //   test_memoResult = current_trie->localMemo.getFromMemo(test_assumptionsBitset);
    //   if (test_memoResult.inSatMemo){
    //     cout << "in memory" << endl;
    //   }else{
    //     cout << "not in memory" << endl;
    //   }
    //   cout << "memory test p5" << endl;
    //   test_assumption.clear();
    //   test_assumption.insert(Literal("p5",1));
    //   test_assumptionsBitset =
    //     current_trie->convertAssumptionsToBitset(test_assumption);
    //   test_memoResult = current_trie->localMemo.getFromMemo(test_assumptionsBitset);
    //   if (test_memoResult.inSatMemo){
    //     cout << "in memory" << endl;
    //   }else{
    //     cout << "not in memory" << endl;
    //   }
    //   cout << "memory test p6" << endl;
    //   test_assumption.clear();
    //   test_assumption.insert(Literal("p6",1));
    //   test_assumptionsBitset =
    //     current_trie->convertAssumptionsToBitset(test_assumption);
    //   test_memoResult = current_trie->localMemo.getFromMemo(test_assumptionsBitset);
    //   if (test_memoResult.inSatMemo){
    //     cout << "in memory" << endl;
    //   }else{
    //     cout << "not in memory" << endl;
    //   }
    //   cout << "memory test p60" << endl;
    //   test_assumption.clear();
    //   test_assumption.insert(Literal("p6",0));
    //   test_assumptionsBitset =
    //     current_trie->convertAssumptionsToBitset(test_assumption);
    //   test_memoResult = current_trie->localMemo.getFromMemo(test_assumptionsBitset);
    //   if (test_memoResult.inSatMemo){
    //     cout << "in memory" << endl;
    //   }else{
    //     cout << "not in memory" << endl;
    //   }
    //   cout << "memory test p5 p4" << endl;
    //   test_assumption.clear();
    //   test_assumption.insert(Literal("p5",1));
    //   test_assumption.insert(Literal("p4",1));
    //   test_assumptionsBitset =
    //     current_trie->convertAssumptionsToBitset(test_assumption);
    //   test_memoResult = current_trie->localMemo.getFromMemo(test_assumptionsBitset);
    //   if (test_memoResult.inSatMemo){
    //     cout << "in memory" << endl;
    //   }else{
    //     cout << "not in memory" << endl;
    //   }
    //   cout << "memory test p5 p60" << endl;
    //   test_assumption.clear();
    //   test_assumption.insert(Literal("p5",1));
    //   test_assumption.insert(Literal("p6",0));
    //   test_assumptionsBitset =
    //     current_trie->convertAssumptionsToBitset(test_assumption);
    //   test_memoResult = current_trie->localMemo.getFromMemo(test_assumptionsBitset);
    //   if (test_memoResult.inSatMemo){
    //     cout << "in memory" << endl;
    //   }else{
    //     cout << "not in memory" << endl;
    //   }
    // }
    
    LocalSolutionMemoResult memoResult = current_trie->localMemo.getFromMemo(assumptionsBitset);

    // LocalSolutionMemoResult memoResult = {false,{false,literal_set()}};

    Solution current_solution;
    if (memoResult.inSatMemo) {
      current_solution = memoResult.result;
      solution_stack.top() = current_solution;
    }else{
      current_solution = current_trie->prover->solve(current_assumption);
      solution_stack.top() = current_solution;
    }
    if (!current_solution.satisfiable){
      if (!memoResult.inSatMemo){
        current_trie->updateSolutionMemo(assumptionsBitset, current_solution);
      }
      // remove other children, back to the parent
      while (static_cast<int>(trie_stack.size())>parent_index){
        trie_stack.pop();
        assumption_stack.pop();
        solution_stack.pop();
        triggeredDiamonds_stack.pop();
        triggeredBoxes_stack.pop();
        parent_index_stack.pop();
        diamond_type_stack.pop();
        modality_stack.pop();
      }
      // make sure it's not the root trie
      if (trie_stack.size()==0){
        return current_solution;
      }
      // update the parent
      // if (log) {cout << " unsatisfiable, update parent" << endl;};
      if (diamond_type.getName()=="not_subset_diamond"){
        vector<literal_set> badImplications = trie_stack.top()->prover->getNotProblemBoxClauses(
          current_modality, current_solution.conflict);
        badImplications.push_back(
          trie_stack.top()->prover->getNotAllDiamondLeft(current_modality));
        for (literal_set learnClause : trie_stack.top()->generateClauses(badImplications)) {
          trie_stack.top()->prover->addClause(learnClause);
        }
      }else{
        vector<literal_set> badImplications = trie_stack.top()->prover->getNotProblemBoxClauses(
            current_modality, current_solution.conflict);
        if (current_solution.conflict.find(diamond_type) !=
            current_solution.conflict.end()) {
          // The diamond clause, either on its own or together with box clauses,
          // caused a conflict. We must add diamond implies OR NOT problem
          // box clauses.
          trie_stack.top()->prover->updateLastFail(diamond_type);
          badImplications.push_back(
              trie_stack.top()->prover->getNotDiamondLeft(current_modality, diamond_type));
          for (literal_set learnClause : trie_stack.top()->generateClauses(badImplications)) {
            trie_stack.top()->prover->addClause(learnClause);
          }
        }else{
          badImplications.push_back(
              trie_stack.top()->prover->getNotAllDiamondLeft(current_modality));
          for (literal_set learnClause : trie_stack.top()->generateClauses(badImplications)) {
            trie_stack.top()->prover->addClause(learnClause);
          }
        }

      }

    }else{
      if (!memoResult.inSatMemo){
        // determine expand the node or backtrack
        current_trie->prover->calculateTriggeredDiamondsClauses();
        modal_literal_map triggeredDiamonds = current_trie->prover->getTriggeredDiamondClauses();
        triggeredDiamonds_stack.top() = triggeredDiamonds;
        if (triggeredDiamonds.size() == 0) {
          // if (log) {cout << " satisfiable, backtrack (no trigger)" << endl;};
          current_trie->updateSolutionMemo(assumptionsBitset, current_solution);
  
          // make sure it's not the root trie
          if (trie_stack.size()==0){

            return current_solution;
          }
          // backtrack
   
          while(static_cast<int>(trie_stack.size())==parent_index+1){
            // check if is the root trie
            if (static_cast<int>(trie_stack.size())==1){
              return solution_stack.top();
            }

            trie_stack.pop();
            assumption_stack.pop();
            solution_stack.pop();
            triggeredDiamonds_stack.pop();
            triggeredBoxes_stack.pop();
            parent_index_stack.pop();
            diamond_type_stack.pop();
            modality_stack.pop();
            //  update parent local memory
            shared_ptr<Bitset> ancestor_assumptionsBitset =
                trie_stack.top()->convertAssumptionsToBitset(assumption_stack.top());
            trie_stack.top()->updateSolutionMemo(ancestor_assumptionsBitset,solution_stack.top());

            parent_index = parent_index_stack.top();
          }
          shared_ptr<Bitset> ancestor_assumptionsBitset =
                trie_stack.top()->convertAssumptionsToBitset(assumption_stack.top());
          trie_stack.top()->updateSolutionMemo(ancestor_assumptionsBitset,solution_stack.top());
        
          trie_stack.pop();
          assumption_stack.pop();
          solution_stack.pop();
          triggeredDiamonds_stack.pop();
          triggeredBoxes_stack.pop();
          parent_index_stack.pop();
          diamond_type_stack.pop();
          modality_stack.pop();
        }else{
          
          current_trie->prover->calculateTriggeredBoxClauses();
          modal_literal_map triggeredBoxes = current_trie->prover->getTriggeredBoxClauses();
          triggeredBoxes_stack.top() = triggeredBoxes;
          // expand
          for (auto modalitySubtrie : current_trie->subtrieMap) {
            // Handle each modality
            if (triggeredDiamonds[modalitySubtrie.first].size() == 0) {
              // If there are no triggered diamonds of a certain modality we can skip it
              continue;
            }
            
            if (isSubsetOf(triggeredDiamonds[modalitySubtrie.first],
                          triggeredBoxes[modalitySubtrie.first])) {
              // if (log) {cout << " satisfiable, expand_a 1 ";};
              // The fired diamonds are a subset of the boxes - we thus can create one
              // world.
              literal_set childAssumptions = literal_set(triggeredBoxes[modalitySubtrie.first]);

              shared_ptr<TrieformProverK> tk = dynamic_pointer_cast<TrieformProverK>(modalitySubtrie.second);
              trie_stack.push(tk);
              assumption_stack.push(childAssumptions);
              solution_stack.push(fake_solution);
              triggeredDiamonds_stack.push(fake_map);
              triggeredBoxes_stack.push(fake_map);
              parent_index_stack.push(current_index);
              diamond_type_stack.push(not_subset_diamond);
              modality_stack.push(modalitySubtrie.first);
            } else {
              // The fired diamonds are not a subset of the fired boxes, we need to
              // create one world for each diamond clause
              diamond_queue diamondPriority =
                  current_trie->prover->getPrioritisedTriggeredDiamonds(modalitySubtrie.first);
              // if (log) {cout << " satisfiable, expand_b " << diamondPriority.size();};

              stack<Literal> new_diamondPriority;
              while (!diamondPriority.empty()) {
                new_diamondPriority.push(diamondPriority.top().literal);
                diamondPriority.pop();
              }

              while (!new_diamondPriority.empty()) {
                // Create a world for each diamond
                Literal diamond = new_diamondPriority.top();
                new_diamondPriority.pop();

                literal_set childAssumptions = literal_set(triggeredBoxes[modalitySubtrie.first]);
                childAssumptions.insert(diamond);

                shared_ptr<TrieformProverK> tk = dynamic_pointer_cast<TrieformProverK>(modalitySubtrie.second);
                trie_stack.push(tk);
                assumption_stack.push(childAssumptions);
                solution_stack.push(fake_solution);
                triggeredDiamonds_stack.push(fake_map);
                triggeredBoxes_stack.push(fake_map);
                parent_index_stack.push(current_index);
                diamond_type_stack.push(diamond);
                modality_stack.push(modalitySubtrie.first);
              }
            }
            // if (log) {cout << " " << endl;};
          }
        }
      }else{
        // if (log) {cout << " satisfiable, backtrack (in memory)" << endl;};
        // backtrack
        while(static_cast<int>(trie_stack.size())==parent_index+1){
          // check if is the root trie
          if (static_cast<int>(trie_stack.size())==1){
            return solution_stack.top();
          }
          trie_stack.pop();
          assumption_stack.pop();
          solution_stack.pop();
          triggeredDiamonds_stack.pop();
          triggeredBoxes_stack.pop();
          parent_index_stack.pop();
          diamond_type_stack.pop();
          modality_stack.pop();
          //  update parent local memory
          shared_ptr<Bitset> ancestor_assumptionsBitset =
                trie_stack.top()->convertAssumptionsToBitset(assumption_stack.top());
          trie_stack.top()->updateSolutionMemo(ancestor_assumptionsBitset,solution_stack.top());

          parent_index = parent_index_stack.top();
        }
        shared_ptr<Bitset> ancestor_assumptionsBitset =
              trie_stack.top()->convertAssumptionsToBitset(assumption_stack.top());
        trie_stack.top()->updateSolutionMemo(ancestor_assumptionsBitset,solution_stack.top());

        trie_stack.pop();
        assumption_stack.pop();
        solution_stack.pop();
        triggeredDiamonds_stack.pop();
        triggeredBoxes_stack.pop();
        parent_index_stack.pop();
        diamond_type_stack.pop();
        modality_stack.pop();
      }
    }
  }
  // error, should not reach here
  cout << "error end" << endl;
  return prover->solve(assumptions);
}








