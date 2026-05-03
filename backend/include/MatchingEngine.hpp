#pragma once
#include "OrderBook.hpp"
#include <atomic>
#include <functional>
#include <mutex>
#include <vector>

namespace myex {
using TradeCallback = std::function<void(const Trade &)>;

class MatchingEngine {
public:
  explicit MatchingEngine(TradeCallback cb = nullptr);
  int processOrder(Order &order);

  bool cancelOrder(uint64_t oredr_id);

  OrderBookSnapshot getSnapshot(int depth = 20) const;

  void setTradeCallback(TradeCallback cb);

  uint64_t tradeCount() const { return trade_counter_.load(); }

private:
  void matchLimit(Order &order, std::vector<Trade> &trades);
  void matchMarket(Order &order, std::vector<Trade> &trades);

  orderBook book_;
  mutable std::mutex mutex_;
  TradeCallback trade_cb_;

  std::atomic<uint64_t> order_counter_{0};
  std::atomic<uint64_t> trade_counter_{0};
};
} // namespace myex
