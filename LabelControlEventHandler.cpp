#include "StdAfx.h"
#include "LabelControlEventHandler.h"

CLabelControlEventHandler::CLabelControlEventHandler(osgEarth::Util::Controls::LabelControl* viewLabel, 
	osgEarth::Util::Controls::LabelControl* mouseLabel,
	osgEarth::MapNode* mn)
{
	viewCoords = viewLabel;
	mouseCoords = mouseLabel;
	mapNode = mn;
	nodePath.push_back(mapNode->getTerrainEngine());
}


CLabelControlEventHandler::~CLabelControlEventHandler(void)
{
}

bool CLabelControlEventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	osgViewer::Viewer *viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
	if(viewer)
	{
		if(ea.getEventType() == ea.MOVE || ea.getEventType() == ea.DRAG)
		{
			osgUtil::LineSegmentIntersector::Intersections results;
			if(viewer->computeIntersections(ea.getX(), ea.getY(), nodePath, results))
			{
				osgUtil::LineSegmentIntersector::Intersection first = *(results.begin());
				//取出交点坐标
				osg::Vec3d point = first.getWorldIntersectPoint();
				osg::Vec3d lla;
				
			

				//mapNode->getMap()->worldPointToMapPoint(point, lla);
				mapNode->getMapSRS()->getEllipsoid()->convertLatLongHeightToXYZ( point.x(), point.y(), point.z(), lla.x(), lla.y(), lla.z());
				char wsrc[512];
				sprintf(wsrc, "Lon:%.2f Lat:%.2f Hei:%.2f", lla.x(), lla.y(), lla.z());
				mouseCoords->setText(wsrc);
			}

			osgEarth::Util::EarthManipulator * em = dynamic_cast<osgEarth::Util::EarthManipulator*>(viewer->getCameraManipulator());
			if(em)
			{
				osgEarth::Viewpoint vp = em->getViewpoint();
				char wsrc[512];
				sprintf(wsrc, "Lon:%.2f Lat:%.2f Hei:%.2f", vp.getHeading(), vp.getPitch(), vp.getRange());
				viewCoords->setText(wsrc);
			}
		}
	}
	return false;
}
