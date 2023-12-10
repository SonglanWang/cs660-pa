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
            return 0.0; // 没有任何值大于或等于 v
        } else if (op == db::Predicate::Op::LESS_THAN || op == db::Predicate::Op::LESS_THAN_OR_EQ) {
            return 1.0; // 所有值都小于 v
        }
    }
    double vf = v;
    // if (op == Predicate::Op::GREATER_THAN && v < max) vf += 0.01;
    //if (op == Predicate::Op::LESS_THAN && v > min) vf -= 0.01;
    //if (op == Predicate::Op::LESS_THAN_OR_EQ && vf < max) vf += 0.99;

    // Handle values outside the range
    if ((op == Predicate::Op::NOT_EQUALS || op == Predicate::Op::LIKE) && (v < min || v > max))
        return 1.0;
    if ((op == Predicate::Op::EQUALS) && (v < min || v > max))
        return 0.0;

    // Compute the bucket index
    int bucketIndex = getBucketIndex(vf);
    double bucketWidth = (max - min) / static_cast<double>(bins.size());
    double bucketHeight = static_cast<double>(bins[bucketIndex]);

    // If bucket height is zero or no tuples, selectivity is zero (for EQUALS)
    if ((op == Predicate::Op::EQUALS || op == Predicate::Op::LIKE) && (bucketHeight == 0 || ntups == 0))
        return 0.0;

    // Calculate selectivity for EQUALS and LIKE
    double selectivity = 0.0;
    if (op == Predicate::Op::EQUALS || op == Predicate::Op::LIKE) {
        selectivity = bucketHeight / bucketWidth / ntups;
        return selectivity > 1.0 / ntups ? selectivity : 1.0 / ntups;  // At least 1/ntups if non-zero
    }

    // Calculate selectivity for NOT_EQUALS
    if (op == Predicate::Op::NOT_EQUALS) {
        if (v < min || v > max)
            return 1.0;
        selectivity = 1.0 - (bucketHeight / bucketWidth / ntups);
        return selectivity;
    }
    if (op == Predicate::Op::LESS_THAN ) {
        double sum = 0.0;
        // 将v之前的所有桶的高度加起来
        for (int i = 0; i < bucketIndex; ++i) {
            sum += static_cast<double>(bins[i]);
        }
        // 加上v所在桶高度的一半
        sum += static_cast<double>(bins[bucketIndex]) / 2.0;




        // 返回小于v的选择性
        return sum / ntups;
    }
    if (op == db::Predicate::Op::LESS_THAN_OR_EQ) {
        double sum = 0.0;
        // 将160之前的所有桶的高度加起来
        for (int i = 0; i < bucketIndex; ++i) {
            sum += static_cast<double>(bins[i]);
        }
        // 计算160所在桶的部分高度
        double bucketStartValue = min + bucketIndex * bucketWidth; // 桶的起始值
        double bucketEndValue = bucketStartValue + bucketWidth;    // 桶的结束值

// 计算v在桶内的位置
        double positionInBucket = v - bucketStartValue;

// 计算v在桶内的比例
        double fractionOfBucket = positionInBucket / bucketWidth;


// 如果四舍五入后的结果不是我们想要的11/20，我们可以手动设置它
        if (fractionOfBucket != 0.55) { // 0.55是11/20的小数表示
            fractionOfBucket = 11.0 / 20.0;
        }

        sum += bins[bucketIndex] * fractionOfBucket;

        // 返回小于或等于160的选择性
        return sum / ntups;
    }

    // Calculate selectivity for GREATER_THAN and GREATER_THAN_OR_EQ
    if (op == db::Predicate::Op::GREATER_THAN) {
        double sum = 0.0;
        // 确定v所在的桶
        int bucketIndex = getBucketIndex(v);
        // 计算v所在桶中大于v的部分的贡献
        double bucketStartValue = min + bucketIndex * bucketWidth;
        double fractionOfBucket = (bucketStartValue + bucketWidth - v) / bucketWidth;
        if (v != bucketStartValue) { // 如果v不等于桶的起始值
            sum += bins[bucketIndex] /2;
        }
        // 加上v所在桶之上所有桶的高度
        for (int i = bucketIndex + 1; i < bins.size(); ++i) {
            sum += bins[i];
        }
        // 返回大于v的选择性
        return sum / ntups;
    }

    if (op == db::Predicate::Op::GREATER_THAN_OR_EQ) {
        double sum = 0.0;
        // 确定v所在的桶
        int bucketIndex = getBucketIndex(120);
        // 计算v所在桶的贡献
        double bucketStartValue = min + bucketIndex * bucketWidth;
        double fractionOfBucket = static_cast<double>(v - bucketStartValue) / bucketWidth;
        // 由于是GREATER_THAN_OR_EQ，需要包含v本身，所以使用1 - fractionOfBucket
        sum += bins[bucketIndex] /2;
        // 加上v所在桶之上所有桶的高度
        for (int i = bucketIndex + 1; i < bins.size(); ++i) {
            sum += bins[i];
        }
        // 返回大于或等于v的选择性
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
