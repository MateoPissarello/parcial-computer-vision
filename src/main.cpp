// Coin recognition and value estimation using OpenCV.
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace fs = std::filesystem;

// Configuration constants
constexpr double TOLERANCE_PERCENTAGE = 0.10;
const cv::Scalar LABEL_COLOR = cv::Scalar(255, 0, 0);  // Blue for labels
const cv::Scalar CIRCLE_COLOR = cv::Scalar(0, 255, 0);  // Green for circles
const cv::Scalar UNKNOWN_CIRCLE_COLOR = cv::Scalar(0, 0, 255);  // Red for unknown coins
constexpr double FONT_SCALE = 1.2;
constexpr int FONT_THICKNESS = 3;

struct CoinType
{
    std::string name;
    double value;
    double radiusOld;
    double radiusNew;
    int count;
};

struct DetectionResult
{
    std::vector<cv::Vec3f> circles;
    cv::Mat baseImage;  // clean image without debug overlays
};

struct ClassificationResult
{
    cv::Mat labeledImage;
    double totalAmount;
    std::vector<CoinType> coinCounts;
};

std::vector<CoinType> initializeCoinTypes()
{
    return {
        {"50 COP", 50.0, 82.6, 64.4, 0},    // Average of 64 and 83
        {"100 COP", 100.0, 86.2, 75.2, 0},  // Average of 75 and 86
        {"200 COP", 200.0, 91.6, 85.0, 0},  // Average of 85 and 92
        {"500 COP", 500.0, 89.0, 87.4, 0},  // Average of 87 and 89
        {"1000 COP", 1000.0, 100.8, 100.8, 0} // Same value
    };
}

const CoinType* findMatchingCoin(
    double detectedRadius,
    const std::vector<CoinType>& coinTypes,
    double tolerance)
{
    const CoinType* bestMatch = nullptr;
    double bestDifference = std::numeric_limits<double>::max();

    const auto evaluateCandidate = [&](const CoinType& coinType, double coinRadius) {
        const double difference = std::abs(detectedRadius - coinRadius);
        const double margin = coinRadius * tolerance;  // tolerance tied to the radius being compared

        if (difference <= margin && difference < bestDifference)
        {
            bestDifference = difference;
            bestMatch = &coinType;
        }
    };

    for (const auto& coinType : coinTypes)
    {
        evaluateCandidate(coinType, coinType.radiusOld);
        evaluateCandidate(coinType, coinType.radiusNew);
    }

    return bestMatch;
}

ClassificationResult classifyAndLabelCoins(
    const DetectionResult& detection,
    const std::vector<CoinType>& coinTypes,
    double tolerance)
{
    ClassificationResult result;
    result.labeledImage = detection.baseImage.clone();
    result.totalAmount = 0.0;
    result.coinCounts = coinTypes;
    
    for (const auto& circle : detection.circles)
    {
        const double detectedRadius = circle[2];
        const cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
        const int radius = cvRound(detectedRadius);
        
        const CoinType* matchedCoin = findMatchingCoin(detectedRadius, result.coinCounts, tolerance);
        
        if (matchedCoin != nullptr)
        {
            // Classified coin - draw green circle and blue label
            cv::circle(result.labeledImage, center, radius, CIRCLE_COLOR, 4);
            cv::putText(
                result.labeledImage,
                matchedCoin->name,
                center,
                cv::FONT_HERSHEY_SIMPLEX,
                FONT_SCALE,
                LABEL_COLOR,
                FONT_THICKNESS);
            
            // Update counter and total
            for (auto& coinType : result.coinCounts)
            {
                if (coinType.name == matchedCoin->name)
                {
                    coinType.count++;
                    result.totalAmount += coinType.value;
                    break;
                }
            }
        }
        else
        {
            // Unclassified coin - draw red circle and "?" label
            cv::circle(result.labeledImage, center, radius, UNKNOWN_CIRCLE_COLOR, 4);
            cv::putText(
                result.labeledImage,
                "?",
                center,
                cv::FONT_HERSHEY_SIMPLEX,
                FONT_SCALE,
                UNKNOWN_CIRCLE_COLOR,
                FONT_THICKNESS);
        }
    }
    
    return result;
}

void printSummary(const ClassificationResult& result)
{
    // Print count of coins by denomination
    for (const auto& coinType : result.coinCounts)
    {
        if (coinType.count > 0)
        {
            std::cout << coinType.name << " = " << coinType.count << "x\n";
        }
    }
    
    // Print total amount
    std::cout << "Total: " << result.totalAmount << " COP\n";
}

void saveResults(const ClassificationResult& result, const fs::path& outputDir)
{
    // Create output directory if it doesn't exist
    fs::create_directories(outputDir);
    
    // Save labeled image
    const fs::path outputPath = outputDir / "imagen_monedas_valuadas.jpg";
    const bool success = cv::imwrite(outputPath.string(), result.labeledImage);
    
    if (!success)
    {
        throw std::runtime_error("Failed to save labeled image to: " + outputPath.string());
    }
}

DetectionResult detectCoins(const std::string &inputPath, const fs::path &outputDir)
{
    cv::Mat coins = cv::imread(inputPath, cv::IMREAD_COLOR);
    if (coins.empty())
    {
        throw std::runtime_error("Falló al leer la imagen: " + inputPath);
    }

    cv::Mat gray;
    cv::cvtColor(coins, gray, cv::COLOR_BGR2GRAY);
    cv::Mat blurred;
    cv::medianBlur(gray, blurred, 7);

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(
        blurred,
        circles,
        cv::HOUGH_GRADIENT,
        2.0,   // dp
        80.0,  // min distance between centers
        100.0, // higher threshold for Canny edge detector
        100.0, // Muy importante para evitar falsos positivos
        50,    // min radius
        300);  // max radius

    cv::Mat debugAnnotated = coins.clone();
    for (const auto &detected : circles)
    {
        const cv::Point center(cvRound(detected[0]), cvRound(detected[1]));
        const int radius = cvRound(detected[2]);
        cv::circle(debugAnnotated, center, radius, cv::Scalar(0, 255, 0), 4);
        cv::putText(
            debugAnnotated,
            std::to_string(detected[2]) + " px",
            center,
            cv::FONT_HERSHEY_SIMPLEX,
            1.0,
            cv::Scalar(0, 0, 0),
            4);
    }

    fs::create_directories(outputDir);
    const fs::path debugPath = outputDir / "imagen_segmentacion_monedas.jpg";
    cv::imwrite(debugPath.string(), debugAnnotated);

    return {circles, coins};
}

int main()
{
    try
    {
        const std::string inputPath = "data/coins_colombia.jpeg";
        const fs::path outputDir = "output/coin_amount";
        
        // Detection
        DetectionResult detection = detectCoins(inputPath, outputDir);
        
        // Classification and labeling
        std::vector<CoinType> coinTypes = initializeCoinTypes();
        ClassificationResult result = classifyAndLabelCoins(
            detection, 
            coinTypes, 
            TOLERANCE_PERCENTAGE
        );
        
        // Save results
        saveResults(result, outputDir);
        
        // Print summary
        printSummary(result);
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
    return 0;
}
