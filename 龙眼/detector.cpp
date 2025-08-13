#include"detector.h"
#include<opencv2/opencv.hpp>
#include<opencv2/cudaarithm.hpp>
#include <opencv2/cudaimgproc.hpp>
#include<Windows.h>



class detecor::Ipml
{
private:
    std::vector<cv::Mat>images;

    std::vector<cv::Mat> ReadImage(cv::String pattern)
    {
        std::vector<cv::String> fn;
        cv::glob(pattern, fn, false);
        std::vector<cv::Mat> images;
        int count = fn.size(); //number of png files in images folder
        for (int i = 0; i < count; i++)
        {
            images.emplace_back(cv::imread(fn[i]));
        }
        return images;
    }
    
    void BGR2HSV(cv::cuda::GpuMat& img)
    {
        cv::cuda::cvtColor(img, img, cv::COLOR_BGR2HSV);

#if _DEBUG
        cv::Mat temp;
        img.download(temp);
        cv::imshow("BGR2HSV", temp);
#endif
    }

    void inRange(cv::cuda::GpuMat& img)
    {
        cv::cuda::inRange(img, cv::Scalar(15, 40, 50), cv::Scalar(35, 150, 255), img);
#if _DEBUG
        cv::Mat temp;
        img.download(temp);
        cv::imshow("inRange", temp);
#endif
    }

    void morphology(cv::cuda::GpuMat& img)
    {
        // 形态学处理
        cv::morphologyEx(img, img, cv::MORPH_OPEN, cv::Mat(), cv::Point(-1, -1), 2);
        cv::morphologyEx(img, img, cv::MORPH_CLOSE, cv::Mat(), cv::Point(-1, -1), 2);

#if _DEBUG
        cv::Mat temp;
        img.download(temp);
        cv::imshow("morphology", temp);
#endif
    }

    std::vector<std::vector<cv::Point>> findContours(cv::cuda::GpuMat& img)
    {
        cv::Mat temp;
        img.download(temp);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(temp, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        return contours;
    }

    void select(std::vector<std::vector<cv::Point>>& contours, 
        cv::Mat& img, std::vector<cv::Rect>& fruit)
    {
        for (auto& c : contours) 
        {
            double area = cv::contourArea(c);
            double perimeter = cv::arcLength(c, true);
            double circularity = 4 * CV_PI * area / (perimeter * perimeter);

            if (area > 200 && circularity > 0.7) 
            { // 龙眼候选
                cv::Rect roi = cv::boundingRect(c);
                //cv::Mat fruit = img(roi);
                fruit.push_back(roi);

                // 检测“眼”
                cv::Mat lab;
                cv::cvtColor(fruit, lab, cv::COLOR_BGR2Lab);
                std::vector<cv::Mat> channels;
                cv::split(lab, channels);

                cv::Mat thresh;
                cv::adaptiveThreshold(channels[0], thresh, 255, cv::ADAPTIVE_THRESH_MEAN_C,
                    cv::THRESH_BINARY_INV, 15, 10);
            }
        }
    }

public:
    void set_img_path(std::string& path)
    {
        images = ReadImage(path);
    }

    void run()
    {
        std::vector<cv::Rect>result;
        std::vector<std::vector<cv::Point>>contours;

        for (auto& img : images)
        {
            cv::cuda::GpuMat gpum;
            gpum.upload(img);

            BGR2HSV(gpum);
            inRange(gpum);
            morphology(gpum);

            

            for (auto& rec : result)
                cv::rectangle(img, rec, cv::Scalar(0, 255, 0), 1, 8, 0);
            cv::imshow("result", img);

            result.resize(0);
            contours.resize(0);

            Sleep(3000);
        }
    }
};

void detecor::set_img_path(std::string& path)
{
    pipml->set_img_path(path);
}

void detecor::run()
{
    pipml->run();
}

detecor::detecor()
{
    pipml.set_ptr(new Ipml);
}
