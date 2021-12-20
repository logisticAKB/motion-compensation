#include "MotionCompensation.h"

int main() {
    std::string inputPath = "/home/alexey/Documents/video/sample2.yuv420p";
    std::string outputPath = "../output1.yuv420p";

    int videoWidth = 1920;
    int videoHeight= 1080;

    MotionCompensation mc(inputPath, outputPath, videoWidth, videoHeight);

    int numThreads = 12;
    std::string searchType = "optimized"; // "full" or "optimized"
    bool printPSNR = true; // If "true" prints PSNR between previous and current frame

    mc.run(numThreads, searchType, printPSNR);

    return 0;
}