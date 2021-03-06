/**
***************************************************************************
* @file brick/optimization/optimizerCommon.hh
*
* Header file declaring some utility functions for the Optimizer classes.
*
* Copyright (C) 2003-2011 David LaRose, dlr@davidlarose.com
* See accompanying file, LICENSE.TXT, for details.
*
***************************************************************************
**/

#ifndef BRICK_OPTIMIZATION_OPTIMIZERCOMMON_HH
#define BRICK_OPTIMIZATION_OPTIMIZERCOMMON_HH

#include <cmath>
#include <brick/common/mathFunctions.hh>
#include <brick/numeric/array2D.hh>

namespace brick {

  namespace optimization {

    /**
     * This function computes a scale factor for convergence tests.
     * Roughly, the bigger the elements of argument vector, relative to
     * the elements of argument point, the bigger the returned value.
     *
     * @param vector A direction vector for use in the scaling calculation.
     * @param point A starting point for use in the scaling calculation.
     * @return A scale factor, always greater than zero.
     */
    // Default function template parameters require C++11.
#if __cplusplus <= 199711L
    template <class ArgumentType, class FloatType>
#else
    template <class ArgumentType, class FloatType = double>
#endif
    FloatType
    contextSensitiveScale(const ArgumentType& vector,
                          const ArgumentType& point);


    /**
     * This function copies an argument_type array in such a way that
     * the result is a deep copy of the original, even if argument_type
     * has shallow copy semantics.
     *
     * @param source The array that will be copied.
     * @param target The array into which source will be copied.
     */
    template <class ArgumentType>
    inline void
    copyArgumentType(const ArgumentType& source, ArgumentType& target);


    /**
     * This function computes the dot product of two ArgumentType instances.
     * Note that ArgumentType is assumed to be a vector class supporting
     * operator[](size_t) for const access to its elements, and providing
     * a size() member function.  The dot product is returned as a FloatType.
     *
     * @param argument0 This argument is the first term in the dot product.
     * @param argument1 This argument is the second term in the dot product.
     * @return The sum of the products of corresponding elements of
     * the two arguments.
     */
    // Default function template parameters require C++11.
#if __cplusplus <= 199711L
    template <class ArgumentType, class FloatType>
#else
    template <class ArgumentType, class FloatType = double>
#endif
    FloatType
    dotArgumentType(const ArgumentType& argument0,
                    const ArgumentType& argument1);


    /**
     * This function computes matrix product of an Array2D<FloatType> instance
     * and an ArgumentType instance.  Note that ArgumentType is assumed to
     * be a vector class supporting operator[](size_t) for const access to
     * its elements, and providing a size() member function.  The
     * matrix*vector product is returned through the final reference
     * argument.
     *
     * @param matrix0 This argument is the first term in the product.
     * @param vector0 This argument is the second term in the product.
     * @param result The elements of this argument will be  set to the
     * result of the matrix * vector product.
     */
    // Default function template parameters require C++11.
#if __cplusplus <= 199711L
    template <class ArgumentType, class FloatType>
#else
    template <class ArgumentType, class FloatType = double>
#endif
    void
    matrixMultiplyArgumentType(const brick::numeric::Array2D<FloatType>& matrix0,
                               const ArgumentType& vector0,
                               ArgumentType& result);

  } // namespace optimization

} // namespace brick


/* ======= Declarations to maintain compatibility with legacy code. ======= */

namespace brick {

  using optimization::contextSensitiveScale;
  using optimization::copyArgumentType;
  using optimization::dotArgumentType;
  using optimization::matrixMultiplyArgumentType;

} // namespace brick


/*******************************************************************
 * Function definitions follow.  This would be a .cpp file
 * if it weren't templated.
 *******************************************************************/

#include <brick/common/exception.hh>

namespace brick {

  namespace optimization {

    template <class ArgumentType, class FloatType>
    FloatType
    contextSensitiveScale(const ArgumentType& vector,
                          const ArgumentType& point)
    {
      if(vector.size() != point.size()) {
        BRICK_THROW(brick::common::ValueException, "contextSensitiveScale",
		    "Scaling arguments have different sizes.");
      }
      FloatType chosenScale = 0.0;
      for(size_t index = 0; index < vector.size(); ++index) {
        FloatType vectorElement = brick::common::absoluteValue(vector[index]);
        FloatType pointElement = brick::common::absoluteValue(point[index]);
        if(pointElement < static_cast<FloatType>(1.0)) {
          pointElement = static_cast<FloatType>(1.0);
        }
        FloatType scaleCandidate = vectorElement / pointElement;
        if(scaleCandidate > chosenScale) {
          chosenScale = scaleCandidate;
        }
      }
      return chosenScale;
    }


// This function copies an argument_type array in such a way that
// the result is a deep copy of the original, even if argument_type
// has shallow copy semantics.
    template <class ArgumentType>
    inline void
    copyArgumentType(const ArgumentType& source, ArgumentType& target) {
      if(target.size() != source.size()) {
        target = ArgumentType(source.size());
      }
      for(size_t index = 0; index < static_cast<size_t>(source.size());
          ++index) {
        target[index] = source[index];
      }
    }


// This function computes the dot product of two ArgumentType instances.
    template <class ArgumentType, class FloatType>
    FloatType
    dotArgumentType(const ArgumentType& argument0,
                    const ArgumentType& argument1)
    {
      // Make sure arguments are of the same size
      if(argument0.size() != argument1.size()) {
        BRICK_THROW(brick::common::ValueException, "dotArgumentType()",
		    "Input arguments have different size.");
      }
      // Now compute the dot product.
      FloatType result = 0.0;
      for(size_t index = 0; index < argument0.size(); ++index){
        result += argument0[index] * argument1[index];
      }
      return result;
    }

    // This function computes matrix product of an Array2D<FloatType> instance
    // and an ArgumentType instance.
    template <class ArgumentType, class FloatType>
    void
    matrixMultiplyArgumentType(const brick::numeric::Array2D<FloatType>& matrix0,
                               const ArgumentType& vector0,
                               ArgumentType& result)
    {
      if(vector0.size() != matrix0.columns()) {
        std::ostringstream message;
        message << "Can't right-multiply a " << matrix0.rows() << " x "
                << matrix0.columns() << " matrix by a " << vector0.size()
                << " element vector.";
        BRICK_THROW(brick::common::ValueException,
		    "matrixMultiplyArgumentType(...)",
		    message.str().c_str());
      }
      if(result.size() != matrix0.rows()) {
        std::ostringstream message;
        message << "Matrix argument has " << matrix0.rows()
                << " but result has " << result.size() << " elements.";
        BRICK_THROW(brick::common::ValueException,
		    "matrixMultiplyArgumentType(...)",
		    message.str().c_str());
      }
      for(size_t row = 0; row < matrix0.rows(); ++row) {
        result[row] = 0.0;
        for(size_t column = 0; column < matrix0.columns(); ++column) {
          result[row] += matrix0(row, column) * vector0[column];
        }
      }
    }

  } // namespace optimization

} // namespace brick

#endif /* #ifndef BRICK_OPTIMIZATION_OPTIMIZERCOMMON_HH */
