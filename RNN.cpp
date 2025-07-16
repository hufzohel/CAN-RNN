//inference logic
#include<math.h>
#include<vector>
using namespace std;

float biaslast, output, learningrate = 1; 
float W[4][3], U[4][4], Z[5][4], H[5][4], biashidden[4], V[4];
// vector<vector<float>> W = vector<vector<float>>(4, vector<float>(3));
// vector<vector<float>> U = vector<vector<float>>(4, vector<float>(4));
// vector<vector<float>> Z = vector<vector<float>>(5, vector<float>(4));
// vector<vector<float>> H = vector<vector<float>>(5, vector<float>(4));
// vector<float> biashidden = vector<float>(4);
// vector<float> V = vector<float>(4);

float sigmoid(float x){
    return 10.f / (10.f + exp(-x)); //arduino mega chơi được
}
float tanh(float x){
    float ex = exp(x);
    float enx = exp(-x);
    return (ex - enx) / (ex + enx);
}
void statecalc(vector<vector<float>> X, int j){
    for(int i = 0; i < 4; ++i){
        if(j != 0) Z[j][i] = W[j][i] * X[j][i] + U[j][i] * 0 + biashidden[i]; //first iteration, prev hidden state = 0
        else Z[j][i] = W[j][i] * X[j][i] + U[j][i] * H[j][i] + biashidden[i];
        H[j][i] = tanh(Z[j][i]);   
    }
}   
float brakeornobrake(int j){
    float z5;
    for(int i = 0; i < 4; ++i){
        z5 += V[i] * H[j][i];
    }
    z5 += biaslast;
    float h5 = sigmoid(z5);
    output = output > h5 ? output : h5;
    return output;
}


float Lossfunction(float y_pred, float y_truth){
    return -(y_truth * log(y_pred) + (1-y_truth) * log(1-y_pred));
}

void update(float y_truth, vector<vector<float>> X){
    float dV[4] = {0}, dW[4][3] = {0}, dU[4][4] = {0};
    float dL_dz5 = (output - y_truth);
    //gradient compute - if bug probably here
    //use last output instead of each iteration output for 
    //loss computation because model design for sequence decision
    //derivative by motherfucking hands.
    for(int timestep = 4; timestep >= 0; --timestep){
        for(int i = 0; i < 4; ++i){
            dV[i] += dL_dz5 * H[timestep][i];
            float dL_dz_t = dL_dz5 * V[i] * (1 - pow(H[timestep][i],2));
            for(int j = 0; j < 3; ++j){
                dW[i][j] += dL_dz_t * X[timestep][j];
            }
            for(int j = 0; j < 4; ++j){
                if(timestep != 0) dU[i][j] += dL_dz_t * H[timestep-1][j];
                else dU[i][j] += dL_dz_t * 0; //prev hidden state at first iteration = 0
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
    }
}

int main(){
    vector<vector<float>> X = vector<vector<float>>(5,vector<float>(3));
    //processing
    for(int i = 0; i < 5; ++i){
        //input reading, and storing in input array
        
    }
    if(1 - output < 0.05) Applybrake(); //Applybrake() is pseudo code for gate manipulation

    return 0;
}