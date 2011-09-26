/**
***************************************************************************
* @file rotationsTest.cpp
* 
* Source file defining rotationsTest class.
*
* Copyright (C) 2005, 2011 David LaRose, dlr@cs.cmu.edu
* See accompanying file, LICENSE.TXT, for details.
*
***************************************************************************
**/

#include <brick/numeric/rotations.hh>

#include <brick/common/functional.hh>
#include <brick/test/testFixture.hh>

namespace brick {

  namespace numeric {

    class RotationsTest : public brick::test::TestFixture<RotationsTest> {

    public:

      RotationsTest();
      ~RotationsTest() {}

      void setUp(const std::string& /* testName */) {}
      void tearDown(const std::string& /* testName */) {}

      void testAxisAngleToQuaternion();
      void testAxisAngleToRollPitchYaw();
      void testAxisAngleToTransform3D();

      void testEulerToTransform3D();
    
      void testQuaternionToAxisAngle();
      void testQuaternionToRollPitchYaw();
      void testQuaternionToTransform3D();

      void testRollPitchYawToAxisAngle();
      void testRollPitchYawToQuaternion();
      void testRollPitchYawToTransform3D();

      void testTransform3DToAxisAngle();
      void testTransform3DToQuaternion();
      void testTransform3DToRollPitchYaw();

    private:

      void assertSimilar(Transform3D<double> const& xf0, Transform3D<double> const& xf1) const;

      double m_defaultTolerance;
    
    }; // class RotationsTest


    /* ============== Member Function Definititions ============== */

    RotationsTest::
    RotationsTest()
      : brick::test::TestFixture<RotationsTest>("RotationsTest"),
        m_defaultTolerance(1.0E-10)
    {
      // Register all tests.
      BRICK_TEST_REGISTER_MEMBER(testAxisAngleToQuaternion);
      BRICK_TEST_REGISTER_MEMBER(testAxisAngleToRollPitchYaw);
      BRICK_TEST_REGISTER_MEMBER(testAxisAngleToTransform3D);

      BRICK_TEST_REGISTER_MEMBER(testEulerToTransform3D);

      BRICK_TEST_REGISTER_MEMBER(testQuaternionToAxisAngle);
      BRICK_TEST_REGISTER_MEMBER(testQuaternionToRollPitchYaw);
      BRICK_TEST_REGISTER_MEMBER(testQuaternionToTransform3D);

      BRICK_TEST_REGISTER_MEMBER(testRollPitchYawToAxisAngle);
      BRICK_TEST_REGISTER_MEMBER(testRollPitchYawToQuaternion);
      BRICK_TEST_REGISTER_MEMBER(testRollPitchYawToTransform3D);

      BRICK_TEST_REGISTER_MEMBER(testTransform3DToAxisAngle);
      BRICK_TEST_REGISTER_MEMBER(testTransform3DToQuaternion);
      BRICK_TEST_REGISTER_MEMBER(testTransform3DToRollPitchYaw);
    }


    void
    RotationsTest::
    testAxisAngleToQuaternion()
    {
    
    }

  
    void
    RotationsTest::
    testAxisAngleToRollPitchYaw()
    {

    }

  
    void
    RotationsTest::
    testAxisAngleToTransform3D()
    {

    }

  
    void
    RotationsTest::
    testEulerToTransform3D()
    {
      // We test by comparison to the (presumably known good)
      // rollPitchYaw routine, and promise to make a more comprehensive
      // test later.
      double angle0 = 0.1;
      double angle1 = 0.2;
      double angle2 = 0.3;
      Axis axis0 = BRICK_AXIS_Z;
      Axis axis1 = BRICK_AXIS_Y;
      Axis axis2 = BRICK_AXIS_X;

      Transform3D<double> xf0 = rollPitchYawToTransform3D(
        Vector3D<double>(angle2, angle1, angle0));
      Transform3D<double> xf1 = eulerToTransform3D(
        angle0, axis0, angle1, axis1, angle2, axis2);
      Transform3D<double> xf2 = (
        eulerToTransform3D(angle2, axis2, 0.0, BRICK_AXIS_X,
                           0.0, BRICK_AXIS_X)
        * eulerToTransform3D(angle1, axis1, 0.0, BRICK_AXIS_X,
                             0.0, BRICK_AXIS_X)
        * eulerToTransform3D(angle0, axis0, 0.0, BRICK_AXIS_X,
                             0.0, BRICK_AXIS_X));
      Transform3D<double> xf3 = (
        eulerToTransform3D(0.0, BRICK_AXIS_X, 0.0, BRICK_AXIS_X,
                           angle2, axis2)
        * eulerToTransform3D(0.0, BRICK_AXIS_X, angle1, axis1,
                             0.0, BRICK_AXIS_X)
        * eulerToTransform3D(angle0, axis0, 0.0, BRICK_AXIS_X,
                             0.0, BRICK_AXIS_X));

      this->assertSimilar(xf0, xf1);
      this->assertSimilar(xf0, xf2);
      this->assertSimilar(xf0, xf3);
    }

  
    void
    RotationsTest::
    testQuaternionToAxisAngle()
    {

    }

  
    void
    RotationsTest::
    testQuaternionToRollPitchYaw()
    {

    }

  
    void
    RotationsTest::
    testQuaternionToTransform3D()
    {

    }

  
    void
    RotationsTest::
    testRollPitchYawToAxisAngle()
    {

    }

  
    void
    RotationsTest::
    testRollPitchYawToQuaternion()
    {

    }

  
    void
    RotationsTest::
    testRollPitchYawToTransform3D()
    {

    }

  
    void
    RotationsTest::
    testTransform3DToAxisAngle()
    {

    }

  
    void
    RotationsTest::
    testTransform3DToQuaternion()
    {

    }

  
    void
    RotationsTest::
    testTransform3DToRollPitchYaw()
    {

    }
  

    void
    RotationsTest::
    assertSimilar(Transform3D<double> const& xf0, Transform3D<double> const& xf1) const
    {
      for(size_t ii = 0; ii < 4; ++ii) {
        for(size_t jj = 0; jj < 4; ++jj) {
          BRICK_TEST_ASSERT(
            approximatelyEqual(xf0(jj, ii), xf1(jj, ii), m_defaultTolerance));
        }
      }
    }

  } // namespace numeric
  
} // namespace brick


#if 0

int main(int argc, char** argv)
{
  brick::numeric::RotationsTest currentTest;
  bool result = currentTest.run();
  return (result ? 0 : 1);
}

#else

namespace {

  brick::numeric::RotationsTest currentTest;

}

#endif