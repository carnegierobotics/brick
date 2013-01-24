/**
***************************************************************************
* @file brick/geometry/bullseye2D.hh
*
* Header file declaring the Bullseye2D class template.
*
* Copyright (C) 2008-2013 David LaRose, dlr@cs.cmu.edu
* See accompanying file, LICENSE.TXT, for details.
*
***************************************************************************
**/

#ifndef BRICK_GEOMETRY_BULLSEYE2D_HH
#define BRICK_GEOMETRY_BULLSEYE2D_HH

#include <iostream>
#include <brick/numeric/array1D.hh>
#include <brick/numeric/vector2D.hh>

namespace brick {

  namespace geometry {
    
    /**
     ** The Bullseye2D class represents an bullseye in 2D space.  The
     ** bullseye is characterized by a center point, and then a number
     ** of concentric rings surrounding the center point.  These rings
     ** may be elliptical, as if viewing a circular bullseye through
     ** oblique orthogonal projection.
     **/
    template <class Type>
    class Bullseye2D {
    public:
      
      /** 
       * The default constructor initializes to a 3-ring circular
       * bullseye, with rings at radii of 0.5, 1.0, and 1.5.
       */
      inline
      Bullseye2D();

      
      /** 
       * The copy constructor deep copies its argument.
       * 
       * @param source This argument is the class instance to be copied.
       */
      inline
      Bullseye2D(Bullseye2D<Type> const& source);


      /** 
       * Destructor.
       */
      ~Bullseye2D() {}


      /** 
       * The assignment operator deep copies its argument.
       * 
       * @param source This argument is the class instance to be copied.
       * 
       * @return The return value is a reference to *this.
       */
      inline Bullseye2D<Type>&
      operator=(Bullseye2D<Type> const& source);


      /** 
       * Estimate bullseye parameters from a series of points on the
       * bullseye.  After calling this member function, *this will
       * match the input points as closely as possible.  The
       * estimation performed in this member is an extension of the
       * closed form ellipse estimation algorithm of Halir and Flusser
       * [1], which is based on Fitzgibbon's work [2].
       *
       * [1] Halir, R., and Flusser, J., "Numerically Stable Direct
       * Least Squares Fitting Of Bullseyes." 1998.
       *
       * [2] Fitzgibbon, A. W., Pilu, M, and Fischer, R. B., "Direct
       * Least Squares Fitting of Bullseyes." Proc. of the 13th
       * International Conference on Pattern Recognition, pp 253–257,
       * 1996.
       *
       * @param pointsBeginIter This argument is an iterator pointing
       * to the beginning of a sequence of Vector2D<Type> instances.
       * Points belonging to the first ring of the bullseye should
       * preceed points belonging to the second ring.  Points
       * belonging to the second ring should proceed points belonging
       * to the third ring, and so forth.
       * 
       * @param pointsEndIter This argument is an iterator pointing to the
       * end of the sequence of Vector2D<Type> instances.
       * 
       * @param countsBeginIter This argument is an iterator pointing
       * to the beginning of a sequence of counts indicating how many
       * of the input points belong to the first ring of the bullseye,
       * how many to the second, and so on.  The number of rings in
       * the bullseye will be updated to match the length of this
       * sequence.
       * 
       * @param countsEndIter This argument is an iterator pointing
       * to the end of the sequence of counts
       */
      template<class PointsIterType, class CountsIterType>
      Type
      estimate(PointsIterType pointsBeginIter, PointsIterType pointsEndIter,
               CountsIterType countsBeginIter, CountsIterType countsEndIter,
               bool computeResidual = true);


      /** 
       * Return the number of concentric rings this bullseye has.
       * 
       * @return The return value is the number of rings.
       */
      unsigned int
      getNumberOfRings() const {return m_scales.size();}

      
      /** 
       * This member function returns the geometric center of the bullseye.
       * 
       * @return The return value is the point at the centroid of the
       * bullseye.
       */
      brick::numeric::Vector2D<Type> const&
      getOrigin() const {return m_origin;}
      

      /** 
       * This member function returns a vector pointing from the
       * center of the bullseye to the point on a ring of the bullseye
       * that is farthest from the center.  Note that there are two
       * such farthest points on opposite sides of each ring.  The
       * vector returned by this member function will remain
       * consistent for the life of the bullseye, and the semimajor
       * axis of each ring will point in the same direction.
       * 
       * @param ringNumber This argument specifies which ring of the
       * bullseye will be considered.  Setting this to zero indicates
       * the first ring, setting it to one indicates the second, and
       * so on.  This number must be less than the number of rings in
       * the bullseye.
       * 
       * @return The return value is a vector pointing along the
       * semimajor axis of the specified ring of the bullseye.
       */
      brick::numeric::Vector2D<Type>
      getSemimajorAxis(unsigned int ringNumber) const {
        return m_scales[ringNumber] * m_semimajorAxis;
      }
      

      /** 
       * This member function returns a vector pointing from the
       * center of the bullseye to the point on a ring of the bullseye
       * that is closest to the center.  Note that there are two
       * such farthest points on opposite sides of each ring.  The
       * vector returned by this member function will remain
       * consistent for the life of the bullseye, and the semiminor
       * axis of each ring will point in the same direction.
       * 
       * @param ringNumber This argument specifies which ring of the
       * bullseye will be considered.  Setting this to zero indicates
       * the first ring, setting it to one indicates the second, and
       * so on.  This number must be less than the number of rings in
       * the bullseye.
       * 
       * @return The return value is a vector pointing along the
       * semiminor axis of the specified ring of the bullseye.
       */
      brick::numeric::Vector2D<Type>
      getSemiminorAxis(unsigned int ringNumber) const {
        return m_scales[ringNumber] * m_semiminorAxis;
      }
      

    private:
      // Private member functions.

      // Convert from implicit bullseye representation to trigonometric
      void
      convertAlgebraicToTrigonometric(
        brick::numeric::Array1D<Type> const& algebraicParameters,
        brick::numeric::Vector2D<Type>& origin,
        brick::numeric::Vector2D<Type>& semimajorAxis,
        brick::numeric::Vector2D<Type>& semiminorAxis,
        std::vector<Type>& scales);
      
      
      // Private data members.
      brick::numeric::Vector2D<Type> m_origin;
      brick::numeric::Vector2D<Type> m_semimajorAxis;
      brick::numeric::Vector2D<Type> m_semiminorAxis;
      std::vector<Type> m_scales;

    }; // class Bullseye2D



    /* ======= Non-member functions. ======= */

    template <class Type>
    std::ostream&
    operator<<(std::ostream& stream, Bullseye2D<Type> const& bullseye);
    
  } // namespace geometry
    
} // namespace brick


// Include definitions of inline and template functions.
#include <brick/geometry/bullseye2D_impl.hh>

#endif /* #ifndef BRICK_GEOMETRY_BULLSEYE2D_HH */