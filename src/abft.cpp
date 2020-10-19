
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
    
    for (int c = 1; c <= cols; ++c)
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
    /*
    for (int i = 0; i < cSum.cols; ++i)
    {
        cout<<cSum.at<Vec3f>(i,0)
    }
    */
    hconcat(im, rSum, im);
    vconcat(im, cSum.t(), im);
    imwrite("./im.jpg",im);
    //cout<<"p0: "<<(float)im.at<Vec3f>(2, cols)[0]<<endl;
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
            cout<<setprecision(10)<<sum<<"\t"<<img.at<float>(r,cols - 1)<<"\t"<<r<<endl;
            //return false;
        }
    }

    for (int c = 0; c < cols - 1; ++c)
    {
        sum = 0;
        for (int i = 0; i < rows - 1; ++i)
        {
            sum += img.at<float>(i,c);
        }
        if (sum != img.at<float>(rows - 1,c))
        {
            return false;
        }
    }
    return true;
}
