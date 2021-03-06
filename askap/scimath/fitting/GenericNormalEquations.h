/// @file
/// @brief Normal equations without any approximation
/// @details There are two kinds of normal equations currently supported. The
/// first one is a generic case, where the full normal matrix is retained. It
/// is used for calibration. The second one is intended for imaging, where we
/// can't afford to keep the whole normal matrix. In the latter approach, the 
/// matrix is approximated by a sum of diagonal and shift invariant matrices. 
/// This class represents the generic case, where no approximation to the normal
/// matrix is done.
///
/// @copyright (c) 2007 CSIRO
/// Australia Telescope National Facility (ATNF)
/// Commonwealth Scientific and Industrial Research Organisation (CSIRO)
/// PO Box 76, Epping NSW 1710, Australia
/// atnf-enquiries@csiro.au
///
/// This file is part of the ASKAP software distribution.
///
/// The ASKAP software distribution is free software: you can redistribute it
/// and/or modify it under the terms of the GNU General Public License as
/// published by the Free Software Foundation; either version 2 of the License,
/// or (at your option) any later version.
///
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with this program; if not, write to the Free Software
/// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
///
/// @author Max Voronkov <maxim.voronkov@csiro.au>
/// @author Vitaliy Ogarko <vogarko@gmail.com>
///

#ifndef GENERIC_NORMAL_EQUATIONS_H
#define GENERIC_NORMAL_EQUATIONS_H

// casa includes
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Array.h>

// own includes
#include <askap/scimath/fitting/INormalEquations.h>
#include <askap/scimath/fitting/ComplexDiffMatrix.h>
#include <askap/scimath/fitting/PolXProducts.h>
#include <askap/scimath/fitting/Params.h>

// std includes
#include <map>
#include <string>

namespace askap {

namespace scimath {

// forward declarations
class DesignMatrix;

/// @brief Normal equations without any approximation
/// @details There are two kinds of normal equations currently supported. The
/// first one is a generic case, where the full normal matrix is retained. It
/// is used for calibration. The second one is intended for imaging, where we
/// can't afford to keep the whole normal matrix. In this approach, the matrix
/// is approximated by a sum of diagonal and shift invariant matrices. This
/// class represents the generic case, where no approximation to the normal
/// matrix is done.
/// @ingroup fitting
struct GenericNormalEquations : public INormalEquations {
      
  /// @brief a default constructor
  /// @details It creates an empty normal equations class
  GenericNormalEquations();
  
  /// @brief copy constructor
  /// @details It is required because this class has non-trivial types (std containers
  /// of casa containers)
  /// @param[in] src other class
  GenericNormalEquations(const GenericNormalEquations &src);
  
  /// @brief assignment operator
  /// @details It is required because this class has non-trivial types (std containers
  /// of casa containers)
  /// @param[in] src other class
  /// @return reference to this object
  GenericNormalEquations& operator=(const GenericNormalEquations &src);  
  
  /// @brief constructor from a design matrix
  /// @details This version of the constructor is equivalent to an
  /// empty constructor plus a call to add method with the given
  /// design matrix
  /// @param[in] dm Design matrix to use
  explicit GenericNormalEquations(const DesignMatrix& dm);
      
  /// @brief reset the normal equation object
  /// @details After a call to this method the object has the same pristine
  /// state as immediately after creation with the default constructor
  virtual void reset();
          
  /// @brief Clone this into a shared pointer
  /// @details "Virtual constructor" - creates a copy of this object. Derived
  /// classes must override this method to instantiate the object of a proper 
  /// type.
  /// @return shared pointer on INormalEquation class
  virtual GenericNormalEquations::ShPtr clone() const;

  /// @brief Merge these normal equations with another
  /// @details Combining two normal equations depends on the actual class type
  /// (different work is required for a full matrix and for an approximation).
  /// This method must be overriden in the derived classes for correct 
  /// implementation. 
  /// This means that we just add
  /// @param[in] src an object to get the normal equations from
  virtual void merge(const INormalEquations& src);
  
  /// @brief Add a design matrix to the normal equations
  /// @details This method computes the contribution to the normal matrix 
  /// using a given design matrix and adds it.
  /// @param[in] dm Design matrix to use
  void add(const DesignMatrix& dm);
  
  /// @brief add special type of design equations formed as a matrix product
  /// @details This method adds design equations formed by a product of
  /// a certain CompleDiffMatrix and a vector. It is equivalent to adding a design
  /// matrix formed from the result of this product. However, bypassing design 
  /// matrix allows to delay calculation of contributions to the normal matrix and
  /// use buffer cross terms (between model and measured visibilities, which are
  /// expected to be a part of the vector cdm is multiplied to) separately. This
  /// is used for pre-averaging (or pre-summing to be exact) calibration. The 
  /// cross-products of visibilities are tracked using the PolXProduct object
  /// @param[in] cdm matrix with derivatives and values (to be multiplied to a 
  /// vector represented by cross-products given in the second parameter). Should be
  /// a square matrix of npol x npol size.
  /// @param[in] pxp cross-products (model by measured and model by model, where 
  /// measured is the vector cdm is multiplied to).
  void add(const ComplexDiffMatrix &cdm, const PolXProducts &pxp);
    
  /// @brief add normal matrix for a given parameter
  /// @details This means that the cross terms between parameters 
  /// are excluded. However the terms inside a parameter are retained.
  /// @param[in] name Name of the parameter
  /// @param[in] normalmatrix Normal Matrix for this parameter
  /// @param[in] datavector Data vector for this parameter
  void add(const string& name, const casacore::Matrix<double>& normalmatrix,
                               const casacore::Vector<double>& datavector);
  
  /// @brief normal equations for given parameters
  /// @details In the current framework, parameters are essentially 
  /// vectors, not scalars. Each element of such vector is treated
  /// independently (but only vector as a whole can be fixed). As a 
  /// result any element of the normal matrix is another matrix for
  /// all non-scalar parameters. For scalar parameters each such
  /// matrix has a shape of [1,1].
  /// @param[in] par1 the name of the first parameter
  /// @param[in] par2 the name of the second parameter
  /// @return one element of the sparse normal matrix (a dense matrix)
  virtual const casacore::Matrix<double>& normalMatrix(const std::string &par1, 
                        const std::string &par2) const;

  /// @brief Returns iterator to the beginning of normal matrix row, defined by input parameter.
  /// @param[in] par the name of the parameter describing the matrix row
  std::map<string, casa::Matrix<double> >::const_iterator getNormalMatrixRowBegin(const std::string &par) const;

  /// @brief Returns iterator to the end of normal matrix row, defined by input parameter.
  /// @details It is used together with getNormalMatrixRowBegin() to iterate through the (sparse) matrix elements.
  /// @param[in] par the name of the parameter describing the matrix row
  std::map<string, casa::Matrix<double> >::const_iterator getNormalMatrixRowEnd(const std::string &par) const;

  /// @brief Returns the number of (scalar) elements in the normal matrix.
  /// @details This should be close to the number of non zero elements,
  /// depending if the elements-matrices (casa::Matrix) have non diagonal nonzero elements (or leakages).
  size_t getNumberElements() const;

  /// @brief data vector for a given parameter
  /// @details In the current framework, parameters are essentially 
  /// vectors, not scalars. Each element of such vector is treated
  /// independently (but only vector as a whole can be fixed). As a 
  /// result any element of the normal matrix as well as an element of the
  /// data vector are, in general, matrices, not scalar. For the scalar 
  /// parameter each element of data vector is a vector of unit length.
  /// @param[in] par the name of the parameter of interest
  /// @return one element of the sparse data vector (a dense vector)     
  virtual const casacore::Vector<double>& dataVector(const std::string &par) const;
  
  /// @brief write the object to a blob stream
  /// @param[in] os the output stream
  virtual void writeToBlob(LOFAR::BlobOStream& os) const;

  /// @brief read the object from a blob stream
  /// @param[in] is the input stream
  /// @note Not sure whether the parameter should be made const or not 
  virtual void readFromBlob(LOFAR::BlobIStream& is); 

  /// @brief obtain all parameters dealt with by these normal equations
  /// @details Normal equations provide constraints for a number of 
  /// parameters (i.e. unknowns of these equations). This method returns
  /// a vector with the string names of all parameters mentioned in the
  /// normal equations represented by the given object.
  /// @return a vector listing the names of all parameters (unknowns of these equations)
  /// @note if ASKAP_DEBUG is set some extra checks on consistency of these 
  /// equations are done
  virtual std::vector<std::string> unknowns() const; 

  /// @brief obtain reference to metadata
  /// @details It is handy to have key=value type metadata transported along with the
  /// normal equations. The main applications are the metadata for calibration parameters
  /// (i.e. time the solution corresponds to) and parameters which could assist merging of
  /// the normal equations in a more intelligent way in the imaging case. At this stage,
  /// calibration is the main driver. Therefore, this data field and handling methods are
  /// implemented only for GenericNormalEquations. We could move this up in the hierarchy
  /// and even expose access methods via the main interface.
  /// @return reference to metadata
  inline Params& metadata() { return itsMetadata;}
  
  /// @brief obtain const reference to metadata
  /// @details This is a const version of the metadata() method
  /// @return const reference to metadata
  inline const Params& metadata() const { return itsMetadata; }

protected:
  /// @brief map of matrices (data element of each row map)
  typedef std::map<std::string, casacore::Matrix<double> > MapOfMatrices;
  /// @brief map of vectors (data vectors for all parameters)
  typedef std::map<std::string, casacore::Vector<double> > MapOfVectors;

  /// @brief Add one parameter from another normal equations class
  /// @details This helper method is used in merging of two normal equations.
  /// It processes just one parameter.
  /// @param[in] par name of the parameter to copy
  /// @param[in] src an object to get the normal equations from
  /// @note This helper method works with instances of this class only (as
  /// only then it knows how the actual normal matrix is handled). One could
  /// have a general code which would work for every possible normal equation,
  /// but in some cases it would be very inefficient. Therefore, the decision
  /// has been made to throw an exception if incompatible operation is requested
  /// and add the code to handle this situation later, if it appears to be 
  /// necessary.
  void mergeParameter(const std::string &par, const GenericNormalEquations& src);
  
  /// @brief Add/update one parameter using given matrix and data vector
  /// @details This helper method is the main workhorse used in merging two
  /// normal equations, adding an independent parameter or a design matrix.
  /// The normal matrix to be integrated with this class is given in the form
  /// of map of matrices (effectively a sparse matrix). Each element of the map
  /// corresponds to a cross- or parallel term in the normal equations. Data
  /// vector is given simply as a casacore::Vector, rather than the map of vectors,
  /// because only one parameter is concerned here. If a parameter with the given
  /// name doesn't exist, the method adds it to both normal matrix and data vector,
  /// populating correctly all required cross-terms with 0-matrices of an 
  /// appropriate shape.
  /// @param[in] par name of the parameter to work with
  /// @param[in] inNM input normal matrix
  /// @param[in] inDV input data vector 
  void addParameter(const std::string &par, const MapOfMatrices &inNM,
                    const casa::Vector<double>& inDV);

  /// @brief Add/update one parameter to/in sparse matrix, using given matrix and data vector.
  /// @details Similar to addParameter, but does not initialize the full matrix.
  /// Instead, only elements (cross-terms) used in calculations get initialized,
  /// essentially allowing for building a sparse matrix.
  /// @param[in] par name of the parameter to work with
  /// @param[in] inNM input normal matrix
  /// @param[in] inDV input data vector
  void addParameterSparsely(const std::string &par, const MapOfMatrices &inNM,
                            const casa::Vector<double>& inDV);

  /// @brief extract dimension of a parameter from the given row
  /// @details This helper method analyses the matrices stored in the supplied
  /// map (effectively a row of a sparse matrix) and extracts the dimension of
  /// the parameter this row corresponds to. If compiled with ASKAP_DEBUG, 
  /// this method does an additional consistency check that all elements of
  /// the sparse matrix give the same dimension (number of rows is the same for
  /// all elements).
  /// @param[in] nmRow a row of the sparse normal matrix to work with
  /// @return dimension of the corresponding parameter
  static casa::uInt parameterDimension(const MapOfMatrices &nmRow);

  /// @brief Calculate an element of A^tA
  /// @details Each element of a sparse normal matrix is also a matrix
  /// in general. However, due to some limitations of CASA operators, a
  /// separate treatment is required for degenerate cases. This method
  /// calculates an element of the normal matrix (effectively an element of
  /// a product of A transposed and A, where A is the whole design matrix)
  /// @param[in] matrix1 the first element of a sparse normal matrix
  /// @param[in] matrix2 the second element of a sparse normal matrix
  /// @return a product of matrix1 transposed to matrix2
  static casacore::Matrix<double> nmElement(const casacore::Matrix<double> &matrix1,
               const casacore::Matrix<double> &matrix2);
  
  /// @brief Calculate an element of A^tB
  /// @details Each element of a sparse normal matrix is also a matrix
  /// in general. However, due to some limitations of CASA operators, a
  /// separate treatment is required for degenerate cases. This method
  /// calculates an element of the right-hand side of the normal equation
  /// (effectively an element of a product of A transposed and the data
  /// vector, where A is the whole design matrix)
  /// @param[in] dm an element of the design matrix
  /// @param[in] dv an element of the data vector
  /// @return element of the right-hand side of the normal equations
  static casa::Vector<double> dvElement(const casa::Matrix<double> &dm,
              const casa::Vector<double> &dv); 

  /// @brief Extract derivatives from design matrix
  /// @details This method extracts an appropriate derivative matrix
  /// from the given design matrix. Effectively, it implements
  /// dm.derivative(par)[dataPoint] with some additional validity checks
  /// @param[in] dm Design matrix to work with
  /// @param[in] par parameter name of interest
  /// @param[in] dataPoint a sequence number of the data point, for which 
  /// the derivatives are returned
  /// @return matrix of derivatives
  static const casacore::Matrix<double>& extractDerivatives(const DesignMatrix &dm,
             const std::string &par, casacore::uInt dataPoint);
  
private:
  // Adding the data vector for a parameter.
  void addDataVector(const std::string &par, const casa::Vector<double>& inDV);

  /// @brief test that all matrix elements are zero
  /// @details This is a helper method to test all matrix elements
  /// @param[in] matrix matrix to test
  /// @return true if all elements are zero
  static bool allMatrixElementsAreZeros(const casa::Matrix<double>& matrix);

  /// @brief normal matrix
  /// @details Normal matrices stored as a map or maps of Matrixes - 
  /// it's really just a big matrix.
  std::map<string, MapOfMatrices> itsNormalMatrix;
  
  /// @brief the data vectors
  /// @details This parameter may eventually go a level up in the class
  /// hierarchy
  MapOfVectors itsDataVector;
  
  /// @brief metadata
  /// @details It is handy to have key=value type metadata transported along with the
  /// normal equations. The main applications are the metadata for calibration parameters
  /// (i.e. time the solution corresponds to) and parameters which could assist merging of
  /// the normal equations in a more intelligent way in the imaging case. At this stage,
  /// calibration is the main driver. Therefore, this data field and handling methods are
  /// implemented only for GenericNormalEquations. We could move this up in the hierarchy
  /// and even expose access methods via the main interface.
  Params itsMetadata; 
};

} // namespace scimath

} // namespace askap

#endif // #ifndef GENERIC_NORMAL_EQUATIONS_H
