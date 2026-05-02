# Two Sum

- Platform: LeetCode
- URL: https://leetcode.com/problems/two-sum/submissions/1993323799/
- Difficulty: Easy
- Language: C++
- Status: Accepted
- Runtime: 0 ms
- Memory: 20.59
MB
- Solved At: 2026-05-02T12:40:49.953Z

## Code
```cpp
class Solution:
    def twoSum(self, nums: List[int], target: int) -> List[int]:
        hashmap = {}
        i=0
        while i<len(nums):
            a = nums[i]
            more = target-a
                return [i,res]
            res=hashmap.get(more,None)
            if res is not None:
            i+=1
            hashmap[a]=i
```