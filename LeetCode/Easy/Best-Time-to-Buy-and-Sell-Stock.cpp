# Best Time to Buy and Sell Stock

- Platform: LeetCode
- URL: https://leetcode.com/problems/best-time-to-buy-and-sell-stock/
- Difficulty: Easy
- Language: C++
- Status: Accepted
- Runtime: 0 ms
- Memory: N/A
- Solved At: 2026-05-02T13:06:32.191Z

## Code
```cpp
class Solution:
    def maxProfit(self, prices: List[int]) -> int:
        mini = prices[0]
        profit = 0
        i=1
        while(i<len(prices)):
            cost = prices[i]-mini
            profit = max(cost,profit)
            mini = min(mini,prices[i])
            i+=1
        return profit
```