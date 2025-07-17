#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;

struct Step {
    float gas, brake, speed;
};

vector<Step> generate_sequence(int& label) {
    vector<Step> seq;
    label = 0;

    float speed = static_cast<float>(rand()) / RAND_MAX * 46.0f + 3.5f;  // starting speed
    float last_speed = speed;
    float last_gas = 0.0f;
    int lasttype = 0;
    int maxtype1 = 0;
    int maxtype2 = 0;

    int type_1_contradiction_count = 0;
    int type_2_contradiction_count = 0;
    
    int case_type = rand() % 100;
    if(case_type < 35) case_type = 1;
    else if(case_type < 70) case_type = 2;
    int inject_time = rand() % 3 + 1;

    for (int t = 0; t < 5; ++t) {
        Step s;

        // Random gas and brake values
        s.gas = static_cast<float>(rand()) / RAND_MAX;
        s.brake = static_cast<float>(rand()) / RAND_MAX;

        // Avoid simultaneous pedal press
        if (s.gas > 0.2f && s.brake > 0.2f) {
            if ((rand() % 2) == 0) s.gas = 0;
            else s.brake = 0;
        }

        // Simulate acceleration
        float accel;
        if (s.gas > 0.05f && s.brake < 0.05f) {
            accel = s.gas * 2.5f; 
        } else if (s.brake > 0.05f) {
            accel = -s.brake * 4.0f;
        } else {
            accel = -0.9f; // natural deceleration
        }
        if(t == inject_time || t == inject_time + 1){
            if(case_type == 1){
                s.brake = 0.85f + static_cast<float>(rand()) / RAND_MAX * 0.1f; //si
                accel = 1.5f;  // simulate rising speed 
            }else if(case_type == 2){
                accel = static_cast<float>(rand()) / RAND_MAX * 3.5f + 5.0f;
            }
        }

        s.speed = max(0.0f, speed + accel * 0.2f); //timestep of 0.5s
        float delta_speed = s.speed - last_speed; //derivative of each step
        float delta_gas = s.gas - last_gas;
        float max_safe_accel = (speed < 3.0f) ? 2.f : 1.f; //dynamic speed accel threshold

        // CONTRADICTION LABELING: brake is high and speed isnt decreasing
        // Contradictions have to happen >= 2 simultaneously 
        if (s.brake > 0.6f && delta_speed >= 0.0f) {
            if(lasttype == 2){
                maxtype1 = maxtype1 > type_1_contradiction_count ? maxtype1 : type_1_contradiction_count;
                type_1_contradiction_count = 1;
            }else{
                lasttype = 1;
                type_1_contradiction_count++;
            }
        }
        if (delta_speed > max_safe_accel || delta_gas >= 0.4f) { // EDGE CASE: sudden spike
            if(lasttype == 1){
                maxtype1 = maxtype1 > type_1_contradiction_count ? maxtype1 : type_1_contradiction_count;
                type_2_contradiction_count = 1;
            }else{
                lasttype = 2;
                type_2_contradiction_count++;
            }
        }
        seq.push_back(s);
        last_gas = s.gas;
        last_speed = s.speed;
        speed = s.speed;

    }

    // If contradiction happened for ≥2 steps -> label = 1
    if (maxtype1 >= 2 || maxtype2 >= 2) {
        label = 1;
    }

    return seq;
}

void write_csv(const string& filename, int num_sequences) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file!\n";
        return;
    }

    for (int i = 0; i < num_sequences; ++i) {
        int label = 0;
        vector<Step> seq = generate_sequence(label);
        for (int t = 0; t < 5; ++t) {
            file << fixed << setprecision(3)
                 << setw(6) << seq[t].gas << " "
                 << setw(6) << seq[t].brake << " "
                 << setw(6) << seq[t].speed << " ";
        }
        file << label << "\n";
    }

    file.close();
    cout << "Data written to: " << filename << "\n";
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    write_csv("rnn_contradiction_data.txt", 500);
    return 0;
}
