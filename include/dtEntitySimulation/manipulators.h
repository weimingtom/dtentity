#pragma once

/*
* dtEntity Game and Simulation Engine
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* Martin Scheffler
*/


#include <dtEntitySimulation/export.h>
#include <osgManipulator/Dragger>
#include <osgManipulator/TabBoxDragger>
#include <osgManipulator/TabBoxTrackballDragger>
#include <osgManipulator/TabPlaneDragger>
#include <osgManipulator/TabPlaneTrackballDragger>
#include <osgManipulator/TrackballDragger>
#include <osgManipulator/Translate1DDragger>
#include <osgManipulator/Translate2DDragger>
#include <osgManipulator/TranslateAxisDragger>

namespace dtEntitySimulation
{

   class TerrainTranslateDragger : public osgManipulator::CompositeDragger
   {

   public:

      TerrainTranslateDragger();
      virtual void setupDefaultGeometry();

   private:
      osg::ref_ptr<osgManipulator::Translate1DDragger> _xDragger;
      osg::ref_ptr<osgManipulator::Translate1DDragger> _yDragger;
      osg::ref_ptr<osgManipulator::Translate1DDragger> _zDragger;
      osg::ref_ptr<osgManipulator::Translate2DDragger > _translate2DDragger;
   };
}