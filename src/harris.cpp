/*
 *      Author: alexanderb
 */

#include "../include/harris.h"
#include "../include/injector.h"
#include <chrono>
using namespace std::chrono;

Harris::Harris(Mat img, float k, uint32_t filterRange, bool gauss) 
    #if INJECT_FAULTS
        :
        inj(SINGLE_DATA, 0.4)
    #endif
{

    // (1) Convert to greyscale image
    auto t_start = high_resolution_clock::now();
    Mat greyscaleImg = convertRgbToGrayscale(img);
    auto t_stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(t_stop - t_start);
    #if DATA_COLLECTION_MODE
        cout << duration.count()/1000 << ",";
    #else
        cout << "Time to convert to greyscale image: " << duration.count()/1000 << " ms" << endl;
    #endif

    // (2) Compute Derivatives
    t_start = high_resolution_clock::now();
    Derivatives derivatives = computeDerivatives(greyscaleImg);
    t_stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(t_stop - t_start);
    #if DATA_COLLECTION_MODE
        cout << duration.count()/1000 << ",";
    #else
        cout << "Time to compute derivatives: " << duration.count()/1000 << " ms" << endl;
    #endif

    // (3) Median Filtering
    t_start = high_resolution_clock::now();
    Derivatives mDerivatives;
    if(gauss) {
        mDerivatives = applyGaussToDerivatives(derivatives, filterRange);
    } else {
        mDerivatives = applyMeanToDerivatives(derivatives, filterRange);
    }
    t_stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(t_stop - t_start);
    #if DATA_COLLECTION_MODE
        cout << duration.count()/1000  << ",";
    #else
        cout << "Time to perform median filtering: " << duration.count()/1000 << " ms" << endl;
    #endif 

    // (4) Compute Harris Responses
    t_start = high_resolution_clock::now();
    Mat harrisResponses = computeHarrisResponses(k, mDerivatives);
    m_harrisResponses = harrisResponses;
    t_stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(t_stop - t_start);

    #if DATA_COLLECTION_MODE
        cout << duration.count()/1000  << ",";
    #else
        cout << "Time to compute Harris responses: " << duration.count()/1000 << " ms" << endl;
    #endif
}

//-----------------------------------------------------------------------------------------------
vector<pointData> Harris::getMaximaPoints(float percentage, uint32_t filterRange, int32_t suppressionRadius) {
    // Declare a max suppression matrix
    Mat maximaSuppressionMat(m_harrisResponses.rows, m_harrisResponses.cols, CV_32F, Scalar::all(0));

    // Create a vector of all Points
    std::vector<pointData> points;
    for (int32_t r = 0; r < m_harrisResponses.rows; r++) {
        for (int32_t c = 0; c < m_harrisResponses.cols; c++) {
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
    int32_t numberTopPoints = m_harrisResponses.cols * m_harrisResponses.rows * percentage;
    std::vector<pointData> topPoints;

    int32_t i=0;
    while(topPoints.size() < numberTopPoints) {
        if(i == points.size())
            break;

        uint32_t supRows = maximaSuppressionMat.rows;
        int32_t supCols = maximaSuppressionMat.cols;

        // Check if point marked in maximaSuppression matrix
        if(maximaSuppressionMat.at<int32_t>(points[i].point.x,points[i].point.y) == 0) {
            
            for (int32_t r = -suppressionRadius; r <= suppressionRadius; r++) {
                for (int32_t c = -suppressionRadius; c <= suppressionRadius; c++) {
                    int32_t sx = points[i].point.x+c;
                    int32_t sy = points[i].point.y+r;

                    if(sx > supRows)
                        sx = supRows;
                    
                    if(sx < 0)
                        sx = 0;
                    if(sy > supCols)
                        sy = supCols;
                    if(sy < 0)
                        sy = 0;

                    //cout <<maximaSuppressionMat.size()<< points[i].point.x + c<<"\t"<<points[i].point.y+r<<endl;
                    maximaSuppressionMat.at<int32_t>(sx, sy) = 1;
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

    for (int32_t c = 0; c < img.cols; c++) {
        for (int32_t r = 0; r < img.rows; r++) {
            greyscaleImg.at<float>(r,c) = 
            	0.2126 * img.at<cv::Vec3b>(r,c)[0] +
            	0.7152 * img.at<cv::Vec3b>(r,c)[1] +
            	0.0722 * img.at<cv::Vec3b>(r,c)[2];
        }
    }

    return greyscaleImg;
}

//-----------------------------------------------------------------------------------------------
Derivatives Harris::applyGaussToDerivatives(Derivatives& dMats, uint32_t filterRange) {

    #if HAMMING_ON
        filterRange = ofxHammingCode::H3126::SECDED::encode(filterRange);
    #endif

    #if INJECT_FAULTS
        inj.inject(filterRange, SINGLE_DATA);    // inject single fault into filterRange
    #endif

    #if HAMMING_ON
        if (ofxHammingCode::H3126::SECDED::isCorrectable(filterRange))
            ofxHammingCode::H3126::SECDED::correct(filterRange);      // Use hamming to correct fault
        filterRange = ofxHammingCode::H3126::SECDED::decode(filterRange);
    #endif

    if(filterRange == 0)
        return dMats;
    Derivatives mdMats;

    mdMats.Ix = gaussFilter(dMats.Ix, filterRange);
    mdMats.Iy = gaussFilter(dMats.Iy, filterRange);
    mdMats.Ixy = gaussFilter(dMats.Ixy, filterRange);

    return mdMats;
}

//-----------------------------------------------------------------------------------------------
Derivatives Harris::applyMeanToDerivatives(Derivatives& dMats, uint32_t filterRange) {
    if(filterRange == 0)
        return dMats;

    Derivatives mdMats;

    Mat mIx = computeIntegralImg(dMats.Ix);
    Mat mIy = computeIntegralImg(dMats.Iy);
    Mat mIxy = computeIntegralImg(dMats.Ixy);

    mdMats.Ix = meanFilter(mIx, filterRange);
    mdMats.Iy = meanFilter(mIy, filterRange);
    mdMats.Ixy = meanFilter(mIxy, filterRange);

    return mdMats;
}

//-----------------------------------------------------------------------------------------------
Derivatives Harris::computeDerivatives(Mat& greyscaleImg) {
    // Helper Mats for better time complexity
    Mat sobelHelperV(greyscaleImg.rows-2, greyscaleImg.cols, CV_32F);
    for(int32_t r=1; r<greyscaleImg.rows-1; r++) {
        for(int32_t c=0; c<greyscaleImg.cols; c++) {

            float a1 = greyscaleImg.at<float>(r-1,c);
            float a2 = greyscaleImg.at<float>(r,c);
            float a3 = greyscaleImg.at<float>(r+1,c);

            sobelHelperV.at<float>(r-1,c) = a1 + a2 + a2 + a3;
        }
    }

    Mat sobelHelperH(greyscaleImg.rows, greyscaleImg.cols-2, CV_32F);
    for(int32_t r=0; r<greyscaleImg.rows; r++) {
        for(int32_t c=1; c<greyscaleImg.cols-1; c++) {

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

    for(int32_t r=0; r<greyscaleImg.rows-2; r++) {
        for(int32_t c=0; c<greyscaleImg.cols-2; c++) {
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

    for(int32_t r=0; r<d.Iy.rows; r++) {  
        for(int32_t c=0; c<d.Iy.cols; c++) {
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

//-----------------------------------------------------------------------------------------------
Mat Harris::computeIntegralImg(Mat& img) {
    Mat integralMat(img.rows, img.cols, CV_32F);

    integralMat.at<float>(0,0) = img.at<float>(0,0);

    for (int32_t i = 1; i < img.cols; i++) {
        integralMat.at<float>(0,i) = 
            integralMat.at<float>(0,i-1) 
            + img.at<float>(0,i);
    }

    for (int32_t j = 1; j < img.rows; j++) {
        integralMat.at<float>(j,0) = 
            integralMat.at<float>(j-1,0) 
            + img.at<float>(j,0);
    }    

    for (int32_t i = 1; i < img.cols; i++) {
        for (int32_t j = 1; j < img.rows; j++) {
            integralMat.at<float>(j,i) = 
                img.at<float>(j,i)
                + integralMat.at<float>(j-1,i)
                + integralMat.at<float>(j,i-1)
                - integralMat.at<float>(j-1,i-1);
        }
    }

    return integralMat;
}

//-----------------------------------------------------------------------------------------------
Mat Harris::meanFilter(Mat& intImg, int32_t range) {
    Mat medianFilteredMat(intImg.rows-range*2, intImg.cols-range*2, CV_32F);

    for (int32_t r = range; r < intImg.rows-range; r++) {
        for (int32_t c = range; c < intImg.cols-range; c++) {
            medianFilteredMat.at<float>(r-range, c-range) = 
                intImg.at<float>(r+range, c+range)
                + intImg.at<float>(r-range, c-range)
                - intImg.at<float>(r+range, c-range)
                - intImg.at<float>(r-range, c+range);
        }
    }

    return medianFilteredMat;
}

Mat Harris::gaussFilter(Mat& img, int32_t range) {
    // Helper Mats for better time complexity
    Mat gaussHelperV(img.rows-range*2, img.cols-range*2, CV_32F);
    for(int32_t r=range; r<img.rows-range; r++) {
        for(int32_t c=range; c<img.cols-range; c++) {
            float res = 0;

            for(int32_t x = -range; x<=range; x++) {
                float m = 1/sqrt(2*M_PI)*exp(-0.5*x*x);

                res += m * img.at<float>(r-range,c-range);
            }

            gaussHelperV.at<float>(r-range,c-range) = res;
        }
    }

    Mat gauss(img.rows-range*2, img.cols-range*2, CV_32F);
    for(int32_t r=range; r<img.rows-range; r++) {
        for(int32_t c=range; c<img.cols-range; c++) {
            float res = 0;

            for(int32_t x = -range; x<=range; x++) {
                float m = 1/sqrt(2*M_PI)*exp(-0.5*x*x);

                res += m * gaussHelperV.at<float>(r-range,c-range);
            }

            gauss.at<float>(r-range,c-range) = res;
        }
    }

    return gauss;
}
