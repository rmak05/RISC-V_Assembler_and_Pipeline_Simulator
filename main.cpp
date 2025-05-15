#include "pipeline.cpp"
#include <cstring>

int main(int argc, char **argv){
    if(argc >= 2){
        if(strcmp(argv[1], "display") == 0){
            display = true;

            if(argc >= 3){
                if(strcmp(argv[2], "automatic") == 0){
                    automatic = true;

                    if(argc >= 4){
                        time_delay = stoi(argv[3]);
                    }
                }
                else if(strcmp(argv[2], "manual") == 0){
                    automatic = false;
                }
            }
        }
    }

    convert_to_binary();
    run_pipeline();

    return 0;
}