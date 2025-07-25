//inference logic
#include<math.h>
#include<vector>
#include<fstream>
#include<cstdlib>
#include<sstream>
#include<string>
#include<iostream>
using namespace std;

float biaslast = 0.0996, output, learningrate = 0.001; 
float W[4][3] = {{-0.0675, -0.0780, 0.0423},{0.0527, -0.0930, 0.0974},{0.0982, -0.0328, -0.0964},{0.0256, 0.0339, -0.0686}}
, U[4][4] = {{-0.0383, -0.0574, 0.0545, -0.0898}, {-0.0904, -0.0889, 0.0852, 0.0269}, {0.0047, 0.0085, -0.0414, -0.0051}, {-0.0802, 0.0255, -0.0014, -0.0330}}, Z[5][4], H[5][4], biashidden[4] = {0.0491, -0.0811, -0.0792, -0.0252}, V[4] = {-0.0207, -0.0901, -0.0508, 0.0196};

float W_init = W[0][0], U_init = U[0][0], bh_init = biashidden[0], V_init = V[0];


float sigmoid(float x){
    return 1.f / (1.f + exp(-x)); //arduino mega chơi được
}
float mtanh(float x){
    if (x > 20.0f) return 1.0f;
    if (x < -20.0f) return -1.0f;
    float ex = exp(x);
    float enx = exp(-x);
    return (ex - enx) / (ex + enx);
}
void statecalc(vector<vector<float>> X, int j){

    for(int m = 0; m < 4; ++m){
        float z = 0;
        for(int n = 0; n < 3; ++n){
            z += W[m][n] * X[j][n];
        }
        for(int n = 0; n < 4; ++n){
            if(j!=0) z += U[m][n] * H[j-1][n];
            else break;
        }
        Z[j][m] = z + biashidden[m];
        H[j][m] = mtanh(Z[j][m]);
    }

}   
float brakeornobrake(int j){
    float z5 = 0;
    for(int i = 0; i < 4; ++i){
        z5 += V[i] * H[j][i];
    }
    z5 += biaslast;
    output = sigmoid(z5);
    return output;
}


float Lossfunction(float y_pred, float y_truth){
    y_pred = max(1e-7f, min(1.f - 1e-7f, y_pred));
    return -(y_truth * log(y_pred) + (1-y_truth) * log(1-y_pred));
}

float clip(float x, float limit = 10.0f) {
    return x > limit ? limit : (x < -limit ? -limit : x);
}

void update(float y_truth, vector<vector<float>> X){
    float dV[4] = {0}, dW[4][3] = {0}, dU[4][4] = {0}, dbh[4] = {0};
    float dL_dz5 = (output - y_truth);
    
    for(int timestep = 4; timestep >= 0; --timestep){
        for(int i = 0; i < 4; ++i){
            float dL_dz_t = dL_dz5 * V[i] * (1 - pow(H[timestep][i],2));
            dV[i] += clip(dL_dz5 * H[timestep][i]);
            dbh[i] += clip(dL_dz_t);
            for(int j = 0; j < 3; ++j){
                dW[i][j] += clip(dL_dz_t * X[timestep][j]);
            }
            for(int j = 0; j < 4; ++j){
                if(timestep != 0) dU[i][j] += clip(dL_dz_t * H[timestep-1][j]);
            }
        }
    }
    //update
    for(int i = 0 ; i < 4; ++i){
        for(int j = 0; j < 3; ++j){
            W[i][j] -= learningrate * dW[i][j];
        }
        for(int j = 0; j < 4; ++j){
            U[i][j] -= learningrate * dU[i][j];
        }
        V[i] -= learningrate * dV[i];
        biashidden[i] -= learningrate * dbh[i];
    }
    biaslast -= learningrate * dL_dz5;
}

bool read_data_file(const string& filename, vector<vector<vector<float>>>& sequences, vector<int>& labels) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error reading file!\n";
        return false;
    }

    string line;
    while (getline(file, line)) {
        istringstream ss(line);
        vector<vector<float>> sequence(5, vector<float>(3));
        for (int t = 0; t < 5; ++t) {
            ss >> sequence[t][0];  // gas
            ss >> sequence[t][1];  // brake
            ss >> sequence[t][2];  // speed
        }
        int label;
        ss >> label;
        sequences.push_back(sequence);
        labels.push_back(label);
    }

    file.close();
    return true;
}

bool isTrained(){
    return W[0][0] != W_init && U[0][0] != U_init && biashidden[0] != bh_init && V[0] != V_init; 
}

int main(){
    vector<vector<vector<float>>> Xseq;
    vector<int> lbl;
    //processing

    read_data_file("rnn_contradiction_data.txt", Xseq, lbl);
    for(int i = 0; i < 100; ++i){
        for(int j = 0; j < Xseq.size(); ++j){
            for(int m = 0; m < 5; ++m){
                statecalc(Xseq[j], m);
            }
            brakeornobrake(4);
            float loss = Lossfunction(output, lbl[j]);
            cout << "Epoch " << i << " | Loss: " << loss << " | Output: " << output << endl;
            update(lbl[j], Xseq[j]);
            for(int k = 0; k < 5; ++k){
                for(int l = 0; l < 4; ++l){
                    H[k][l] = 0;
                }
            }
        }
    }
    // if(1 - output < 0.05) Applybrake(); //Applybrake() is pseudo code for gate manipulation
    if(isTrained()) cout << "model is trained"; 
    else cout << "Motherfucker";
    // cout << "W" << endl;
    // for(int i = 0; i < 4; ++i){
    //     for(int j = 0; j < 3; ++j){
    //         cout << W[i][j] << " " << endl;
    //     }
    // }
    // cout << "U" << endl;
    // for(int i = 0; i < 4; ++i){
    //     for(int j = 0; j < 4; ++j){
    //         cout << U[i][j] << " " << endl;
    //     }
    // }
    // cout << "V" << endl;
    // for(int i = 0; i < 4; ++i){
    //     cout << V[i] << " " << endl;
    // }
    // cout << "biashidden" << endl; 
    // for(int i = 0; i < 4; ++i){
    //     cout << biashidden[i] << " " << endl;
    // } 
    // cout << "biaslast" << endl;
    // cout << biaslast << " " << endl;
    return 0;
}