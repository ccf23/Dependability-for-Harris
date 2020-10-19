
#include "../include/abft.h"
/*
abftMat::abftMat(Mat m, abftMode mode)
{
    /*
    switch (mode)
    {
    case NONE:
        img = m;
        hasRowSum = false;
        hasColSum = false;
        break;
    case GRAYSCALE:
        Mat im = m;
        img = doGrayscaleABFT(im);
        break;
    default:

        break;
    }*/
    
//}//*/
/*
Mat abftMat::encode(Mat m, abftMode mode)
{
    switch (mode)
    {
    case NONE:
        img = m;
        hasRowSum = false;
        hasColSum = false;
        break;
    case GRAYSCALE:
        img = doGrayscaleABFT(m);
        break;
    default:

        break;
    }
    return img;
}
*/

Mat doGrayscaleABFT(Mat im)
{
    int rows = im.rows;
    int cols = im.cols;

    Mat rSum = Mat::zeros(rows, 1, CV_8UC3);    
    Mat cSum = Mat::zeros(cols + 1, 1, CV_8UC3);

    float cs0, cs1, cs2;    
    for (int r = 0; r < rows; ++r)
    {
        cs0 = cs1 = cs2 = 0;
        for (int i = 0; i < cols; ++i)
        {
            cs0 += im.at<Vec3b>(r,i)[0];
            cs1 += im.at<Vec3b>(r,i)[1];
            cs2 += im.at<Vec3b>(r,i)[2];
        }
        //cout<<r<<endl;
        rSum.at<Vec3b>(r,1)[0] = cs0;
        rSum.at<Vec3b>(r,1)[1] = cs1;
        rSum.at<Vec3b>(r,1)[2] = cs2;
    }
    
    for (int c = 0; c < cols; ++c)
    {
        cs0 = cs1 = cs2 = 0;
        for (int i = 0; i < rows; ++i)
        {
            cs0 += im.at<Vec3b>(i,c)[0];
            cs1 += im.at<Vec3b>(i,c)[1];
            cs2 += im.at<Vec3b>(i,c)[2];
        }
        cSum.at<Vec3b>(c,1)[0] = cs0;
        cSum.at<Vec3b>(c,1)[1] = cs1;
        cSum.at<Vec3b>(c,1)[2] = cs2;
    } 
    
    hconcat(im, rSum, im);
    vconcat(im, cSum.t(), im);
    return im;
}

bool grayscaleABFTCheck(Mat img)
{
    int rows = img.rows - 1;
    int cols = img.cols - 1;
    float sum;
    for (int r = 0; r < rows; ++r)
    {
        sum = 0;
        for (int i = 0; i < cols; ++i)
        {
            sum += img.at<float>(r,i);
        }
        if (sum != img.at<float>(r,cols + 1))
        {
            cout<<sum<<"\t"<<img.at<float>(r,cols + 1)<<"\t"<<r<<endl;
            return false;
        }
    }

    for (int c = 0; c < cols; ++c)
    {
        sum = 0;
        for (int i = 0; i < cols; ++i)
        {
            sum += img.at<float>(i,c);
        }
        if (sum != img.at<float>(rows + 1,c))
        {
            return false;
        }
    }
    return true;
}
