
#include "../include/abft.h"

Mat doGrayscaleABFT(Mat im)
{
   
    int rows = im.rows;
    int cols = im.cols;

    im.convertTo(im,CV_32FC3);
    Mat rSum = Mat::zeros(rows, 1, CV_32FC3);    
    Mat cSum = Mat::zeros(cols + 1, 1, CV_32FC3);

    float cs0, cs1, cs2;    
    for (int r = 0; r < rows; ++r)
    {
        cs0 = cs1 = cs2 = 0;
        for (int i = 0; i < cols; ++i)
        {
            cs0 += im.at<Vec3f>(r,i)[0];
            cs1 += im.at<Vec3f>(r,i)[1];
            cs2 += im.at<Vec3f>(r,i)[2];
        }

        rSum.at<Vec3f>(r,0)[0] = cs0;
        rSum.at<Vec3f>(r,0)[1] = cs1;
        rSum.at<Vec3f>(r,0)[2] = cs2;
    }
    
    for (int c = 0; c <= cols; ++c)
    {
        cs0 = cs1 = cs2 = 0;
        for (int i = 0; i < rows; ++i)
        {
            cs0 += im.at<Vec3f>(i,c)[0];
            cs1 += im.at<Vec3f>(i,c)[1];
            cs2 += im.at<Vec3f>(i,c)[2];
        }
        cSum.at<Vec3f>(c,0)[0] = cs0;
        cSum.at<Vec3f>(c,0)[1] = cs1;
        cSum.at<Vec3f>(c,0)[2] = cs2;
    } 

    hconcat(im, rSum, im);
    vconcat(im, cSum.t(), im);
    return im;
}

bool grayscaleABFTCheck(Mat& img)
{
    //cout<<img.at<float>(1,img.cols - 1)<<endl;
    int rows = img.rows;
    int cols = img.cols;
    float sum;
    for (int r = 0; r < rows - 1; ++r)
    {
        sum = 0;
        for (int i = 0; i < cols - 1; ++i)
        {
            sum += img.at<float>(r,i);
        }
        if (abs(sum - img.at<float>(r,cols - 1))> 1)
        {
            //cout<<setprecision(10)<<sum<<"\t"<<img.at<float>(r,cols - 1)<<"\t"<<r<<endl;
            return false;
        }
    }

    
    for (int c = 0; c < cols - 1; ++c)
    {
        sum = 0;
        for (int i = 0; i < rows - 1; ++i)
        {
            sum += img.at<float>(i,c);
        }
        if (abs(sum - img.at<float>(rows - 1,c)) > 1)
        {
            //cout<<setprecision(10)<<sum<<"\t"<<img.at<float>(rows - 1,c)<<"\t"<<c<<endl;
            return false;
        }
    }
    return true;
}

bool abft_addChecksums(Mat img, Mat &rCheck, Mat &cCheck)
{
    reduce(img, cCheck, 0, REDUCE_SUM);
    reduce(img, rCheck, 1, REDUCE_SUM);
}

bool abft_check(Mat &img, Mat &rCheck, Mat &cCheck)
{
    Mat newCcheck, newRcheck, cDiff, rDiff;
    reduce(img, newCcheck, 0, REDUCE_SUM);
    reduce(img, newRcheck, 1, REDUCE_SUM);

    cDiff = abs(newCcheck - cCheck);
    rDiff = abs(newRcheck - rCheck);

    Mat zeroCheckC = Mat::ones(cDiff.rows, cDiff.cols, cDiff.type());
    Mat zeroCheckR = Mat::ones(rDiff.rows, rDiff.cols, rDiff.type());


    compare(cDiff, zeroCheckC, cDiff, CMP_GE);
    compare(rDiff, zeroCheckR, rDiff, CMP_GE);

    cout<<rDiff<<endl;
}
