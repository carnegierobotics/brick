/**
***************************************************************************
* @file brick/iso12233/test/iso12233Test.cc
*
* Source file defining tests for the MTF calculating routines.
*
* Copyright (C) 2017 David LaRose, dlr@cs.cmu.edu
* See accompanying file, LICENSE.TXT, for details.
*
***************************************************************************
**/

#include <brick/iso12233/iso12233.hh>

#include <brick/computerVision/image.hh>
#include <brick/numeric/bilinearInterpolator.hh>
#include <brick/numeric/convolve1D.hh>
#include <brick/numeric/transform2D.hh>
#include <brick/test/testFixture.hh>
#include <brick/utilities/timeUtilities.hh>

using brick::computerVision::Image;
using brick::computerVision::GRAY8;

namespace brick {

  namespace iso12233 {

    class Iso12233Test
      : public brick::test::TestFixture<Iso12233Test> {

    public:

      Iso12233Test();
      ~Iso12233Test() {}

      void setUp(const std::string& /* testName */) {}
      void tearDown(const std::string& /* testName */) {}

      // Tests.
      void testLowPassEdge();
      void testVerticalEdge();

    private:

      Image<brick::computerVision::GRAY8>
      buildSlantedEdgeImage(Array1D<double>& referenceMtf,
                            std::size_t patchHeight, std::size_t patchWidth,
                            std::size_t windowSize, double kernelSigma);

      Array1D<double> getFFTMagnitude(Array1D<double> const& kernel,
                                      std::size_t signalSize);

      double m_defaultTolerance;

    }; // class Iso12233Test


    /* ============== Member Function Definititions ============== */

    Iso12233Test::
    Iso12233Test()
      : brick::test::TestFixture<Iso12233Test>("Iso12233Test"),
        m_defaultTolerance(1.0E-8)
    {
      BRICK_TEST_REGISTER_MEMBER(testLowPassEdge);
      BRICK_TEST_REGISTER_MEMBER(testVerticalEdge);
    }


    void
    Iso12233Test::
    testLowPassEdge()
    {
      constexpr std::size_t patchWidth = 128;
      constexpr std::size_t patchHeight = 100;
      constexpr std::size_t windowSize = 64;
      constexpr double kernelSigma = 0.26;
      constexpr double tolerance = 0.03;
      constexpr double noiseFloor = 0.1;

      Array1D<double> referenceMtf;
      Image<brick::computerVision::GRAY8> rotatedImage =
        this->buildSlantedEdgeImage(referenceMtf, patchHeight, patchWidth,
                                    windowSize, kernelSigma);

      // Try to process the image.
      Array1D<double> mtf = iso12233<double>(rotatedImage, windowSize,
                                             [](double arg){return arg;});

      // Expect that the two are similar within 2.5% of full scale.
      BRICK_TEST_ASSERT(mtf.size() == referenceMtf.size());
      for(std::size_t ii = 0; ii < mtf.size() / 2; ++ii) {
        BRICK_TEST_ASSERT(mtf[ii] >= 0.0);
        BRICK_TEST_ASSERT(mtf[ii] <= 1.0);

        // We require mtf to track the expected value closely (within
        // 3% of full scale), but noise tends to get big in the higher
        // frequencies, so we simply require those values to be less
        // than smaller than our arbitrarily chosen noise floor..
        if(referenceMtf[ii] > noiseFloor || mtf[ii] > noiseFloor) {
          BRICK_TEST_ASSERT(std::fabs(mtf[ii] - referenceMtf[ii]) < tolerance);
        }
      }
    }


    void
    Iso12233Test::
    testVerticalEdge()
    {
      constexpr std::size_t patchWidth = 100;
      constexpr std::size_t patchHeight = 100;
      constexpr std::size_t windowSize = 50;

      // Create a test image with a vertically straight up and down
      // edge.  We should have trouble with this image because the
      // vertical edge doesn't let us so super-resolution.
      Image<brick::computerVision::GRAY8> edgeImage(
        patchHeight, patchWidth); // Rows, columns.
      for(std::size_t rr = 0; rr < patchHeight; ++rr) {
        std::size_t cc = 0;
        while(cc < ((patchWidth / 2) - (windowSize / 4))) {
          edgeImage(rr, cc) = 100;
          ++cc;
        }
        while(cc < patchWidth) {
          edgeImage(rr, cc) = 200;
          ++cc;
        }
      }

      // Try to process the image.
      Array1D<double> mtf;
      BRICK_TEST_ASSERT_EXCEPTION(
        brick::common::ValueException,
        mtf = iso12233<double>(edgeImage, windowSize,
                               [](double arg){return arg;}));
    }


    Image<brick::computerVision::GRAY8>
    Iso12233Test::
    buildSlantedEdgeImage(Array1D<double>& referenceMtf,
                          std::size_t patchHeight, std::size_t patchWidth,
                          std::size_t windowSize, double kernelSigma)
    {
      // We'd like the slanted edge to slant at about 15 degrees.
      // Note that tan(14.036deg) is 0.25, which is to convenient to
      // pass up.  We'll oversample by a factor of four, and simply
      // shift one (oversampled) pixel for each row in the output
      // image.

      // Create a single row with a dark-to-light transition.
      int constexpr oversampleFactor = 4;
      constexpr double darkColor = 100.0;
      constexpr double lightColor = 200.0;
      std::size_t const workingWidth = oversampleFactor * patchWidth;
      Array1D<double> prototypeRow(workingWidth);
      {
        std::size_t cc = 0;
        while(cc < (workingWidth / 2)) {
          prototypeRow[cc] = darkColor;
          ++cc;
        }
        while(cc < workingWidth) {
          prototypeRow[cc] = lightColor;
          ++cc;
        }
      }

      // Low-pass filter the input row.
      Array1D<double> kernel = brick::numeric::getGaussian1D<double>(
        oversampleFactor * kernelSigma);
      kernel /= brick::numeric::sum<double>(kernel);
      Array1D<double> blurredRow = brick::numeric::convolve1D<double>(
        kernel, prototypeRow, brick::numeric::BRICK_CONVOLVE_REFLECT_SIGNAL);

      // Create a test image that samples from this blurred row.
      // Because the kernel is a low-pass filter, we don't worry about
      // aliasing here.
      Image<GRAY8> edgeImage(patchHeight, patchWidth);
      for(std::size_t rr = 0; rr < patchHeight; ++rr) {
        int ii = rr - static_cast<int>(patchHeight / 2);
        for(std::size_t cc = 0; cc < patchWidth; ++cc) {
          int inputIndex = std::max(ii, 0);
          inputIndex = std::min(
            inputIndex, static_cast<int>(blurredRow.size() - 1));
          edgeImage(rr, cc) =
            static_cast<uint8_t>(blurredRow[inputIndex] + 0.5);
          ii += oversampleFactor;
        }
      }

      // Now we have our slanted edge image.  Figure out what the MTF
      // of the filter is.
      Array1D<double> fftMagnitude = getFFTMagnitude(kernel, 4 * windowSize);

      // Note that we have to subsample this, too.  We'll use a simple
      // binomial filter.
      referenceMtf.reinitIfNecessary(windowSize);
      referenceMtf[0] = (6.0 * fftMagnitude[0]
                         + 8.0 * fftMagnitude[1]
                         + 2.0 * fftMagnitude[2]) / 16.0;
      for(std::size_t ii = 1; ii < windowSize; ++ii) {
        int jj = 4 * ii;
        referenceMtf[ii] = (fftMagnitude[jj - 2]
                            + 4.0 * fftMagnitude[jj - 1]
                            + 6.0 * fftMagnitude[jj]
                            + 4.0 * fftMagnitude[jj + 1]
                            + fftMagnitude[jj + 2]) / 16.0;
      }
      referenceMtf /= referenceMtf[0];

      return edgeImage;
    }


    Array1D<double>
    Iso12233Test::
    getFFTMagnitude(Array1D<double> const& kernel,
                    std::size_t signalSize)
    {
      if(signalSize < kernel.size()) {
        BRICK_THROW(brick::common::ValueException,
                    "Iso12233Test::getFFTMagnitude()",
                    "Kernel size exceeds signal size.");
      }

      Array1D<std::complex<double>> signal(signalSize);
      std::size_t ii = 0;
      while(ii < kernel.size()) {
        signal[ii] = std::complex<double>(kernel[ii], 0.0);
        ++ii;
      }
      while(ii < signalSize) {
        signal[ii] = std::complex<double>(0.0, 0.0);
        ++ii;
      }

      Array1D<std::complex<double>> fft = brick::numeric::computeFFT(signal);
      Array1D<double> fftMagnitude(signalSize);

      ii = 0;
      while(ii < signalSize) {
        fftMagnitude[ii] = std::abs(fft[ii]);
        ++ii;
      }
      return fftMagnitude;
    }
    
  } // namespace iso12233

} // namespace brick


#if 0

int main(/* int argc, char** argv */)
{
  brick::iso12233::Iso12233Test currentTest;
  bool result = currentTest.run();

  currentTest.testLowPassEdge();

  return (result ? 0 : 1);
}

#else

namespace {

  brick::iso12233::Iso12233Test currentTest;

}

#endif
