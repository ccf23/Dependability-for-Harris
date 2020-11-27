
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

bool grayscaleABFTCheck(Mat& img, bool useThresh)
{
    //cout<<img.at<float>(1,img.cols - 1)<<endl;
    int rows = img.rows;
    int cols = img.cols;
    float sum;
    int rErrs = 0;
    int cErrs = 0;
    for (int r = 0; r < rows - 1; ++r)
    {
        sum = 0;
        for (int i = 0; i < cols - 1; ++i)
        {
            sum += img.at<float>(r,i);
        }
        if (abs(sum - img.at<float>(r,cols - 1))> .001)
        {
            //cout<<setprecision(10)<<sum<<"\t"<<img.at<float>(r,cols - 1)<<"\t"<<r<<endl;
            rErrs++;
        }
    }

    
    for (int c = 0; c < cols - 1; ++c)
    {
        sum = 0;
        for (int i = 0; i < rows - 1; ++i)
        {
            sum += img.at<float>(i,c);
        }
        if (abs(sum - img.at<float>(rows - 1,c)) > .001)
        {
            //cout<<setprecision(10)<<sum<<"\t"<<img.at<float>(rows - 1,c)<<"\t"<<c<<endl;
            cErrs++;
        }
    }
    int maxErrors = cErrs*rErrs;

    if (maxErrors == 0)
    {
        return true;
    }
    else
    {
        return (useThresh && maxErrors < maxErrorLimit);
    }
}

bool abft_addChecksums(Mat img, Mat &rCheck, Mat &cCheck)
{
    reduce(img, cCheck, 0, REDUCE_SUM);
    reduce(img, rCheck, 1, REDUCE_SUM);

}

bool abft_check(Mat &img, Mat &rCheck, Mat &cCheck, bool useThresh)
{
    Mat newCcheck, newRcheck, cDiff, rDiff, cErr, rErr;

    reduce(img, newCcheck, 0, REDUCE_SUM);
    reduce(img, newRcheck, 1, REDUCE_SUM);

    absdiff(newCcheck, cCheck, cDiff);
    absdiff(newRcheck, rCheck, rDiff);

    Mat zeroCheckC = Mat(cDiff.rows, cDiff.cols, cDiff.type(),Scalar::all(.001));
    Mat zeroCheckR = Mat(rDiff.rows, rDiff.cols, rDiff.type(), Scalar::all(.001));


    compare(cDiff, zeroCheckC, cErr, CMP_GE);
    compare(rDiff, zeroCheckR, rErr, CMP_GE);

    vector<Point> cErrPts, rErrPts;

    findNonZero(cErr, cErrPts);
    findNonZero(rErr, rErrPts);

    if (rErrPts.size() == 0 && cErrPts.size() == 0)
    {
        //cout<<"ABFT: NO ERRORS DETECTED"<<endl;
        return true;
    }
    else if (rErrPts.size() == 1 && cErrPts.size() == 1)
    {
        //cout<<"ABFT: CORRECTABLE ERROR DETECTED"<<endl;

        // correct value
        int v = cErrPts[0].x;
        int u = rErrPts[0].y;

        float diff = newCcheck.at<float>(0,v) - img.at<float>(u,v);
        img.at<float>(u,v) = cCheck.at<float>(0,v) - diff;
        return true;
    }
    else if (useThresh)
    {
        // maximum number of possible errors
        int maxErrors = rErrPts.size()*cErrPts.size();

        if (maxErrors < maxErrorLimit)
        {
            // error quantity in image does not justify repeat
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }

}
