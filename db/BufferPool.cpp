#include <db/BufferPool.h>
#include <db/Database.h>

using namespace db;

// TODO pa1.3: implement
BufferPool::BufferPool(int numPages) {
    capacity=numPages;

}

Page *BufferPool::getPage(const TransactionId &tid, PageId *pid) {
    // TODO pa1.3: implement
    auto it = page.find(pid);
    if (it != page.end()) {
        // The page is already in the buffer pool, return it
        return it->second.get();
    }
    uint8_t *data = HeapPage::createEmptyPageData();
    auto newPage = std::make_unique<HeapPage>(*dynamic_cast<HeapPageId*>(pid), data);
    page[pid] = std::move(newPage);
    return page[pid].get();
}

void BufferPool::evictPage() {
    // do nothing for now
}
