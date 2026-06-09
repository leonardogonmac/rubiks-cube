#include <bits/stdc++.h>
#include <boost/multi_index/hashed_index.hpp>
#include "pdb_api.h"
#include "cube.h"
#include "corner.h"

using namespace std;

size_t fhash(vector<face>& cube){
    size_t seed = 0;
    for(int i = 0; i < cube.size(); i++){
        for(int j = 0; j < cube[i].size(); j++){
            for(int k = 0; k < cube[i][j].size(); k++)
                boost::hash_combine(seed, cube[i][j][k]);
        }
    }
    return seed;
}

vector<vector<face>> get_adj(vector<face>& cube){
    vector<vector<face>> adj(12, cube);
    U(adj[0], CLOCK);
    D(adj[1], CLOCK);
    L(adj[2], CLOCK);
    R(adj[3], CLOCK);
    F(adj[4], CLOCK);
    B(adj[5], CLOCK);
    U(adj[6], COUNTER);
    D(adj[7], COUNTER);
    L(adj[8], COUNTER);
    R(adj[9], COUNTER);
    F(adj[10], COUNTER);
    B(adj[11], COUNTER);
    return adj;
}

bool face_solved(face& f){
    char a = f[0][0];
    for(vector<char> xs: f)
        for(char x: xs)
            if(x != a)
                return false;
    return true;
}

bool solved(vector<face>& cube) {
    for(face f: cube)
        if(!face_solved(f))
            return false;
    return true;
}

// void astar(vector<face>& cube, NibblePDB& cornerPDB){
//     cout << "astar started\n";
//     map<size_t, vector<face>> open, closed;
//     priority_queue<pair<size_t,vector<face>>> pq;
//     open[fhash(cube)] = cube;
//     pq.push({-cornerHeuristic(cube, cornerPDB), cube});
//     while(!pq.empty()){
//         auto [w, v] = pq.top();
//         pq.pop();
//         w = -w; w -= cornerHeuristic(v, cornerPDB);
//         open.erase(fhash(v));
//         closed[fhash(v)] = v;
//         vector<vector<face>> adj = get_adj(v);
//         for(auto a: adj){
//             if(solved(a)){
//                 print_cube(a);
//                 cout << w + 1 << '\n';
//                 return;
//             }
//             if(!open.count(fhash(a)) && !closed.count(fhash(a))){
//                 open[fhash(a)] = a;
//                 pq.push({-(cornerHeuristic(a, cornerPDB) + w + 1), a});
//             }
//         }
//     }    
// }

vector<map<size_t, pair<vector<face>,size_t>>> open, closed;
vector<priority_queue<pair<long long,vector<face>>>> pq;
vector<queue<pair<long long,pair<vector<face>,size_t>>>> qs;
vector<mutex> mtx;
mutex sol_mtx;
size_t num = 0, sol_hash = 0, sol_thread = 0;
bool solution_found = false;
NibblePDB cornerPDB("./pdbs/corner.pdb", NUM_CORNER_STATES, 1);

void enqueue(size_t target, vector<face>& a, long long w, size_t parent){
    mtx[target].lock();
    qs[target].push({(w + 1), {a, parent}});
    mtx[target].unlock();
}

void dequeue(size_t id){
    queue<pair<size_t,pair<vector<face>,size_t>>> q;
    mtx[id].lock();
    while(!qs[id].empty()){
        q.push(qs[id].front());
        qs[id].pop();
    }
    mtx[id].unlock();
    while(!q.empty()){
        auto [w, v] = q.front();
        q.pop();
        if( !open[id].count(fhash(v.first)) && !closed[id].count(fhash(v.first)) ){
            open[id][fhash(v.first)] = v;
            pq[id].push({-(cornerHeuristic(v.first, cornerPDB) + w), v.first});
        }
    }
}

void thread_astar(vector<face> cube, size_t id){
    cout << "thread " << id << " started\n";
    while(!solution_found){
		if(pq[id].empty()){
			dequeue(id);
			continue;
		}
        auto [w, v] = pq[id].top();
        pq[id].pop();
        w = -w; w -= cornerHeuristic(v, cornerPDB);
        closed[id][fhash(v)] = {v, open[id][fhash(v)].second};
        open[id].erase(fhash(v));
        vector<vector<face>> adj = get_adj(v);
        for(auto& a: adj){
            size_t target = fhash(a) % num;
            if(solved(a)){
				sol_mtx.lock();
                solution_found = true;
                sol_hash = fhash(a);
                sol_thread = id;
                closed[id][fhash(a)] = {a, fhash(v)};
				sol_mtx.unlock();
                return;
            }
            if(target == id && !open[id].count(fhash(a)) && !closed[id].count(fhash(a))){
                open[id][fhash(a)] = {a, fhash(v)};
                pq[id].push({-(cornerHeuristic(a, cornerPDB) + w + 1), a});
            }
            else
                enqueue(target, a, w, fhash(v));
        }
        dequeue(id);
    }    
}

void sample(){
    vector<face> cube = get_cube();
    int moves, m = -1;
    cin >> moves;
    while(moves--){
        while(true){
            srand(time(0));
            int mm = rand() % 6;
            if(mm != m){
                m = mm;
                break;
            }
        }
        switch (m){
            case 0:
                U(cube, CLOCK);
                break;
            case 1:
                D(cube, CLOCK);
                break;
            case 2:
                L(cube, CLOCK);
                break;
            case 3:
                R(cube, CLOCK);
                break;
            case 4:
                F(cube, CLOCK);
                break;
            case 5:
                B(cube, CLOCK);
                break;
        }
    }
    print_sample(cube);
}


size_t state = 0;
void backtrack(vector<face>& cube, size_t parent){
    if(fhash(cube) != parent){
        auto [next_cube, next_parent] = closed[parent % num][parent];
        backtrack(next_cube, next_parent);
    }
    cout << "===================================================\n";
    cout << "STATE " << state++ << '\n';
    print_cube(cube);
}

int main(int argc, char* argv[]){
    if(argc == 1){
        sample();
        return 0;
    }

    num = atoi(argv[1]);
    open = vector<map<size_t, pair<vector<face>,size_t>>>(num);
    closed = vector<map<size_t, pair<vector<face>,size_t>>>(num);
    pq = vector<priority_queue<pair<long long,vector<face>>>>(num);
    qs = vector<queue<pair<long long,pair<vector<face>,size_t>>>>(num);
    mtx = vector<mutex>(num);
    
    vector<face> cube = read_cube();
    
    open[fhash(cube) % num][fhash(cube)] = {cube, fhash(cube)};
    pq[fhash(cube) % num].push({-cornerHeuristic(cube, cornerPDB), cube});
    vector<thread> ts;
    for(size_t i = 0; i < num; i++)
        ts.emplace_back(thread_astar, cube, i);

    for(size_t i = 0; i < num; i++)
        ts[i].join();
    
    auto [sol, parent] = closed[sol_thread][sol_hash];
    backtrack(sol, parent);

    return 0;
}
