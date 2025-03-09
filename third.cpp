#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

struct Item {
    int value, weight;
};

struct Node {
    int level, value, weight, bound;
};

bool cmp(Item a, Item b) {
    double r1 = (double)a.value / a.weight;
    double r2 = (double)b.value / b.weight;
    return r1 > r2;
}

int bound(Node u, int n, int W, vector<Item>& items) {
    if (u.weight >= W)
        return 0;
    
    int profit_bound = u.value;
    int j = u.level + 1;
    int totweight = u.weight;
    
    while ((j < n) && (totweight + items[j].weight <= W)) {
        totweight += items[j].weight;
        profit_bound += items[j].value;
        j++;
    }
    
    if (j < n)
        profit_bound += (W - totweight) * items[j].value / items[j].weight;
    
    return profit_bound;
}

int knapsack(int W, vector<Item>& items, int n) {
    sort(items.begin(), items.end(), cmp);
    
    queue<Node> Q;
    Node u, v;
    u.level = -1;
    u.value = u.weight = 0;
    Q.push(u);
    
    int maxProfit = 0;
    
    while (!Q.empty()) {
        u = Q.front();
        Q.pop();
        
        if (u.level == n - 1)
            continue;
        
        v.level = u.level + 1;
        v.weight = u.weight + items[v.level].weight;
        v.value = u.value + items[v.level].value;
        
        if (v.weight <= W && v.value > maxProfit)
            maxProfit = v.value;
        
        v.bound = bound(v, n, W, items);
        
        if (v.bound > maxProfit)
            Q.push(v);
        
        v.weight = u.weight;
        v.value = u.value;
        v.bound = bound(v, n, W, items);
        
        if (v.bound > maxProfit)
            Q.push(v);
    }
    
    return maxProfit;
}

int main() {
    int N, W;
    cin >> N >> W;
    
    vector<Item> items(N);
    for (int i = 0; i < N; i++) {
        cin >> items[i].value >> items[i].weight;
    }
    
    cout << knapsack(W, items, N) << endl;
    
    return 0;
}
