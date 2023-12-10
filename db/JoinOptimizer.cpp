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
        if (t1pkey) {
            return card2;
        } else if (t2pkey) {
            return card1;
        } else {
            return (card1 + card2) / 2;
        }
    } else {
        return static_cast<int>(0.3 * card1 * card2);
    }
}

std::vector<LogicalJoinNode> JoinOptimizer::orderJoins(std::unordered_map<std::string, TableStats> stats,
                                                       std::unordered_map<std::string, double> filterSelectivities) {
    // TODO pa4.3: some code goes here
    PlanCache pc;

    if (joins.empty()) {
        return std::vector<LogicalJoinNode>();
    }

    // Iterate over all subset sizes
    for (size_t subsetSize = 1; subsetSize <= joins.size(); ++subsetSize) {
        auto subsets = enumerateSubsets(joins, subsetSize);


        for (const auto &subset : subsets) {
            CostCard *bestPlan = nullptr;
            double bestCost = DBL_MAX;

            for (const auto &joinNode : subset) {
                auto newSubset = subset;
                newSubset.erase(joinNode);

                auto cachedPlan = pc.get(newSubset);
                if (cachedPlan) {
                    auto newPlan = computeCostAndCardOfSubplan(
                            stats, filterSelectivities, joinNode, newSubset,
                            bestCost, pc);

                    if (newPlan && newPlan->cost < bestCost) {
                        bestCost = newPlan->cost;
                        bestPlan = newPlan;
                    }
                }
            }

            if (bestPlan) {
                std::unordered_set<LogicalJoinNode> modifiableSubset = subset;
                pc.addPlan(modifiableSubset, bestPlan);
            }
        }

    }

    auto fullSet = std::unordered_set<LogicalJoinNode>(joins.begin(), joins.end());
    auto finalPlan = pc.get(fullSet);

    if (finalPlan) {
        return finalPlan->plan;
    } else {
        return std::vector<LogicalJoinNode>();
    }
}

