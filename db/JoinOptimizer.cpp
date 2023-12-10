#include <db/JoinOptimizer.h>
#include <db/PlanCache.h>
#include <cfloat>

using namespace db;

double JoinOptimizer::estimateJoinCost(int card1, int card2, double cost1, double cost2) {
    // TODO pa4.2: some code goes here
    return cost1 + card1 * cost2 + card1 * card2;
}

int JoinOptimizer::estimateTableJoinCardinality(Predicate::Op joinOp,
                                                const std::string &table1Alias, const std::string &table2Alias,
                                                const std::string &field1PureName, const std::string &field2PureName,
                                                int card1, int card2, bool t1pkey, bool t2pkey,
                                                const std::unordered_map<std::string, TableStats> &stats,
                                                const std::unordered_map<std::string, int> &tableAliasToId) {
    // TODO pa4.2: some code goes here
    if (joinOp == Predicate::Op::EQUALS) {
        if (t1pkey) return card2;
        else if (t2pkey) return card1;
        else return std::max(card1, card2);
    } else {
        // For non-equality joins, you might use a heuristic like 30% of the cross-product size
        return static_cast<int>(0.3 * card1 * card2);
    }
}

std::vector<LogicalJoinNode> JoinOptimizer::orderJoins(std::unordered_map<std::string, TableStats> stats,
                                                       std::unordered_map<std::string, double> filterSelectivities) {
    // TODO pa4.3: some code goes here

    PlanCache pc;

    // Base case: If there are no joins, return an empty vector.
    if (joins.empty()) {
        return std::vector<LogicalJoinNode>();
    }

    // Iterate over all subset sizes
    for (size_t subsetSize = 1; subsetSize <= joins.size(); ++subsetSize) {
        // Enumerate all subsets of the current size
        auto subsets = enumerateSubsets(joins, subsetSize);

        for (const auto& subset : subsets) {
            double bestCost = DBL_MAX;
            CostCard* bestPlan = nullptr;

            for (const auto& joinToRemove : subset) {
                // Clone the current subset and remove one join
                auto newSubset = subset;
                newSubset.erase(joinToRemove);

                // Get the best plan for the newSubset from the PlanCache
                CostCard* plan = pc.get(newSubset);
                if (plan) {
                    // Compute the cost and cardinality of adding the removed join
                    CostCard* newPlan = computeCostAndCardOfSubplan(
                            stats, filterSelectivities, joinToRemove, newSubset,
                            bestCost, pc);

                    // Update the best plan and cost if the new plan is better
                    if (newPlan && newPlan->cost < bestCost) {
                        bestCost = newPlan->cost;
                        bestPlan = newPlan;
                    }
                }
            }

            // Update the PlanCache with the best plan for this subset
            if (bestPlan) {
                std::unordered_set<LogicalJoinNode> modifiableSubset = subset;
                pc.addPlan(modifiableSubset, bestPlan);
            }
        }
    }

    // Retrieve the best plan for the full set of joins
    std::unordered_set<LogicalJoinNode> fullSet(joins.begin(), joins.end());
    CostCard* finalPlan = pc.get(fullSet);

    // Return the join order from the final plan
    //if (finalPlan && finalPlan->order) { // Ensure 'order' is accessible
       // return *(finalPlan->order);
    //} else {
        return std::vector<LogicalJoinNode>();
    }

