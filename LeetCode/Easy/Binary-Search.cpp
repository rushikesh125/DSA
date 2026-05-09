# Binary Search

- Platform: LeetCode
- URL: https://leetcode.com/problems/binary-search/submissions/1998717983/
- Difficulty: Easy
- Language: C++
- Status: Accepted
- Runtime: 0 ms
- Memory: 20.46
MB
- Solved At: 2026-05-09T09:55:07.290Z

## Code
```cpp
class Solution:
    def search(self, nums: List[int], target: 
int) -> int:
        n = len(nums)-1
        start = 0
        end = n
        while start <=end:
            mid = (start+end)//2
            if(nums[mid]==target):
                return mid
            elif nums[mid]<target:
                start = mid+1
            else:
                end = mid-1
            
        return -1
```