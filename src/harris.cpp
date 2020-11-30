/*
 *      addapted from work done by alexanderb
 */

#include "../include/harris.h"
#include "../include/abft.h"
#include <omp.h>
#include <chrono>
using namespace std::chrono;

#if ASSERTIONS_ON
  // function to check range of each saved matrix element

  int iterateFlo(float Flo, float lB, float uB){

    if (Flo < lB || Flo > uB)
    {
      return 1; //there is a fault, so stop checking rest of matrix
    }

    return 0; //there is no fault
  }
#endif

Harris::Harris(Mat img, float k, int filterRange)
#if INJECT_FAULTS
 :
 fi(PROB_DATA, 2e-5)
#endif
{

    // (1) Convert to greyscale image
    auto t_start = high_resolution_clock::now();
    Mat greyscaleImg;
#if ABFT_ON
    img = doGrayscaleABFT(img);
    bool correct = false;
    do
    {
        stats.abft.grayscaleRuns++;
        greyscaleImg = convertRgbToGrayscale(img);
        correct = grayscaleABFTCheck(greyscaleImg, true);
    } while (!correct);

    // remove checksums from image
    greyscaleImg = cv::Mat(greyscaleImg,cv::Range(0,greyscaleImg.rows - 1), cv::Range(0,greyscaleImg.cols - 1));

#elif THREADS_ON
    img.convertTo(img, CV_32FC3);
    greyscaleImg = runParallel_convertRgbToGrayscale(img);
#else
    img.convertTo(img, CV_32FC3);
    greyscaleImg = convertRgbToGrayscale(img);
#endif

    auto t_stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(t_stop - t_start);
#if DATA_COLLECTION_MODE
    stats.timing.greyscale = duration.count();
#else
    cout << "Time to convert to greyscale image: " << duration.count() / 1000 << " ms" << endl;
#endif

    // (2) Compute Derivatives
    t_start = high_resolution_clock::now();
#if ABFT_ON
    /*Mat a, b;
    abft_addChecksums(greyscaleImg,a,b);
    bool valid = false;
    do
    {
        // TODO
        valid = abft_check(greyscaleImg,a,b,true);
    } while (!valid);*/
    Derivatives derivatives = computeDerivatives(greyscaleImg);
#elif THREADS_ON
    Derivatives derivatives = runParallel_computeDerivatives(greyscaleImg);
#else
    Derivatives derivatives = computeDerivatives(greyscaleImg);
#endif

#if ABFT_ON
    //generate checksums for Derivatives
    Mat IxC, IxR, IyC, IyR, IxyC, IxyR;
    Mat IxC_gold, IxR_gold, IyC_gold, IyR_gold, IxyC_gold, IxyR_gold;
    abft_addChecksums(derivatives.Ix, IxR_gold, IxC_gold);
    abft_addChecksums(derivatives.Iy, IyR_gold, IyC_gold);
    abft_addChecksums(derivatives.Ixy, IxyR_gold, IxyC_gold);
    Derivatives d_gold = derivatives;

    do
    {
        stats.abft.derivativeRuns++;

        // TODO: update so this isn't in timing results
        IxC = IxC_gold;
        IyC = IyC_gold;
        IxyC = IxyC_gold;
        IxR = IxR_gold;
        IyR = IyR_gold;
        IxyR = IxyR_gold;
        derivatives = d_gold;
        #if INJECT_FAULTS
            fi.setBHP(1.2e-5);
            fi.inject(derivatives.Ix);
            fi.inject(IxR);
            fi.inject(IxC);
            fi.setBHP(6e-6);
            fi.inject(derivatives.Iy);
            fi.inject(IyR);
            fi.inject(IyC);
            fi.setBHP(3e-6);
            fi.inject(derivatives.Ixy);
            fi.inject(IxyR);
            fi.inject(IxyC);
        #endif

    }while (!abft_check(derivatives.Ix, IxR, IxC, true, stats) || \
            !abft_check(derivatives.Iy, IyR, IyC, true, stats) || \
            !abft_check(derivatives.Ixy, IxyR, IxyC, true, stats));
#else
    #if INJECT_FAULTS
        fi.setBHP(1.2e-5);
        fi.inject(derivatives.Ix);
        fi.setBHP(6e-6);
        fi.inject(derivatives.Iy);
        fi.setBHP(3e-6);
        fi.inject(derivatives.Ixy);
    #endif
#endif
    t_stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(t_stop - t_start);
#if DATA_COLLECTION_MODE
    stats.timing.derivatives = duration.count();
#else
    cout << "Time to compute derivatives: " << duration.count() / 1000 << " ms" << endl;
#endif

    // (3) Gaussian Filtering
    t_start = high_resolution_clock::now();
#if THREADS_ON
    Derivatives mDerivatives = runParallel_applyToDerivatives(derivatives, filterRange);
#else
    Derivatives mDerivatives = applyGaussToDerivatives(derivatives, filterRange);

#endif
    t_stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(t_stop - t_start);
#if DATA_COLLECTION_MODE
    stats.timing.filtering = duration.count();
#else
    cout << "Time to perform median filtering: " << duration.count() / 1000 << " ms" << endl;
#endif


    // (4) Compute Harris Response
    t_start = high_resolution_clock::now();

#if THREADS_ON
    Mat harrisResponses = runParallel_computeHarrisResponses(k, mDerivatives);
    m_harrisResponses = harrisResponses;
#elif ABFT_ON
    Mat harrisResponses = computeHarrisResponses(k, mDerivatives);
    m_harrisResponses = harrisResponses;
    // check created here, verified when get Maxima Points is called
    abft_addChecksums(m_harrisResponses,hrRc,hrCc);
#else
    Mat harrisResponses = computeHarrisResponses(k, mDerivatives);
    m_harrisResponses = harrisResponses;
#endif

    t_stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(t_stop - t_start);

#if DATA_COLLECTION_MODE
    stats.timing.response = duration.count();
#else
    cout << "Time to compute Harris responses: " << duration.count() / 1000 << " ms" << endl;
#endif

}

//-----------------------------------------------------------------------------------------------
vector<pointData> Harris::getMaximaPoints(float percentage, int filterRange, int suppressionRadius)
{
    // Declare a max suppression matrix
    bool maxSuppresionMat[m_harrisResponses.rows][m_harrisResponses.cols];
    for (int r = 0; r < m_harrisResponses.rows; ++r)
    {
        for (int c = 0; c < m_harrisResponses.cols; ++c)
        {
            maxSuppresionMat[r][c] = false;
        }
    }

    Mat m_harrisResponses_gold = m_harrisResponses.clone();
    #if ABFT_ON
        Mat hrRc_gold = hrRc;
        Mat hrCc_gold = hrCc;
    #endif
    
    std::vector<pointData> points; // Create a vector of all Points
    bool valid; // used by abft to flag restart
    do
    {
        #if ABFT_ON
            hrRc = hrRc_gold;
            hrCc = hrCc_gold;
        #endif
        m_harrisResponses = m_harrisResponses_gold.clone();
        points.clear();
        valid = true;
        for (int r = 0; r < m_harrisResponses.rows; r++) {
            #if ABFT_ON
                // perform continual verification during this critical part
                if (r % 50 == 0)
                {
                    if (!abft_check(m_harrisResponses,hrRc,hrCc,true,2000, stats))
                    {
                        // corrupted, go back to begining
                        valid = false;
                        stats.abft.responseLoopResets++;
                        break;
                    }
                }
            #endif
            for (int c = 0; c < m_harrisResponses.cols; c++) {
                if (m_harrisResponses.at<float>(r,c) > .5) // set corner response threshold
                {
                    Point p(r,c);

                    pointData d;
                    d.cornerResponse = m_harrisResponses.at<float>(r,c);
                    d.point = p;

                    points.push_back(d);
                }
            }
            #if INJECT_FAULTS
                if (r%100 == 0) // too slow to do every time
                {
                    fi.setBHP(2e-6);
                    fi.inject(m_harrisResponses);
                }
            #endif
        }
    }while (!valid);

    // Sort points by corner Response
    sort(points.begin(), points.end(), by_cornerResponse());

    int numberTopPoints = points.size();
    std::vector<pointData> topPoints;
    int supRows = m_harrisResponses.rows - 1;
    int supCols = m_harrisResponses.cols - 1;

    for (int i = 0; i < numberTopPoints; ++i)
    {
        // Check if point marked in maximaSuppression matrix
        if(maxSuppresionMat[points[i].point.x][points[i].point.y] == 0)
        {
            for (int r = -suppressionRadius; r <= suppressionRadius; r++)
            {
                for (int c = -suppressionRadius; c <= suppressionRadius; c++)
                {

                    int sx = points[i].point.x+c;
                    int sy = points[i].point.y+r;

                    // bound checking
                    if (sx > supRows)
                        sx = supRows;
                    if (sx < 0)
                        sx = 0;
                    if (sy > supCols)
                        sy = supCols;
                    if (sy < 0)
                        sy = 0;

                    maxSuppresionMat[sx][sy] = 1;
                }
            }
            // Convert back to original image coordinate system
            points[i].point.x += 1 + filterRange;
            points[i].point.y += 1 + filterRange;
            topPoints.push_back(points[i]);
        }

    }
    return topPoints;
}

//-----------------------------------------------------------------------------------------------
Mat Harris::convertRgbToGrayscale(Mat &img)
{
    Mat greyscaleImg(img.rows, img.cols, CV_32F);

    #if ASSERTIONS_ON
      int reset;
      reset = 0;
    #endif
    for (int c = 0; c < img.cols; c++) {
        for (int r = 0; r < img.rows; r++) {
            greyscaleImg.at<float>(r,c) =
            	0.2126 * img.at<cv::Vec3f>(r,c)[0] +
            	0.7152 * img.at<cv::Vec3f>(r,c)[1] +
            	0.0722 * img.at<cv::Vec3f>(r,c)[2];
            greyscaleImg.at<float>(r,c) /= 255;

            #if INJECT_FAULTS
                fi.setBHP(1.65e-5);
                fi.inject(greyscaleImg.at<float>(r,c));
            #endif

            #if ASSERTIONS_ON
            //ck 1
              if (iterateFlo(greyscaleImg.at<float>(r,c),0,1) == 1 && reset < 3)
              {
                //error, so reset loop
                r =0;
                c =0;
                reset+=1;
              }
            #endif
        }
    }
    return greyscaleImg;
}

//-----------------------------------------------------------------------------------------------
Derivatives Harris::applyGaussToDerivatives(Derivatives &dMats, int filterRange)
{
    if (filterRange == 0)
        return dMats;

    Derivatives mdMats;
    Derivatives mdMats_gold;


    mdMats_gold.Ix = gaussFilter(dMats.Ix, filterRange);
#if ABFT_ON // protect after generation
    Mat IxRc_gold, IxCc_gold, IxRc, IxCc;
    abft_addChecksums(mdMats_gold.Ix, IxRc_gold, IxCc_gold);
#endif

    mdMats_gold.Iy = gaussFilter(dMats.Iy, filterRange);
#if ABFT_ON // protect after generation
    Mat IyRc_gold, IyCc_gold, IyRc, IyCc;
    abft_addChecksums(mdMats_gold.Iy, IyRc_gold, IyCc_gold);
#endif

    mdMats_gold.Ixy = gaussFilter(dMats.Ixy, filterRange);
#if ABFT_ON // protect after generation
    Mat IxyRc_gold, IxyCc_gold, IxyRc, IxyCc;
    abft_addChecksums(mdMats_gold.Ixy, IxyRc_gold, IxyCc_gold);
#endif

#if ABFT_ON
    // validate ABFT before returning
    do
    {
        stats.abft.gDerivativeRuns++;

        mdMats = mdMats_gold;
        // TODO: update so this isn't in timing results
        IxCc = IxCc_gold;
        IyCc = IyCc_gold;
        IxyCc = IxyCc_gold;
        IxRc = IxRc_gold;
        IyRc = IyRc_gold;
        IxyRc = IxyRc_gold;

        #if INJECT_FAULTS
            fi.setBHP(1.2e-5);
            fi.inject(mdMats.Ix);
            fi.inject(IxCc);
            fi.inject(IxRc);
            fi.setBHP(6e-6);
            fi.inject(mdMats.Iy);
            fi.inject(IyCc);
            fi.inject(IyRc);
            fi.setBHP(3e-6);
            fi.inject(mdMats.Ixy);
        #endif
    } while (!abft_check(mdMats.Ix,IxRc,IxCc, true, stats) || \
                !abft_check(mdMats.Iy,IyRc,IyCc, true, stats) || \
                !abft_check(mdMats.Ixy,IxyRc,IxyCc, true, stats));
#else
    mdMats = mdMats_gold;
    #if INJECT_FAULTS
        fi.setBHP(1.2e-5);
        fi.inject(mdMats.Ix);
        fi.setBHP(6e-6);
        fi.inject(mdMats.Iy);
        fi.setBHP(3e-6);
        fi.inject(mdMats.Ixy);
    #endif
#endif
    return mdMats;
}

//-----------------------------------------------------------------------------------------------
Derivatives Harris::computeDerivatives(Mat &greyscaleImg)
{
    // Convolution
    Mat sobelHelperV(greyscaleImg.rows-2, greyscaleImg.cols, CV_32F);
    Mat sobelHelperH(greyscaleImg.rows, greyscaleImg.cols-2, CV_32F);


    // Vertical
    #if ASSERTIONS_ON
      int reset;
      reset = 0;
    #endif
  // do { // runs through loop once and checks if there is a fault
    for(int r=1; r<greyscaleImg.rows-1; r++) {
        for(int c=0; c<greyscaleImg.cols; c++) {

            float a1 = greyscaleImg.at<float>(r - 1, c);
            float a2 = greyscaleImg.at<float>(r, c);
            float a3 = greyscaleImg.at<float>(r + 1, c);

            sobelHelperV.at<float>(r - 1, c) = a1 + a2 + a2 + a3;
            #if ASSERTIONS_ON
            //ck 2
              if (iterateFlo(sobelHelperV.at<float>(r-1,c),0,4) == 1 && reset < 3)
              {
                //error, so reset loop
                r =1;
                c =0;
                reset+=1;
              }
            #endif
        }
    }
    // Horizontal
    #if ASSERTIONS_ON
      reset = 0;
    #endif
    for (int r = 0; r < greyscaleImg.rows; r++)
    {
        for (int c = 1; c < greyscaleImg.cols - 1; c++)
        {

            float a1 = greyscaleImg.at<float>(r, c - 1);
            float a2 = greyscaleImg.at<float>(r, c);
            float a3 = greyscaleImg.at<float>(r, c + 1);

            sobelHelperH.at<float>(r, c - 1) = a1 + a2 + a2 + a3;
            #if ASSERTIONS_ON
            //ck 3
              if (iterateFlo(sobelHelperH.at<float>(r,c-1),0,4) == 1 && reset < 3 )
              {
                //error, so reset loop
                r =0;
                c =1;
                reset+=1;
              }
            #endif
        }
    }

    // Apply Sobel filter to compute 1st derivatives
    Mat Ix(greyscaleImg.rows - 2, greyscaleImg.cols - 2, CV_32F);
    Mat Iy(greyscaleImg.rows - 2, greyscaleImg.cols - 2, CV_32F);
    Mat Ixy(greyscaleImg.rows - 2, greyscaleImg.cols - 2, CV_32F);

    for (int r = 0; r < greyscaleImg.rows - 2; r++)
    {
        for (int c = 0; c < greyscaleImg.cols - 2; c++)
        {
            Ix.at<float>(r, c) = sobelHelperH.at<float>(r, c) - sobelHelperH.at<float>(r + 2, c);
            Iy.at<float>(r, c) = -sobelHelperV.at<float>(r, c) + sobelHelperV.at<float>(r, c + 2);
            Ixy.at<float>(r, c) = Ix.at<float>(r, c) * Iy.at<float>(r, c);
        }
    }

    Derivatives d;
    d.Ix = Ix;
    d.Iy = Iy;
    d.Ixy = Iy;

    return d;
}

//-----------------------------------------------------------------------------------------------
Mat Harris::computeHarrisResponses(float k, Derivatives &d)
{
    Mat M(d.Iy.rows, d.Ix.cols, CV_32F);

    #if ASSERTIONS_ON
      int reset,reset11, reset22, reset21, reset12;
      reset = 0;
      reset11 = 0;
      reset22 = 0;
      reset21 = 0;
      reset12 = 0;
    #endif
    float det, trace;
      for(int r=0; r<d.Iy.rows; r++) {
          for(int c=0; c<d.Iy.cols; c++) {
              float   a11, a12,
                      a21, a22;


              a11 = d.Ix.at<float>(r,c) * d.Ix.at<float>(r,c);
              a22 = d.Iy.at<float>(r,c) * d.Iy.at<float>(r,c);
              a21 = d.Ix.at<float>(r,c) * d.Iy.at<float>(r,c);
              a12 = d.Ix.at<float>(r,c) * d.Iy.at<float>(r,c);


              #if ASSERTIONS_ON
                //ck 7
                int count_f = 0;
                do { // runs through loop once and checks if there is a fault

                  det = a11*a22 - a12*a21; //always 0 unless fault
                  trace = a11 + a22; // cant be larger than 1024
                  count_f += 1;
                  if (count_f > 3){
                    break;
                  }
                } while (iterateFlo(trace,0,1024) == 1);

              #else
                det = a11*a22 - a12*a21;
                trace = a11 + a22;
              #endif


              M.at<float>(r,c) = abs(det - k * trace*trace);

              #if INJECT_FAULTS
                fi.setBHP(2e-5);
                fi.inject(M.at<float>(r,c));
              #endif
          }
      }
    return M;
}

Mat Harris::gaussFilter(Mat& img, int range) {
    Mat m(1,2*range+1, CV_32F); // gaussian Kernel
    Mat m_gold(1,2*range+1, CV_32F); // gaussian Kernel
    for (int i = -range; i<= range; ++i)
    {
        float val = 1/sqrt(2*M_PI)*exp(-0.5*i*i);
        m_gold.at<float>(0,i+range) = val;
    }
    #if ABFT_ON
        Mat mRcheck, mCcheck, mRcheck_gold, mCcheck_gold;
        //float mRcheck, mCcheck, mRcheck_gold, mCcheck_gold;
        abft_addChecksums(m_gold,mRcheck_gold,mCcheck_gold);
    #endif


    // Helper Mats for better time complexity
    Mat gaussHelperV(img.rows-range*2, img.cols-range*2, CV_32F);
    #if ASSERTIONS_ON
      int reset;
      reset = 0;
    #endif
    bool valid;
    do
    {
        #if ABFT_ON
            mCcheck = mCcheck_gold.clone();
            mRcheck = mRcheck_gold.clone();
            //mCcheck = mCcheck_gold;
            //mRcheck = mRcheck_gold;
        #endif 
        m = m_gold.clone();
        valid = true;
        for(int r=range; r<img.rows-range; r++)
        {
            #if ABFT_ON
                
                bool kernel_good = abft_check(m,mRcheck,mCcheck, false, stats);
                //bool kernel_good = abft_check(m,mRcheck,mCcheck);
                if (!kernel_good)
                {
                    //cout<<"ABFT: GAUSS LOOP1 RESET"<<endl;
                    stats.abft.gaussLoopResets++;
                    valid = false;
                    break;
                }
            #endif
            for(int c=range; c<img.cols-range; c++)
            {
                float res = 0;

                for(int x = -range; x<=range; x++)
                 {
                    res += m.at<float>(0,x+3) * img.at<float>(r-range,c-range);
                }

                gaussHelperV.at<float>(r-range,c-range) = res;
                #if ASSERTIONS_ON
                //ck 4
                  if (iterateFlo(gaussHelperV.at<float>(r-range,c-range),-4,4) == 1 && reset < 3)
                  {
                    //error, so reset loop
                    r =range;
                    c =range;
                    reset+=1;
                  }
                #endif

            }
            #if INJECT_FAULTS
                // TODO : reset kernel every 20-25 cycles to simulate transient faults
                fi.setBHP(2e-5); // TODO: update bhp
                fi.inject(m);
                #if ABFT_ON
                    fi.inject(mRcheck);
                    fi.inject(mCcheck);
                #endif
                //cout<<"injecting faults into m in gauss filter"<<endl;
            #endif
        }
    } while(!valid);

    Mat gauss(img.rows-range*2, img.cols-range*2, CV_32F);
    #if ASSERTIONS_ON
      reset = 0;
    #endif
    do
    {   
        m = m_gold.clone();
        #if ABFT_ON
            mCcheck = mCcheck_gold.clone();
            mRcheck = mRcheck_gold.clone();
            //mCcheck = mCcheck_gold;
            //mRcheck = mRcheck_gold;
        #endif
        valid = true;
        for(int r=range; r<img.rows-range; r++)
        {
            #if INJECT_FAULTS
                if (r%115 == 0)
                {
                    // reset m to simulate transient faults
                    m = m_gold.clone();
                    #if ABFT_ON
                        mCcheck = mCcheck_gold.clone();
                        mRcheck = mRcheck_gold.clone();
                    #endif
                }
            #endif
            #if ABFT_ON
                    bool kernel_good = abft_check(m,mRcheck,mCcheck, false, stats);
                    //bool kernel_good = abft_check(m,mRcheck,mCcheck);
                    if (!kernel_good)
                    {
                        //cout<<"ABFT: GAUSS LOOP2 RESET"<<endl;
                        stats.abft.gaussLoopResets++;
                        valid = false;
                        break;
                    }
            #endif
            for(int c=range; c<img.cols-range; c++)
             {
                float res = 0;
                for(int x = -range; x<=range; x++)
                {
                    res += m.at<float>(0,x+3) * gaussHelperV.at<float>(r-range,c-range);
                }

                gauss.at<float>(r-range,c-range) = res;
                #if ASSERTIONS_ON
                //ck 5
                  if (iterateFlo(gauss.at<float>(r-range,c-range),-4,4) == 1 && reset < 3)
                  {
                    //error, so reset loop
                    r = range;
                    c = range;
                    reset+=1;
                  }
                #endif


            }
            #if INJECT_FAULTS
               // cout<<"injecting faults into m in gauss filter (2)"<<endl;
                fi.setBHP(2e-5); // TODO: update bhp
                #if ABFT_ON
                    fi.inject(mRcheck);
                    fi.inject(mCcheck);
                #endif
                fi.inject(m);
            #endif
        }
    } while (!valid);
    return gauss;
}

#if THREADS_ON

bool Harris::withinPixelDiffTolerance(Mat mat1, Mat mat2)
{
    Mat diff;

    compare(mat1, mat2, diff, CMP_NE);

    int num_diff = countNonZero(diff);
    // cout << "num diff pixels: " << num_diff << "\n";

    return (countNonZero(diff) <= THREADS_NUM_FAULTS_TOLERATED);
}

Mat Harris::runParallel_convertRgbToGrayscale(Mat& img)
{
    // flag for error detection
    bool match = false;
    Mat greyscaleImg, greyscaleImg2;

    while (!match)
    {
#pragma omp parallel num_threads(2)
        {
            int i = omp_get_thread_num();
            if (i == 0)
            {
                greyscaleImg = convertRgbToGrayscale(img);
            }
            if (i == 1 || omp_get_num_threads() != 2)
            {
                greyscaleImg2 = convertRgbToGrayscale(img);
            }
        }

        // verify that they match within THREADS_NUM_FAULTS_TOLERATED pixels
        match = withinPixelDiffTolerance(greyscaleImg2, greyscaleImg);
        // cout << "Grayscale match: " << boolalpha << match << endl;
    }

    return greyscaleImg;
}

Derivatives Harris::runParallel_computeDerivatives(Mat& greyscaleImg)
{
    Derivatives derivatives, derivatives2;
    Mat greyscaleImg1, greyscaleImg2;
    bool match = false;

    while (!match)
    {
        // If threads don't match, reset greyscaleImgs for next run
        // (computeDerivatives() modifies greyscaleImg)
        greyscaleImg1 = greyscaleImg.clone();
        greyscaleImg2 = greyscaleImg.clone();
#pragma omp parallel num_threads(2)
        {
            int i = omp_get_thread_num();
            if (i == 0)
            {
                derivatives = computeDerivatives(greyscaleImg1);
            }

            if (i == 1 || omp_get_num_threads() != 2)
            {
                derivatives2 = computeDerivatives(greyscaleImg2);
            }
        }

        //verify match within THREADS_NUM_FAULTS_TOLERATED pixels
        bool matchX = withinPixelDiffTolerance(derivatives2.Ix, derivatives.Ix);
        bool matchY = withinPixelDiffTolerance(derivatives2.Iy, derivatives.Iy);
        bool matchXY = withinPixelDiffTolerance(derivatives2.Ixy, derivatives.Ixy);

        match = matchX & matchY & matchXY;

        // cout << "Derivatives match: " << boolalpha << match << endl;
    }
    // set greyscaleImg equal to one of the results
    greyscaleImg = greyscaleImg1.clone();
    return derivatives;
}

Derivatives Harris::runParallel_applyToDerivatives(Derivatives& derivatives, int filterRange)
{
    Derivatives mDerivatives, mDerivatives2;
    bool match = false;

    while (!match)
    {
#pragma omp parallel num_threads(2)
        {
            int i = omp_get_thread_num();
            if (i == 0)
            {
                mDerivatives = applyGaussToDerivatives(derivatives, filterRange);
            }
            if (i == 1 || omp_get_num_threads() != 2)
            {
                mDerivatives2 = applyGaussToDerivatives(derivatives, filterRange);
            }
        }

        //verify match
        bool mMatchX = withinPixelDiffTolerance(mDerivatives2.Ix, mDerivatives.Ix);
        bool mMatchY = withinPixelDiffTolerance(mDerivatives2.Iy, mDerivatives.Iy);
        bool mMatchXY = withinPixelDiffTolerance(mDerivatives2.Ixy, mDerivatives.Ixy);

        match = mMatchX & mMatchY & mMatchXY;

        // cout << "M Derivatives match: " << boolalpha << match << endl;
    }
    return mDerivatives;
}

Mat Harris::runParallel_computeHarrisResponses(float k, Derivatives& mDerivatives)
{
    Mat harrisResponses, harrisResponses2;
    bool match = false;
    while (!match)
    {
#pragma omp parallel num_threads(2)
        {
            int i = omp_get_thread_num();
            if (i == 0)
            {
                harrisResponses = computeHarrisResponses(k,mDerivatives);
            }
            if (i == 1 || omp_get_num_threads() != 2)
            {
                harrisResponses2 = computeHarrisResponses(k,mDerivatives);
            }
        }
        match = withinPixelDiffTolerance(harrisResponses2, harrisResponses);
    
        // cout << "Harris Responses match: " << boolalpha << match << endl;

        m_harrisResponses = harrisResponses;
    }

    return harrisResponses;
}
#endif
