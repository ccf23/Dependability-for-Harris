/*
 *      Author: alexanderb
 */

#include "../include/harris.h"
#include <chrono>
#include <opencv2/core.hpp> //minMaxLoc
using namespace std::chrono;

#if ASSERTIONS_ON
  // function to check range of each saved matrix element
  int iterateMat(Mat matrix, float lB, float uB){

    //check each element of matrix
    for (int i = 0; i <matrix.rows; i++)
    {
      for (int j = 0; j <matrix.cols; j++)
      {
        //cout << matrix.at<float>(i,j) << "              " << endl;
        if (matrix.at<float>(i,j) < lB || matrix.at<float>(i,j) > uB)
        {
          return 1; //there is a fault, so stop checking rest of matrix
        }
      }
    }
    return 0; //there is no fault
  }

    // int iterateFlo(float Flo, int lB, int uB){
    //
    //   if (Flo < lB || Flo > uB) // make sure to delete = sign
    //   {
    //     return 1; //there is a fault, so stop checking rest of matrix
    //   }
    //
    //   return 0; //there is no fault
//  }
#endif

Harris::Harris(Mat img, float k, int filterRange, bool gauss) {

    #if ASSERTIONS_ON
      Mat greyscaleImg;

      int count_fault =0;// gets int since it is the first time it is called

      greyscaleImg = convertRgbToGrayscale(img);
      ck.greyA = greyscaleImg.clone();

////////////////////////////////////////////////////////////////
      // int count_fault =0;// gets int since it is the first time it is called
      // do { // runs through loop once and checks if there is a fault
      //   greyscaleImg = convertRgbToGrayscale(img);
      //   ck.greyA = greyscaleImg.clone();
      //
      //   // will only loop at most three times so that conitinuous (permanent?) fault doesnt make the program stuck
      //   // if break is needed, the rest of program will most likely use a break as well, which will cause latency in overall program
      //   count_fault += 1;
      //   if (count_fault > 3){
      //     break;
      //   }
      // } while (iterate(ck.greyA,0,256) == 1);
////////////////////////////////////////////

    #else
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
    #endif

    #if ASSERTIONS_ON

    // (2) Compute Derivatives
      //Derivatives derivatives = computeDerivatives(greyscaleImg);
      Derivatives derivatives;

      count_fault =0;
      do { // runs through loop once and checks if there is a fault
        derivatives = computeDerivatives(greyscaleImg);// watch out for multiple matrices saved
        ck.derivxA = derivatives.Ix.clone();
        ck.derivyA = derivatives.Iy.clone();

        count_fault += 1;
        if (count_fault > 3){
          break;
        }
      } while (iterateMat(ck.derivxA,-1024,1024) == 1 || iterateMat(ck.derivyA,-1024,1024) == 1);


      ck.derivxyA = derivatives.Ixy.clone(); //could delete

    #else

      // (2) Compute Derivatives-- sobel filter
      t_start = high_resolution_clock::now();
      Derivatives derivatives = computeDerivatives(greyscaleImg);
      t_stop = high_resolution_clock::now();
      duration = duration_cast<microseconds>(t_stop - t_start);
      #if DATA_COLLECTION_MODE
          cout << duration.count()/1000 << ",";
      #else
          cout << "Time to compute derivatives: " << duration.count()/1000 << " ms" << endl;
      #endif

    #endif

    #if ASSERTIONS_ON
      Derivatives mDerivatives; // could clean the code a bit more since the guass filter will always be applied

      count_fault =0;
      do { // runs through loop once and checks if there is a fault
        if(gauss) {
            mDerivatives = applyGaussToDerivatives(derivatives, filterRange);
        } else {// could delete
            mDerivatives = applyMeanToDerivatives(derivatives, filterRange);
        }// watch out for multiple matrices saved
        ck.mderivxA = mDerivatives.Ix.clone();
        ck.mderivyA = mDerivatives.Iy.clone();

        count_fault += 1;
        if (count_fault > 3){
          break;
        }
      } while (iterateMat(ck.mderivxA,-1024,1024) == 1 || iterateMat(ck.mderivyA,-1024,1024) == 1);

      ck.mderivxyA = mDerivatives.Ixy.clone(); //could delete

    #else

      // (3) Median Filtering
      t_start = high_resolution_clock::now();
      Derivatives tives;

      //could delete if statement
      if(gauss) {
          mDerivatives = applyGaussToDerivatives(derivatives, filterRange);
      } else { //could delete
          mDerivatives = applyMeanToDerivatives(derivatives, filterRange);
      }
      t_stop = high_resolution_clock::now();
      duration = duration_cast<microseconds>(t_stop - t_start);
      #if DATA_COLLECTION_MODE
          cout << duration.count()/1000  << ",";
      #else
          cout << "Time to perform median filtering: " << duration.count()/1000 << " ms" << endl;
      #endif

    #endif

    #if ASSERTIONS_ON
      Mat harrisResponses;

      float maxHarrisResponse;

      count_fault =0;
      do { // runs through loop once and checks if there is a fault
        harrisResponses = computeHarrisResponses(k, mDerivatives);
        ck.cornersA = harrisResponses.clone();

        count_fault += 1;
        if (count_fault > 3){
          break;
        }
      } while (iterateMat(ck.cornersA,0,4.398*pow(10,12)) == 1); ///overflow, so remove this checkpoint

      m_harrisResponses = harrisResponses;
    #else
      // (4) Compute Harris Response
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

    #endif
}

//-----------------------------------------------------------------------------------------------
vector<pointData> Harris::getMaximaPoints(float percentage, int filterRange, int suppressionRadius) {
    // Declare a max suppression matrix
    Mat maximaSuppressionMat(m_harrisResponses.rows, m_harrisResponses.cols, CV_32F, Scalar::all(0));

    // Create a vector of all Points
    std::vector<pointData> points;
    for (int r = 0; r < m_harrisResponses.rows; r++) {
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

    //cout << ck.greyA <<endl;
    ////////////debugging for assertions/////////////////////////////
    double min, max;
    minMaxLoc(ck.greyA, &min, &max);
    cout << min << "   grey  "<< max <<endl;
    minMaxLoc(ck.derivxA, &min, &max);
    cout << min << "   derivx  "<< max <<endl;
    minMaxLoc(ck.derivyA, &min, &max);
    cout << min << "  derivy   "<< max <<endl;
    minMaxLoc(ck.mderivxA, &min, &max);
    cout << min << "  mDerivx  "<< max <<endl;
    minMaxLoc(ck.mderivyA, &min, &max);
    cout << min << "  mDerivy  "<< max <<endl;
    minMaxLoc(ck.cornersA, &min, &max);
    cout << min << "  corners   "<< max <<endl;
    ////////////debugging for assertions///////////////////////////


    //cout << ck.derivyA <<endl;
    //cout << ck.mderivxA<<endl;
    //cout << ck.mderivyA <<endl;

    // cout << ck.greyA <<endl;
    // cout << ck.greyA <<endl;

    return topPoints;
}

//-----------------------------------------------------------------------------------------------
Mat Harris::convertRgbToGrayscale(Mat& img) {
    Mat greyscaleImg(img.rows, img.cols, CV_32F);

    for (int c = 0; c < img.cols; c++) {
        for (int r = 0; r < img.rows; r++) {
            greyscaleImg.at<float>(r,c) =
            	0.2126 * img.at<cv::Vec3b>(r,c)[0] +
            	0.7152 * img.at<cv::Vec3b>(r,c)[1] +
            	0.0722 * img.at<cv::Vec3b>(r,c)[2];

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
    mdMats.Iy = gaussFilter(dMats.Iy, filterRange);
    mdMats.Ixy = gaussFilter(dMats.Ixy, filterRange);

    return mdMats;
}

//-----------------------------------------------------------------------------------------------
Derivatives Harris::applyMeanToDerivatives(Derivatives& dMats, int filterRange) {
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
            // cout << a1 << "    " << a2 << "    " << a3 << "    " <<  endl;
            // cout << a1 + a2 + a2 + a3 <<endl;

            sobelHelperH.at<float>(r,c-1) = a1 + a2 + a2 + a3;
        }
    }
    // double min, max;
    // minMaxLoc(sobelHelperH, &min, &max);
    // cout << min << "  csobelHHHHH  "<< max <<endl;
    //cout << sobelHelperH <<endl;

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

    // double min, max;
    // minMaxLoc(Iy, &min, &max);
    // cout << min << "  Iy "<< max <<endl;

    Derivatives d;
    d.Ix = Ix;
    d.Iy = Iy;
    d.Ixy = Iy;

    return d;
}

//-----------------------------------------------------------------------------------------------
Mat Harris::computeHarrisResponses(float k, Derivatives& d) {
    Mat M(d.Iy.rows, d.Ix.cols, CV_32F);

    #if ASSERTIONS_ON
    for(int r=0; r<d.Iy.rows; r++) {
        for(int c=0; c<d.Iy.cols; c++) {
            float   a11, a12,
                    a21, a22;

            a11 = d.Ix.at<float>(r,c) * d.Ix.at<float>(r,c);
            a22 = d.Iy.at<float>(r,c) * d.Iy.at<float>(r,c);
            a21 = d.Ix.at<float>(r,c) * d.Iy.at<float>(r,c);
            a12 = d.Ix.at<float>(r,c) * d.Iy.at<float>(r,c);

            float det = a11*a22 - a12*a21; //always 0 unless fault
            //cout << det << "the det issssss" <<endl;
            float trace = a11 + a22; // cant be larger than 2.1 million

            M.at<float>(r,c) = abs(det - k * trace*trace);// coud be over 4 Tera
        }
    }
      // float det, trace;
      // int count_fault =0;
      // for(int r=0; r<d.Iy.rows; r++) {
      //     for(int c=0; c<d.Iy.cols; c++) {
      //         float   a11, a12,
      //                 a21, a22;
      //
      //         a11 = d.Ix.at<float>(r,c) * d.Ix.at<float>(r,c);
      //         a22 = d.Iy.at<float>(r,c) * d.Iy.at<float>(r,c);
      //         a21 = d.Ix.at<float>(r,c) * d.Iy.at<float>(r,c);
      //         a12 = d.Ix.at<float>(r,c) * d.Iy.at<float>(r,c);
      //
      //
      //         do { // runs through loop once and checks if there is a fault
      //
      //           det = a11*a22 - a12*a21; //always 0 unless fault
      //           cout << det << "the det issssss" <<endl;
      //           trace = a11 + a22; // cant be larger than 2.1 million
      //
      //           ck.traceA = trace.clone();
      //
      //
      //           count_fault += 1;
      //           if (count_fault > 3){
      //             break;
      //           }
      //         } while (iterateFlo(ck.traceA,0,2.1*pow(10,6)) == 1);
      //
      //         M.at<float>(r,c) = abs(det - k * trace*trace);// coud be over 4 Tera
      //     }
      // }

    #else

      for(int r=0; r<d.Iy.rows; r++) {
          for(int c=0; c<d.Iy.cols; c++) {
              float   a11, a12,
                      a21, a22;

              a11 = d.Ix.at<float>(r,c) * d.Ix.at<float>(r,c);
              a22 = d.Iy.at<float>(r,c) * d.Iy.at<float>(r,c);
              a21 = d.Ix.at<float>(r,c) * d.Iy.at<float>(r,c);
              a12 = d.Ix.at<float>(r,c) * d.Iy.at<float>(r,c);

              float det = a11*a22 - a12*a21; //always 0 unless fault
              cout << det << "the det issssss" <<endl;
              float trace = a11 + a22; // cant be larger than 2.1 million

              M.at<float>(r,c) = abs(det - k * trace*trace);// coud be over 4 Tera
          }
      }

    #endif
    return M;
}

//-----------------------------------------------------------------------------------------------
Mat Harris::computeIntegralImg(Mat& img) {
    Mat integralMat(img.rows, img.cols, CV_32F);

    integralMat.at<float>(0,0) = img.at<float>(0,0);

    for (int i = 1; i < img.cols; i++) {
        integralMat.at<float>(0,i) =
            integralMat.at<float>(0,i-1)
            + img.at<float>(0,i);
    }

    for (int j = 1; j < img.rows; j++) {
        integralMat.at<float>(j,0) =
            integralMat.at<float>(j-1,0)
            + img.at<float>(j,0);
    }

    for (int i = 1; i < img.cols; i++) {
        for (int j = 1; j < img.rows; j++) {
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
Mat Harris::meanFilter(Mat& intImg, int range) {
    Mat medianFilteredMat(intImg.rows-range*2, intImg.cols-range*2, CV_32F);

    for (int r = range; r < intImg.rows-range; r++) {
        for (int c = range; c < intImg.cols-range; c++) {
            medianFilteredMat.at<float>(r-range, c-range) =
                intImg.at<float>(r+range, c+range)
                + intImg.at<float>(r-range, c-range)
                - intImg.at<float>(r+range, c-range)
                - intImg.at<float>(r-range, c+range);
        }
    }

    return medianFilteredMat;
}

Mat Harris::gaussFilter(Mat& img, int range) {
    // Helper Mats for better time complexity
    Mat gaussHelperV(img.rows-range*2, img.cols-range*2, CV_32F);
    for(int r=range; r<img.rows-range; r++) {
        for(int c=range; c<img.cols-range; c++) {
            float res = 0;

            for(int x = -range; x<=range; x++) {
                float m = 1/sqrt(2*M_PI)*exp(-0.5*x*x);

                res += m * img.at<float>(r-range,c-range);
            }

            gaussHelperV.at<float>(r-range,c-range) = res;
        }
    }

    Mat gauss(img.rows-range*2, img.cols-range*2, CV_32F);
    for(int r=range; r<img.rows-range; r++) {
        for(int c=range; c<img.cols-range; c++) {
            float res = 0;

            for(int x = -range; x<=range; x++) {
                float m = 1/sqrt(2*M_PI)*exp(-0.5*x*x);

                res += m * gaussHelperV.at<float>(r-range,c-range);
                //cout << res << endl;
            }

            gauss.at<float>(r-range,c-range) = res;
        }
    }

    return gauss;
}
