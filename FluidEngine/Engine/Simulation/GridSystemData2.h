#pragma once

#include <memory>
#include <vector>

#include "../Geometry/ScalarGrid2.h"
#include "../Geometry/FaceCenteredGrid2.h"

namespace Engine
{
	//!
	//! \brief      2-D grid system data.
	//!
	//! This class is the key data structure for storing grid system data. To
	//! represent a grid system for fluid simulation, velocity field is defined as a
	//! face-centered (MAC) grid by default. It can also have additional scalar or
	//! vector attributes by adding extra data layer.
	//!
	class GridSystemData2
	{
	public:
		//! Constructs empty grid system.
		GridSystemData2();

		//!
		//! \brief      Constructs a grid system with given resolution, grid spacing
		//!             and origin.
		//!
		//! This constructor builds the entire grid layers within the system. Note,
		//! the resolution is the grid resolution, not the data size of each grid.
		//! Depending on the layout of the grid, the data point may lie on different
		//! part of the grid (vertex, cell-center, or face-center), thus can have
		//! different array size internally. The resolution of the grid means the
		//! grid cell resolution.
		//!
		//! \param[in]  resolution  The resolution.
		//! \param[in]  gridSpacing The grid spacing.
		//! \param[in]  origin      The origin.
		//!
		GridSystemData2(const Size2& resolution, const Vector2D& gridSpacing, const Vector2D& origin);

		//! Copy constructor.
		GridSystemData2(const GridSystemData2& other);

		//! Destructor.
		virtual ~GridSystemData2();

		//!
		//! \brief      Resizes the whole system with given resolution, grid
		//!     spacing, and origin.
		//!
		//! This function resizes the entire grid layers within the system. Note,
		//! the resolution is the grid resolution, not the data size of each grid.
		//! Depending on the layout of the grid, the data point may lie on different
		//! part of the grid (vertex, cell-center, or face-center), thus can have
		//! different array size internally. The resolution of the grid means the
		//! grid cell resolution.
		//!
		//! \param[in]  resolution  The resolution.
		//! \param[in]  gridSpacing The grid spacing.
		//! \param[in]  origin      The origin.
		//!
		void resize(const Size2& resolution, const Vector2D& gridSpacing, const Vector2D& origin);

		//!
		//! \brief      Returns the resolution of the grid.
		//!
		//! This function resizes the entire grid layers within the system. Note,
		//! the resolution is the grid resolution, not the data size of each grid.
		//! Depending on the layout of the grid, the data point may lie on different
		//! part of the grid (vertex, cell-center, or face-center), thus can have
		//! different array size internally. The resolution of the grid means the
		//! grid cell resolution.
		//!
		//! \return     Grid cell resolution.
		//!
		Size2 resolution() const;

		//! Return the grid spacing.
		Vector2D gridSpacing() const;

		//! Returns the origin of the grid.
		Vector2D origin() const;

		//! Returns the bounding box of the grid.
		BoundingBox2D boundingBox() const;

		//!
		//! \brief      Adds a non-advectable scalar data grid by passing its
		//!     builder and initial value.
		//!
		//! This function adds a new scalar data grid. This layer is not advectable,
		//! meaning that during the computation of fluid flow, this layer won't
		//! follow the flow. For the future access of this layer, its index is
		//! returned.
		//!
		//! \param[in]  builder    The grid builder.
		//! \param[in]  initialVal The initial value.
		//!
		//! \return     Index of the data.
		//!
		size_t addScalarData(const ScalarGridBuilder2Ptr& builder, double initialVal = 0.0);

		//!
		//! \brief      Adds a non-advectable vector data grid by passing its
		//!     builder and initial value.
		//!
		//! This function adds a new vector data grid. This layer is not advectable,
		//! meaning that during the computation of fluid flow, this layer won't
		//! follow the flow. For the future access of this layer, its index is
		//! returned.
		//!
		//! \param[in]  builder    The grid builder.
		//! \param[in]  initialVal The initial value.
		//!
		//! \return     Index of the data.
		//!
		size_t addVectorData(const VectorGridBuilder2Ptr& builder, const Vector2D& initialVal = Vector2D());

		//!
		//! \brief      Adds an advectable scalar data grid by passing its builder
		//!     and initial value.
		//!
		//! This function adds a new scalar data grid. This layer is advectable,
		//! meaning that during the computation of fluid flow, this layer will
		//! follow the flow. For the future access of this layer, its index is
		//! returned.
		//!
		//! \param[in]  builder    The grid builder.
		//! \param[in]  initialVal The initial value.
		//!
		//! \return     Index of the data.
		//!
		size_t addAdvectableScalarData(const ScalarGridBuilder2Ptr& builder, double initialVal = 0.0);

		//!
		//! \brief      Adds an advectable vector data grid by passing its builder
		//!     and initial value.
		//!
		//! This function adds a new vector data grid. This layer is advectable,
		//! meaning that during the computation of fluid flow, this layer will
		//! follow the flow. For the future access of this layer, its index is
		//! returned.
		//!
		//! \param[in]  builder    The grid builder.
		//! \param[in]  initialVal The initial value.
		//!
		//! \return     Index of the data.
		//!
		size_t addAdvectableVectorData(const VectorGridBuilder2Ptr& builder, const Vector2D& initialVal = Vector2D());

		//!
		//! \brief      Returns the velocity field.
		//!
		//! This class has velocify field by default, and it is part of the
		//! advectable vector data list.
		//!
		//! \return     Pointer to the velocity field.
		//!
		const FaceCenteredGrid2Ptr& velocity() const;

		//!
		//! \brief      Returns the index of the velocity field.
		//!
		//! This class has velocify field by default, and it is part of the
		//! advectable vector data list. This function returns the index of the
		//! velocity field from the list.
		//!
		//! \return     Index of the velocity field.
		//!
		size_t velocityIndex() const;

		//! Returns the non-advectable scalar data at given index.
		const ScalarGrid2Ptr& scalarDataAt(size_t idx) const;

		//! Returns the non-advectable vector data at given index.
		const VectorGrid2Ptr& vectorDataAt(size_t idx) const;

		//! Returns the advectable scalar data at given index.
		const ScalarGrid2Ptr& advectableScalarDataAt(size_t idx) const;

		//! Returns the advectable vector data at given index.
		const VectorGrid2Ptr& advectableVectorDataAt(size_t idx) const;

		//! Returns the number of non-advectable scalar data.
		size_t numberOfScalarData() const;

		//! Returns the number of non-advectable vector data.
		size_t numberOfVectorData() const;

		//! Returns the number of advectable scalar data.
		size_t numberOfAdvectableScalarData() const;

		//! Returns the number of advectable vector data.
		size_t numberOfAdvectableVectorData() const;

	private:
		Vector2D _origin;
		Size2 _resolution;
		Vector2D _gridSpacing;

		size_t _velocityIdx;
		FaceCenteredGrid2Ptr _velocity;
		std::vector<ScalarGrid2Ptr> _scalarDataList;
		std::vector<VectorGrid2Ptr> _vectorDataList;
		std::vector<ScalarGrid2Ptr> _advectableScalarDataList;
		std::vector<VectorGrid2Ptr> _advectableVectorDataList;

	};

	//! Shared pointer type of GridSystemData2.
	typedef std::shared_ptr<GridSystemData2> GridSystemData2Ptr;

}

