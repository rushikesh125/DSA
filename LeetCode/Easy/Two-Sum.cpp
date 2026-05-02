# Two Sum

- Platform: LeetCode
- URL: https://leetcode.com/problems/two-sum/description/
- Difficulty: Easy
- Language: C++
- Status: Accepted
- Runtime: N/A
- Memory: N/A
- Solved At: 2026-05-02T12:24:20.995Z

## Code
```cpp
class Solution:
    def twoSum(self, nums: List[int], target: int) -> List
[int]:
        i =0
        
        while(i<=len(nums)):
            j=i+1
            while(j<len(nums)):
                if(nums[i]+nums[j]==target):
                    return [i,j]
                j+=1
            i+=1
```