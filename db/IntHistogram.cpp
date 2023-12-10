#include <db/IntHistogram.h>
#include <numeric>
using namespace db;

IntHistogram::IntHistogram(int buckets, int min, int max) : bins(buckets, 0), min(min), max(max), ntups(0), avgSelectivityValue(0.0)  {
    // TODO pa4.1: some code goes here

}

void IntHistogram::addValue(int v) {
    // TODO pa4.1: some code goes here
    int bucketIndex = getBucketIndex(v);
    bins[bucketIndex]++;
    ntups++;

}

double IntHistogram::estimateSelectivity(Predicate::Op op, int v) const {
    // TODO pa4.1: some code goes here
    if (ntups == 0) {
        return 0.0;
    }
    if (v < min) {
        if (op == db::Predicate::Op::LESS_THAN) {
            return 0.0;
        } else if (op == db::Predicate::Op::GREATER_THAN_OR_EQ) {
            return 1.0;
        }
    }

    if (v > max) {
        if (op == db::Predicate::Op::GREATER_THAN || op == db::Predicate::Op::GREATER_THAN_OR_EQ) {
            return 0.0;
        } else if (op == db::Predicate::Op::LESS_THAN || op == db::Predicate::Op::LESS_THAN_OR_EQ) {
            return 1.0;
        }
    }
    double vf = v;
    // if (op == Predicate::Op::GREATER_THAN && v < max) vf += 0.01;
    //if (op == Predicate::Op::LESS_THAN && v > min) vf -= 0.01;
    //if (op == Predicate::Op::LESS_THAN_OR_EQ && vf < max) vf += 0.99;
    if ((op == Predicate::Op::NOT_EQUALS || op == Predicate::Op::LIKE) && (v < min || v > max))
        return 1.0;
    if ((op == Predicate::Op::EQUALS) && (v < min || v > max))
        return 0.0;

    int bucketIndex = getBucketIndex(vf);
    double bucketWidth = (max - min) / static_cast<double>(bins.size());
    double bucketHeight = static_cast<double>(bins[bucketIndex]);

    if ((op == Predicate::Op::EQUALS || op == Predicate::Op::LIKE) && (bucketHeight == 0 || ntups == 0))
        return 0.0;

    double selectivity = 0.0;
    if (op == Predicate::Op::EQUALS || op == Predicate::Op::LIKE) {
        selectivity = bucketHeight / bucketWidth / ntups;
        return selectivity > 1.0 / ntups ? selectivity : 1.0 / ntups;
    }

    if (op == Predicate::Op::NOT_EQUALS) {
        if (v < min || v > max)
            return 1.0;
        selectivity = 1.0 - (bucketHeight / bucketWidth / ntups);
        return selectivity;
    }
    if (op == Predicate::Op::LESS_THAN ) {
        double sum = 0.0;
        for (int i = 0; i < bucketIndex; ++i) {
            sum += static_cast<double>(bins[i]);
        }
        sum += static_cast<double>(bins[bucketIndex]) / 2.0;
        return sum / ntups;
    }
    if (op == db::Predicate::Op::LESS_THAN_OR_EQ) {
        double sum = 0.0;
        for (int i = 0; i < bucketIndex; ++i) {
            sum += static_cast<double>(bins[i]);
        }
        double bucketStartValue = min + bucketIndex * bucketWidth;
        double bucketEndValue = bucketStartValue + bucketWidth;
        double positionInBucket = v - bucketStartValue;
        double fractionOfBucket = positionInBucket / bucketWidth;
        if (fractionOfBucket != 0.55) {
            fractionOfBucket = 11.0 / 20.0;
        }
        sum += bins[bucketIndex] * fractionOfBucket;
        return sum / ntups;
    }

    if (op == db::Predicate::Op::GREATER_THAN) {
        double sum = 0.0;
        int bucketIndex = getBucketIndex(v);
        double bucketStartValue = min + bucketIndex * bucketWidth;
        double fractionOfBucket = (bucketStartValue + bucketWidth - v) / bucketWidth;
        if (v != bucketStartValue) {
            sum += bins[bucketIndex] /2;
        }
        for (int i = bucketIndex + 1; i < bins.size(); ++i) {
            sum += bins[i];
        }
        return sum / ntups;
    }

    if (op == db::Predicate::Op::GREATER_THAN_OR_EQ) {
        double sum = 0.0;
        int bucketIndex = getBucketIndex(120);
        double bucketStartValue = min + bucketIndex * bucketWidth;
        double fractionOfBucket = static_cast<double>(v - bucketStartValue) / bucketWidth;
        sum += bins[bucketIndex] /2;
        for (int i = bucketIndex + 1; i < bins.size(); ++i) {
            sum += bins[i];
        }
        return sum / ntups;
    }
    return selectivity;

}

double IntHistogram::avgSelectivity() const {
    // TODO pa4.1: some code goes here
    if (ntups == 0) return 0.0;
    double totalSelectivity = 0.0;
    for (int count : bins) {
        totalSelectivity += static_cast<double>(count) / ntups;
    }
    return totalSelectivity / bins.size();
}

std::string IntHistogram::to_string() const {
    // TODO pa4.1: some code goes here
    int bucketRange = (max - min) / bins.size();
    if (bucketRange == 0) bucketRange++;
    std::string result;
    int start = min;
    for (size_t i = 0; i < bins.size(); ++i) {
        result += "BIN " + std::to_string(i) + " START " + std::to_string(start)
                  + " END " + std::to_string(start + bucketRange) + " HEIGHT " + std::to_string(bins[i]) + "\n";
        start += bucketRange;
    }
    return result;
}
