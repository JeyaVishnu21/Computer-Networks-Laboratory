class Solution {
public:

    int findMaximumXOR(vector<int>& nums) {
        int si=nums.size();
        std::deque<int> res(si);
        for(int i =0;i<nums.size();i++) {
            for (int j=i;j<nums.size();j++) {
                int x = nums[i]^nums[j];
                res.push_front(x);
            }
        }
        std::sort(res.begin(),res.end());
        return res.back();

    }
};
