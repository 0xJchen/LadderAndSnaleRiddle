#include <iostream>
#include <vector>
#include <tuple>
#include <map>
#include <algorithm>
#include <cmath>
#include <limits>
#include <iterator>

using namespace std;

const map<int, int> LADDERS = {{4, 75}, {5, 15}, {19, 41}, {28, 50}, {35, 96}, {44, 82}, {59, 95}, {58, 94}, {70, 91}};
const map<int, int> SNAKES = {{21, 3}, {52, 23}, {31, 8}, {76, 41}, {81, 62}, {98, 12}, {47, 30}, {88, 67}};

const vector<int> LDS = {4, 5, 19, 28, 35, 44, 59, 58, 70};
const vector<int> SNKS = {21, 52, 31, 76, 81, 98, 47, 88};

using State = tuple<int, vector<int>, vector<int>>;

vector<vector<int>> generateSubsets(const vector<int>& vec) {
    vector<vector<int>> subsets = {{}};
    for (int num : vec) {
        size_t currentSize = subsets.size();
        for (size_t i = 0; i < currentSize; ++i) {
            vector<int> newSubset = subsets[i];
            newSubset.push_back(num);
            subsets.push_back(newSubset);
        }
    }
    return subsets;
}

State transition(const State &state, int action) {
    int pos;
    vector<int> ladders, snakes;
    tie(pos, ladders, snakes) = state;
    pos += action;
    auto it_ladder = find(ladders.begin(), ladders.end(), pos);
    auto it_snake = find(snakes.begin(), snakes.end(), pos);
    if (it_ladder != ladders.end()) {
        pos = LADDERS.at(pos);
        ladders.erase(it_ladder);
    } else if (it_snake != snakes.end()) {
        pos = SNAKES.at(pos);
        snakes.erase(it_snake);
    }
    return make_tuple(pos, ladders, snakes);
}

int reward(const State &state, const State &next_state) {
    if (get<0>(state) == 100) return 0;
    return -1;
}

vector<int> actions(const State &state) {
    int pos = get<0>(state);
    vector<int> act;
    if (pos == 100) return {0};
    if (pos <= 94) act = {1, 2, 3, 4, 5, 6};
    else for (int i = 1; i <= 100 - pos; ++i) act.push_back(i);
    return act;
}

int main() {

size_t total_ladder_subsets = generateSubsets(LDS).size();
size_t total_snake_subsets = generateSubsets(SNKS).size();
size_t total_states = 100 * total_ladder_subsets * total_snake_subsets;
size_t state_counter = 0;
    vector<State> states;
cout << "finish preparation "<< endl;
for (const auto& ladder_subset : generateSubsets(LDS)) {
    for (const auto& snake_subset : generateSubsets(SNKS)) {
        for (int id_tuple = 1; id_tuple <= 100; ++id_tuple) {
            states.emplace_back(id_tuple, ladder_subset, snake_subset);
            state_counter++;

            // Calculate the percentage of completion
            int percentage_complete = static_cast<int>(100.0 * state_counter / total_states);
            // Update progress every 1% of completion
            if (state_counter % (total_states / 100) == 0) {
                cout << "Progress: " << percentage_complete << "%" << endl;
            }
        }
    }
}

cout << "State initialization complete." << endl;

    // for (const auto& ladder_subset : generateSubsets(LDS)) {
    //     for (const auto& snake_subset : generateSubsets(SNKS)) {
    //         for (int id_tuple = 1; id_tuple <= 100; ++id_tuple) {
    //             states.emplace_back(id_tuple, ladder_subset, snake_subset);
    //         }
    //     }
    // }

    cout << "Number of states: " << states.size() << endl;

    State initial_state = make_tuple(1, LDS, SNKS);

    const double gamma = 1;
    const double theta = 0.01;

    state_counter = 0;
map<State, double> V;
for (const auto &state : states) {
    if (get<0>(state) == 100) V[state] = 0;
    else V[state] = -10;

    // Calculate the percentage of completion
    int percentage_complete = static_cast<int>(100.0 * ++state_counter / total_states);
    // Update progress every 1% of completion
    if (state_counter % (total_states / 100) == 0) {
        cout << "Progress: " << percentage_complete << "%" << endl;
    }
}

    map<State, int> pi;

// size_t total_states = states.size();
size_t iteration = 0;

while (true) {
    double delta = 0;
    state_counter = 0;
    cout << "Iteration: " << ++iteration << endl;

    for (const auto &state : states) {
        double v = V[state];
        vector<int> act = actions(state);
        vector<double> Q(act.size(), 0);
        for (size_t i = 0; i < act.size(); ++i) {
            State next_state = transition(state, act[i]);
            Q[i] = reward(state, next_state) + gamma * V[next_state];
        }
        V[state] = *max_element(Q.begin(), Q.end());
        delta = max(delta, abs(v - V[state]));

        // Calculate the percentage of completion for this iteration
        int percentage_complete = static_cast<int>(100.0 * ++state_counter / total_states);
        // Update progress every 1% of completion
        if (state_counter % (total_states / 100) == 0) {
            cout << "Progress: " << percentage_complete << "%" << endl;
        }
    }

    cout << "Delta: " << delta << endl;

    if (delta < theta) {
        cout << "Convergence achieved." << endl;
        break;
    }
}

    double v = V[initial_state];
    cout << "Optimal time: " << -v << endl;

    // Extract policy
    for (const auto &state : states) {
        vector<int> act = actions(state);
        vector<double> Q;
        for (const int &action : act) {
            State next_state = transition(state, action);
            Q.push_back(reward(state, next_state) + gamma * V[next_state]);
        }
        auto max_element_it = max_element(Q.begin(), Q.end());
        pi[state] = act[distance(Q.begin(), max_element_it)];
    }

    // Display actions
    State cur_state = initial_state;
    while (get<0>(cur_state) != 100) {
        cout << pi[cur_state] << endl;
        cur_state = transition(cur_state, pi[cur_state]);
    }

    return 0;
}
