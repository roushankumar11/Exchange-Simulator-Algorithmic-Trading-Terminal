#pragma once
#include <cstdint>
#include <string>
#include<unordered_map>
#include <chrono>
#include<vector>
namespace myex {

    enum class Side { BUY, SELL };
    enum class OrderType { LIMIT, MARKET };

    struct Order {
        uint64_t    id        = 0;      
        std::string user_id;            
        std::string symbol;             
        Side        side      = Side::BUY;
        OrderType   type      = OrderType::LIMIT;
        double      price     = 0.0;
        double      quantity  = 0.0;
        uint64_t    timestamp = 0;
        bool        is_bot    = false;
    };

    struct Trade{
        uint64_t    id        = 0;
        std::string symbol;
        double      price     = 0.0;
        double      quantity  = 0.0;
        std::string buyer_id;
        std::string seller_id;
        uint64_t    buyer_order_id = 0;
        uint64_t    seller_order_id = 0;
        uint64_t    timestamp = 0;
    };

    struct Candle {
    std::string symbol;
    double   open   = 0.0;
    double   high   = 0.0;
    double   low    = 0.0;
    double   close  = 0.0;
    double   volume = 0.0;
    uint64_t timestamp = 0;  
    };

    struct Position{
        double holdings = 0.0;
        double avg_cost = 0.0;
        double realized_pnl = 0.0;
    };
    
    struct Portfolio{
        double cash = 100000.0;
        std::unordered_map<std::string, Position> positions;
    };

    struct OrderBookLevel {
    double price       = 0.0;
    double total_qty   = 0.0;
    int    order_count = 0;
    }; 

    struct OrderBookSnapshot{
        std::string symbol;
        double mid = 0.0;
        double spread = 0.0;
        std::vector<OrderBookLevel> bids;
        std::vector<OrderBookLevel> asks;
        uint64_t timestamp = 0;
    };

    inline uint64_t now_ms(){
        using namespace std::chrono;
        return static_cast<uint64_t>(
            duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count()         
        );
    }
} 