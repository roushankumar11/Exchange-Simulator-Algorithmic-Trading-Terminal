#pragma once
#include "Types.hpp"
#include <map>
#include <deque>
#include <unordered_map>
#include <optional> // IWYU pragma: keep

namespace myex{
    struct OrderLocation{
        Side side;
        double price;
    };

    class OrderBook{
        public:
            void add(const Order& order);
            bool cancel(uint64_t order_id);

            bool hasBids() const;
            bool hasAsks() const;
        
            double bestBidPrice() const;
            double bestAskPrice() const;

            // Peek at the best bid/ask without removing
            std::optional<Order> peekBid() const;
            std::optional<Order> peekAsk() const;

            // Remove the front order from the best bid/ask level
            void popBid();
            void popAsk();

            // For partial fills: reduce the front order's quantity on the best bid/ask
            void reduceFrontBid(double qty);
            void reduceFrontAsk(double qty);

            // Number of resting orders (all levels)
            size_t bidOrderCount() const;
            size_t askOrderCount() const;  
            
            // Get a snapshot of the top `depth` price levels on each side
            OrderBookSnapshot getSnapshot(int depth = 20) const;

        private:
            std::map<double, std::deque<Order> ,std :: greater<double>> bids_;
            std::map<double, std::deque<Order>> asks_;

            std::unordered_map<uint64_t, OrderLocation> index_;
            
            size_t bid_count_ = 0;
            size_t ask_count_ = 0;       
            
    };

}