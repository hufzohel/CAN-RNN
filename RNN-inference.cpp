//inference logic
#include<math.h>
#include<iostream>
using namespace std;

float biaslast = 0.0996, output, learningrate = 0.001; 
float W[4][3] = {{-0.0675, -0.0780, 0.0423},{0.0527, -0.0930, 0.0974},{0.0982, -0.0328, -0.0964},{0.0256, 0.0339, -0.0686}}
, U[4][4] = {{-0.0383, -0.0574, 0.0545, -0.0898}, {-0.0904, -0.0889, 0.0852, 0.0269}, {0.0047, 0.0085, -0.0414, -0.0051}, {-0.0802, 0.0255, -0.0014, -0.0330}}, Z[5][4], H[5][4], biashidden[4] = {0.0491, -0.0811, -0.0792, -0.0252}, V[4] = {-0.0207, -0.0901, -0.0508, 0.0196};
// vector<vector<float>> W = vector<vector<float>>(4, vector<float>(3));
// vector<vector<float>> U = vector<vector<float>>(4, vector<float>(4));
// vector<vector<float>> Z = vector<vector<float>>(5, vector<float>(4));
// vector<vector<float>> H = vector<vector<float>>(5, vector<float>(4));
// vector<float> biashidden = vector<float>(4);
// vector<float> V = vector<float>(4);

float W_init = W[0][0], U_init = U[0][0], bh_init = biashidden[0], V_init = V[0];


float sigmoid(float x){
    return 10.f / (10.f + exp(-x)); //arduino mega chơi được
}
float mtanh(float x){
    float ex = exp(x);
    float enx = exp(-x);
    return (ex - enx) / (ex + enx);
}
void statecalc(float X[], int j){
    // for(int i = 0; i < 4; ++i){
    //     if(j != 0) Z[j][i] = W[j][i] * X[j][i] + U[j][i] * 0 + biashidden[i]; //first iteration, prev hidden state = 0
    //     else Z[j][i] = W[j][i] * X[j][i] + U[j][i] * H[j][i] + biashidden[i];
    //     H[j][i] = tanh(Z[j][i]);   
    // }

    for(int m = 0; m < 4; ++m){
        float z = 0;
        for(int n = 0; n < 3; ++n){
            z += W[m][n] * X[n];
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
    float z5;
    for(int i = 0; i < 4; ++i){
        z5 += V[i] * H[j][i];
    }
    z5 += biaslast;
    float h5 = sigmoid(z5);
    output = output > h5 ? output : h5;
    return output;
}


int main(){
    
    return 0;
}