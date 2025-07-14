//inference logic
#include<math.h>

struct RNN{
    private:
        float weightz[5][4], weightrecur[5][4], biashidden[5][4], state[4] ,weightzlast[5][4], biaslast[5], output; 
        float sigmoid(float x){
            return 1.0f / (1.0f + exp(-x)); //arduino mega chơi được
        }
        float tanh(float x){
            float ex = exp(x);
            float enx = exp(-x);
            return (ex - enx) / (ex + enx);
        }
    public:
        void statecalc(float x[], int j){
            for(int i = 0; i < 4; ++i){
                state[i] = tanh(weightz[j][i] * x[i] + weightrecur[j][i] * state[i] + biashidden[j][i]);
            }
        }   
        float brakeornobrake(int j){
            float z5;
            for(int i = 0; i < 4; ++i){
                z5 += weightzlast[j][i] * state[i];
            }
            z5 += biaslast[j];
            float h5 = sigmoid(z5);
            output = output > h5 ? output : h5;
            return output;
        }
        float getoutput(){
            return output;
        }
};

int main(){
    RNN motherfuckingcar;
    float input[4];
    //processing
    for(int i = 0; i < 5; ++i){
        //input reading, and storing in input array.
        motherfuckingcar.statecalc(input, i);
        motherfuckingcar.brakeornobrake(i);
    }
    if(1 - motherfuckingcar.getoutput() < 0.05) Applybrake(); //Applybrake() is pseudo code for gate manipulation

    return 0;
}