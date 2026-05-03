#include "OrderBook.hpp"
#include "Types.hpp"
#include <optional> // IWYU pragma: keep
#include <utility>  // IWYU pragma: keep
#include <vector>   // IWYU pragma: keep


namespace myex {

void OrderBook::add(const Order &order) {
  index_[order.id] = {order.side, order.price};

  if (order.side == Side::BUY) {
    bids_[order.price].push_back(order);
    bid_count_++;
  } else {
    asks_[order.price].push_back(order);
    ask_count_++;
  }
}

bool OrderBook::hasBids() const { return !bids_.empty(); }
bool OrderBook::hasAsks() const { return !asks_.empty(); }

double OrderBook::bestBidPrice() const {
  return bids_.empty() ? 0.0 : bids_.begin()->first;
}
double OrderBook::bestAskPrice() const {
  return asks_.empty() ? 0.0 : asks_.begin()->first;
}

std::optional<Order> OrderBook::peekBid() const {
  if (bids_.empty())
    return std::nullopt;
  else
    return bids_.begin()->second.front();
}
std::optional<Order> OrderBook::peekAsk() const {
  if (asks_.empty())
    return std::nullopt;
  else
    return asks_.begin()->second.front();
}

size_t OrderBook::bidOrderCount() const { return bid_count_; }
size_t OrderBook::askOrderCount() const { return ask_count_; }

void OrderBook::popBid() {
  if (bids_.empty())
    return;
  auto it = bids_.begin();
  auto &dq = it->second;
  index_.erase(dq.front().id);
  dq.pop_front();
  bid_count_--;
  if (dq.empty()) {
    bids_.erase(it);
  }
}
void OrderBook::popAsk() {
  if (asks_.empty())
    return;
  auto it = asks_.begin();
  auto &dq = it->second;
  index_.erase(dq.front().id);
  dq.pop_front();
  ask_count_--;
  if (dq.empty()) {
    asks_.erase(it);
  }
}

void OrderBook::reduceFrontBid(double qty) {
  if (bids_.empty())
    return;
  auto &front = bids_.begin()->second.front();
  front.quantity -= qty;
  if (front.quantity <= 1e-12) {
    popBid();
  }
}
void OrderBook::reduceFrontAsk(double qty) {
  if (asks_.empty())
    return;
  auto &front = asks_.begin()->second.front();
  front.quantity -= qty;
  if (front.quantity <= 1e-12) {
    popAsk();
  }
}

bool OrderBook::cancel(uint64_t order_id) {
  auto it = index_.find(order_id);
  if (it == index_.end())
    return false;

  auto [side, price] = it->second;
  index_.erase(it);

  if (side == Side::BUY) {
    auto map_it = bids_.find(price); // find price level in bids_
    if (map_it != bids_.end()) {
      auto &dq = map_it->second;
      for (auto dq_it = dq.begin(); dq_it != dq.end(); ++dq_it) {
        if (dq_it->id == order_id) {
          dq.erase(dq_it);
          bid_count_--;
          if (dq.empty())
            bids_.erase(
                map_it); // if deque now empty → erase price level from bids_
          return true;
        }
      }
    }
  } else {
    auto map_it = asks_.find(price);
    if (map_it != asks_.end()) {
      auto &dq = map_it->second;
      for (auto dq_it = dq.begin(); dq_it != dq.end(); ++dq_it) {
        if (dq_it->id == order_id) {
          dq.erase(dq_it);
          ask_count_--;
          if (dq.empty())
            asks_.erase(map_it);
          return true;
        }
      }
    }
  }
  return false;
}

OrderBookSnapshot OrderBook::getSnapshot(int depth) const {
  OrderBookSnapshot snap;
  snap.timestamp = now_ms();

  //  Process the BIDS (Buy side)
  int count = 0;
  for (auto &[price, dq] : bids_) {
    if (count >= depth)
      break;

    double total = 0.0;
    for (auto &o : dq) {
      total += o.quantity;
    }
    snap.bids.push_back({price, total, static_cast<int>(dq.size())});
    count++;
  }

  // Process the ASKS (Sell side)
  count = 0;
  for (auto &[price, dq] : asks_) {
    if (count >= depth)
      break;

    double total = 0.0;
    for (auto &o : dq) {
      total += o.quantity;
    }
    snap.asks.push_back({price, total, static_cast<int>(dq.size())});
    count++;
  }
  // Calculate Market Metrics (Mid-price and Spread)
  if (!bids_.empty() && !asks_.empty()) {
    double best_bid = bids_.begin()->first;
    double best_ask = asks_.begin()->first;
    snap.mid = (best_bid + best_ask) / 2.0;
    snap.spread = best_ask - best_bid;
  } else if (!bids_.empty()) {
    snap.mid = bids_.begin()->first;
  } else if (!asks_.empty()) {
    snap.mid = asks_.begin()->first;
  }

  return snap;
}

} // namespace myex