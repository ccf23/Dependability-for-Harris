/*
 *      Author: alexanderb
 */

#include "../include/harris.h"
#include "../include/abft.h"
#include <chrono>
using namespace std::chrono;

Harris::Harris(Mat img, float k, int filterRange, bool gauss) {

    // (1) Convert to greyscale image
    auto t_start = high_resolution_clock::now();
    #if ABFT_ON
        img = doGrayscaleABFT(img);
    #else
        img.convertTo(img, CV_32F);    
    #endif
    Mat greyscaleImg = convertRgbToGrayscale(img);
    #if ABFT_ON
        bool correct = grayscaleABFTCheck(greyscaleImg);
        
        if (correct)
        {
            greyscaleImg = cv::Mat(greyscaleImg,cv::Range(0,greyscaleImg.rows - 2), cv::Range(0,greyscaleImg.cols - 2));
            cout<<"greyscale correct"<<endl;
        }
        else
        {

            //TODO: handle error
        }

    #endif
    auto t_stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(t_stop - t_start);
    #if DATA_COLLECTION_MODE
        cout << duration.count()/1000 << ",";
    #else
        cout << "Time to convert to greyscale image: " << duration.count()/1000 << " ms" << endl;
    #endif

    #if ABFT_ON
        Mat a, b;
        abft_addChecksums(greyscaleImg,a,b);
        bool val = abft_check(greyscaleImg,a,b);
    #endif

    // (2) Compute Derivatives
    t_start = high_resolution_clock::now();
    Derivatives derivatives = computeDerivatives(greyscaleImg);

    #if ABFT_ON
        //generate checksums for Derivatives
        Mat IxC, IxR, IyC, IyR, IxyC, IxyR;
        abft_addChecksums(derivatives.Ix, IxR, IxC);
        abft_addChecksums(derivatives.Iy, IyR, IyC);
        abft_addChecksums(derivatives.Ixy, IxyR, IxyC);

        // error injection
        bool matchX, matchY, matchXY;
        matchX = matchY = matchXY = false; 
        matchX = abft_check(derivatives.Ix, IxR, IxC);
        matchY = abft_check(derivatives.Iy, IyR, IyC);
        matchXY = abft_check(derivatives.Ixy, IxyR, IxyC);

        if (!matchX || !matchY || !matchXY)
        {
            // repeat derivatives?
        }

    #endif
    t_stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(t_stop - t_start);
    #if DATA_COLLECTION_MODE
        cout << duration.count()/1000 << ",";
    #else
        cout << "Time to compute derivatives: " << duration.count()/1000 << " ms" << endl;
    #endif

    // (3) Gaussian Filtering
    t_start = high_resolution_clock::now();
    Derivatives mDerivatives;
    if(gauss) 
    {
        mDerivatives = applyGaussToDerivatives(derivatives, filterRange);
    } 
    t_stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(t_stop - t_start);
    #if DATA_COLLECTION_MODE
        cout << duration.count()/1000  << ",";
    #else
        cout << "Time to perform gaussian filtering: " << duration.count()/1000 << " ms" << endl;
    #endif 

    // (4) Compute Harris Responses
    t_start = high_resolution_clock::now();
    Mat harrisResponses = computeHarrisResponses(k, mDerivatives);
    m_harrisResponses = harrisResponses;
    #if ABFT_ON
        // check created here, verified when get Maxima Points is called
        abft_addChecksums(m_harrisResponses,hrRc,hrCc);
    #endif
    t_stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(t_stop - t_start);

    #if DATA_COLLECTION_MODE
        cout << duration.count()/1000  << ",";
    #else
        cout << "Time to compute Harris responses: " << duration.count()/1000 << " ms" << endl;
    #endif
}

//-----------------------------------------------------------------------------------------------
vector<pointData> Harris::getMaximaPoints(float percentage, int filterRange, int suppressionRadius) {
    // Declare a max suppression matrix
    Mat maximaSuppressionMat(m_harrisResponses.rows, m_harrisResponses.cols, CV_32F, Scalar::all(0));

    // Create a vector of all Points
    std::vector<pointData> points;
    for (int r = 0; r < m_harrisResponses.rows; r++) {
        #if ABFT_ON
            // perform continual verification during this critical part
            
            if (!abft_check(m_harrisResponses,hrRc,hrCc))
            {
                // corrupted, go back to begining
            }//*/

        #endif
        for (int c = 0; c < m_harrisResponses.cols; c++) {
            Point p(r,c); 

            pointData d;
            d.cornerResponse = m_harrisResponses.at<float>(r,c);
            d.point = p;

            points.push_back(d);
        }
    }

    // Sort points by corner Response
    sort(points.begin(), points.end(), by_cornerResponse());

    // Get top points, given by the percentage
    int numberTopPoints = m_harrisResponses.cols * m_harrisResponses.rows * percentage;
    std::vector<pointData> topPoints;

    int i=0;
    while(topPoints.size() < numberTopPoints) {
        if(i == points.size())
            break;

        int supRows = maximaSuppressionMat.rows;
        int supCols = maximaSuppressionMat.cols;
    
        // Check if point marked in maximaSuppression matrix
        if(maximaSuppressionMat.at<int>(points[i].point.x,points[i].point.y) == 0) {
            
            for (int r = -suppressionRadius; r <= suppressionRadius; r++) {
                for (int c = -suppressionRadius; c <= suppressionRadius; c++) {
                    int sx = points[i].point.x+c;
                    int sy = points[i].point.y+r;

                    // bound checking
                    if(sx > supRows)
                        sx = supRows;
                    if(sx < 0)
                        sx = 0;
                    if(sy > supCols)
                        sy = supCols;
                    if(sy < 0)
                        sy = 0;

                    //cout <<maximaSuppressionMat.size()<< points[i].point.x + c<<"\t"<<points[i].point.y+r<<endl;
                    maximaSuppressionMat.at<int>(sx, sy) = 1;
                }
            }
            
            // Convert back to original image coordinate system 
            points[i].point.x += 1 + filterRange;
            points[i].point.y += 1 + filterRange;
            topPoints.push_back(points[i]);
        }

        i++;
    }

    return topPoints;
}

//-----------------------------------------------------------------------------------------------
Mat Harris::convertRgbToGrayscale(Mat& img) {
    Mat greyscaleImg(img.rows, img.cols, CV_32F);

    for (int c = 0; c < img.cols; c++) {
        for (int r = 0; r < img.rows; r++) {
            greyscaleImg.at<float>(r,c) = 
            	0.2126 * img.at<cv::Vec3f>(r,c)[0] +
            	0.7152 * img.at<cv::Vec3f>(r,c)[1] +
            	0.0722 * img.at<cv::Vec3f>(r,c)[2];
        }

    }
    return greyscaleImg;
}

//-----------------------------------------------------------------------------------------------
Derivatives Harris::applyGaussToDerivatives(Derivatives& dMats, int filterRange) {
    if(filterRange == 0)
        return dMats;

    Derivatives mdMats;

    mdMats.Ix = gaussFilter(dMats.Ix, filterRange);
    #if ABFT_ON // protect after generation
        Mat IxRc, IxCc;
        abft_addChecksums(mdMats.Ix, IxRc, IxCc);
    #endif

    mdMats.Iy = gaussFilter(dMats.Iy, filterRange);
    #if ABFT_ON // protect after generation
        Mat IyRc, IyCc;
        abft_addChecksums(mdMats.Iy, IyRc, IyCc);
    #endif

    mdMats.Ixy = gaussFilter(dMats.Ixy, filterRange);
    #if ABFT_ON // protect after generation
        Mat IxyRc, IxyCc;
        abft_addChecksums(mdMats.Ixy, IxyRc, IxyCc);
    #endif

    #if ABFT_ON
        // validate ABFT before returning
        if (!abft_check(mdMats.Ix,IxRc,IxCc) || !abft_check(mdMats.Iy,IyRc,IyCc) || !abft_check(mdMats.Ixy,IxyRc,IxyCc))
        {
            // repeat calculations
        }
    #endif
    return mdMats;
}

//-----------------------------------------------------------------------------------------------
Derivatives Harris::computeDerivatives(Mat& greyscaleImg) {
    // Helper Mats for better time complexity
    Mat sobelHelperV(greyscaleImg.rows-2, greyscaleImg.cols, CV_32F);
    for(int r=1; r<greyscaleImg.rows-1; r++) {
        for(int c=0; c<greyscaleImg.cols; c++) {

            float a1 = greyscaleImg.at<float>(r-1,c);
            float a2 = greyscaleImg.at<float>(r,c);
            float a3 = greyscaleImg.at<float>(r+1,c);

            sobelHelperV.at<float>(r-1,c) = a1 + a2 + a2 + a3;
        }
    }

    Mat sobelHelperH(greyscaleImg.rows, greyscaleImg.cols-2, CV_32F);
    for(int r=0; r<greyscaleImg.rows; r++) {
        for(int c=1; c<greyscaleImg.cols-1; c++) {

            float a1 = greyscaleImg.at<float>(r,c-1);
            float a2 = greyscaleImg.at<float>(r,c);
            float a3 = greyscaleImg.at<float>(r,c+1);

            sobelHelperH.at<float>(r,c-1) = a1 + a2 + a2 + a3;
        }
    }

    // Apply Sobel filter to compute 1st derivatives
    Mat Ix(greyscaleImg.rows-2, greyscaleImg.cols-2, CV_32F);
    Mat Iy(greyscaleImg.rows-2, greyscaleImg.cols-2, CV_32F);
    Mat Ixy(greyscaleImg.rows-2, greyscaleImg.cols-2, CV_32F);

    for(int r=0; r<greyscaleImg.rows-2; r++) {
        for(int c=0; c<greyscaleImg.cols-2; c++) {
            Ix.at<float>(r,c) = sobelHelperH.at<float>(r,c) - sobelHelperH.at<float>(r+2,c);
            Iy.at<float>(r,c) = - sobelHelperV.at<float>(r,c) + sobelHelperV.at<float>(r,c+2);
            Ixy.at<float>(r,c) = Ix.at<float>(r,c) * Iy.at<float>(r,c);
        }
    }

    Derivatives d;
    d.Ix = Ix;
    d.Iy = Iy;
    d.Ixy = Iy;

    return d;
}

//-----------------------------------------------------------------------------------------------
Mat Harris::computeHarrisResponses(float k, Derivatives& d) {
    Mat M(d.Iy.rows, d.Ix.cols, CV_32F);

    for(int r=0; r<d.Iy.rows; r++) {  
        for(int c=0; c<d.Iy.cols; c++) {
            float   a11, a12,
                    a21, a22;

            a11 = d.Ix.at<float>(r,c) * d.Ix.at<float>(r,c);
            a22 = d.Iy.at<float>(r,c) * d.Iy.at<float>(r,c);
            a21 = d.Ix.at<float>(r,c) * d.Iy.at<float>(r,c);
            a12 = d.Ix.at<float>(r,c) * d.Iy.at<float>(r,c);

            float det = a11*a22 - a12*a21;
            float trace = a11 + a22;

            M.at<float>(r,c) = abs(det - k * trace*trace);
        }
    }

    return M;
}

Mat Harris::gaussFilter(Mat& img, int range) {
    Mat m(1,2*range+1, CV_32F); // gaussian Kernel
    for (int i = -range; i<= range; ++i)
    {
        float val = 1/sqrt(2*M_PI)*exp(-0.5*i*i);
        m.at<float>(0,i+range) = val;
    }
    #if ABFT_ON
        Mat mRcheck, mCcheck;
        abft_addChecksums(m,mRcheck,mCcheck);
    #endif
    
    
    // Helper Mats for better time complexity
    Mat gaussHelperV(img.rows-range*2, img.cols-range*2, CV_32F);
    for(int r=range; r<img.rows-range; r++) {
        #if ABFT_ON
                bool kernel_good = abft_check(m,mRcheck,mCcheck);
                if (!kernel_good)
                {
                    // TODO: take action to restart? return to checkpoint?
                }
            #endif
        for(int c=range; c<img.cols-range; c++) {
            float res = 0;

            for(int x = -range; x<=range; x++) {
                res += m.at<float>(0,x+3) * img.at<float>(r-range,c-range);
            }

            gaussHelperV.at<float>(r-range,c-range) = res;
        }
    }

    Mat gauss(img.rows-range*2, img.cols-range*2, CV_32F);
    for(int r=range; r<img.rows-range; r++) {
         #if ABFT_ON
                bool kernel_good = abft_check(m,mRcheck,mCcheck);
                if (!kernel_good)
                {
                    // TODO: take action to restart? return to checkpoint?
                }
            #endif
        for(int c=range; c<img.cols-range; c++) {
            float res = 0;
            for(int x = -range; x<=range; x++) {
                res += m.at<float>(0,x+3) * gaussHelperV.at<float>(r-range,c-range);
            }

            gauss.at<float>(r-range,c-range) = res;
        }
    }

    return gauss;
}
