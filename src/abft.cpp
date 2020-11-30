
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
    //cout<<"max errors: "<<maxErrors<<endl;
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

bool abft_check(Mat &img, Mat &rCheck, Mat &cCheck, bool useThresh, runStats &stats)
{
    return abft_check(img, rCheck, cCheck, useThresh, maxErrorLimit, stats);
}

bool abft_check(Mat &img, Mat &rCheck, Mat &cCheck, bool useThresh,int errThresh, runStats &stats)
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
        stats.abft.correctedErrors++;
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

        //cout<<"max errors found: "<<maxErrors<<endl;

        if (maxErrors < errThresh)
        {
            // error quantity in image does not justify repeat
            return true;
        }
        else
        {
            //cout<<"ABFT: THRESHOLD EXCEDED"<<endl;
            return false;
        }
    }
    else
    {
        return false;
    }

}

// Weighted checksum code below not currently in use

// weighted checksum encoding
bool abft_addChecksums(Mat m, float &s1, float &s2)
{
    //we know the kernel size so predefine the weights
    float weights[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};

    s1 = cv::sum(m)[0];
    
    s2 = 0;

    for (int i = 0; i < m.cols; ++i)
    {
        s2 += (weights[i]*m.at<float>(0,i));
        
    }

}

// weighted checksum check
bool abft_check(Mat &m, float &s1, float &s2)
{
    
    //we know the kernel size so predefine the weights
    float weights[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};

    float s1c= cv::sum(m)[0];
    float s2c = 0;

    for (int i = 0; i < m.cols; ++i)
    {
        s2c += weights[i]*m.at<float>(0,i);
    }
    //cout<<"s1c: "<<s1c<<"  s2c: "<<s2c<<"  s1: "<<s1<< "   s2: "<<s2<<endl;
    float d2 = s2c - s2;
    float d1 = s1c - s1;

    if (d2 == 0 && d1 != 0)
    {
        cout<<"WABFT corrected"<<endl;
        // normal checksum is incorrect
        s1 = s1c;
        return true;
    }
    else if (d2 != 0 && d1 == 0)
    {
        cout<<"WABFT corrected"<<endl;
        // weighted checksum is wrong
        s2 = s2c;
        return true;
    }
    else if (d2 == 0 && d1 == 0)
    {
        //cout<<"EVERYTHING CORRECT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
        return true; // everything correct
    }
    else 
    {
         cout<<"WABFT corrected"<<endl;
        float kf = log2(d2/d1);
        int k = std::round(kf);
        // pos k is in error
        if (k >= 0 && k < m.cols)
        {
            float newVal = s1 - (s1c - m.at<float>(0,k));
            m.at<float>(0,k) = newVal;
            return true;
        }
        else
        {
            cout<<"------------------------------------------------------------ can't correct"<<endl;
            cout<<"d2: "<<d2<<"  d1: "<<d1<<"  m: "<<m<<endl;
            return false;
        }



    }
    
   return true;
}